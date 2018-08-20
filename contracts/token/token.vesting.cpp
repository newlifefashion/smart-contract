/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "token.vesting.hpp"

namespace token {
    using namespace eosio;

    void vesting::unlock(account_name from) {
        require_auth(from);
        time_point_sec time_now = time_point_sec(now());
        vestingassets vassets(_self, from);
        vector<vestingasset> unlocked{};
        for (auto itr = vassets.begin(); itr != vassets.end(); itr++) {
            if (itr->lock_until <= time_now) {
                unlocked.emplace_back(*itr);
            }
        }
        if (unlocked.size() > 0) {
            asset liquid_delta{0, TOKEN_SYMBOL};
            print(liquid_delta);
            for (auto ua : unlocked) {
                liquid_delta += ua.quantity;
                eosio_assert(c(ua.quantity.amount > 0), "vesting asset quantity should be positive");
                vassets.erase(ua);
            }
            accounts from_accounts(_self, from);
            auto from_itr = from_accounts.find(liquid_delta.symbol.name());
            eosio_assert(c(from_itr != from_accounts.end()), "account does not exist");
            from_accounts.modify(from_itr, from, [&](auto &a) {
                a.liquid += liquid_delta;
                eosio_assert(c(a.balance >= a.liquid), "liquid should be greater than balance");
            });
        }
    }

    void vesting::create(account_name issuer,
                         asset maximum_supply) {
        require_auth(_self);

        auto sym = maximum_supply.symbol;
        eosio_assert(c(sym.is_valid()), "invalid symbol name");
        eosio_assert(c(maximum_supply.is_valid()), "invalid supply");
        eosio_assert(c(maximum_supply.amount > 0), "max-supply must be positive");

        stats statstable(_self, sym.name());
        auto existing = statstable.find(sym.name());
        eosio_assert(c(existing == statstable.end()), "token with symbol already exists");

        statstable.emplace(_self, [&](auto &s) {
            s.supply.symbol = maximum_supply.symbol;
            s.max_supply = maximum_supply;
            s.issuer = issuer;
            s.base_lock = time_point_sec(now());
        });
    }


    void vesting::issue(account_name to, asset quantity, string memo) {
        auto sym = quantity.symbol;
        eosio_assert(c(sym.is_valid()), "invalid symbol name");
        eosio_assert(c(memo.size() <= 256), "memo has more than 256 bytes");

        auto sym_name = sym.name();
        stats statstable(_self, sym_name);
        auto existing = statstable.find(sym_name);
        eosio_assert(c(existing != statstable.end()), "token with symbol does not exist, create token before issue");
        const auto &st = *existing;

        require_auth(st.issuer);
        eosio_assert(c(quantity.is_valid()), "invalid quantity");
        eosio_assert(c(quantity.amount > 0), "must issue positive quantity");

        eosio_assert(c(quantity.symbol == st.supply.symbol), "symbol precision mismatch");
        eosio_assert(c(quantity.amount <= st.max_supply.amount - st.supply.amount), "quantity exceeds available supply");

        statstable.modify(st, 0, [&](auto &s) {
            s.supply += quantity;
        });

        add_balance(st.issuer, quantity, st.issuer);

        if (to != st.issuer) {
            SEND_INLINE_ACTION(*this, transfer, { st.issuer, N(active) }, { st.issuer, to, quantity, memo });
        }
    }

    void vesting::transfer(account_name from,
                           account_name to,
                           asset quantity,
                           string memo) {
        eosio_assert(from != to, "cannot transfer to self");
        require_auth(from);
        eosio_assert(is_account(to), "to account does not exist");
        auto sym = quantity.symbol.name();
        stats statstable(_self, sym);
        const auto &st = statstable.get(sym);

        require_recipient(from);
        require_recipient(to);

        eosio_assert(c(quantity.is_valid()), "invalid quantity");
        eosio_assert(c(quantity.amount > 0), "must transfer positive quantity");
        eosio_assert(c(quantity.symbol == st.supply.symbol), "symbol precision mismatch");
        eosio_assert(c(memo.size() <= 256), "memo has more than 256 bytes");


        sub_balance(from, quantity);
        add_balance(to, quantity, from);
    }

    void vesting::grantvesting(account_name from, account_name to, asset quantity, time_point_sec lock_until) {
//        eosio_assert(from != to, "cannot transfer to self");
        require_auth(from);
//        eosio_assert(is_account(to), "to account does not exist");
        auto sym = quantity.symbol.name();
        stats statstable(_self, sym);
        const auto &st = statstable.get(sym);

        eosio_assert(c(quantity.is_valid()), "invalid quantity");
        eosio_assert(c(quantity.amount > 0), "must transfer positive quantity");
        eosio_assert(c(quantity.symbol == st.supply.symbol), "symbol precision mismatch");

        accounts to_accounts(_self, to);
        vestingassets vassets(_self, to);

        vassets.emplace(from, [&](auto &va) {
            va.id = vassets.available_primary_key();
            va.quantity = quantity;
            va.lock_until = lock_until;
        });
        add_vesting(to, quantity, quantity, from);
    }


    void vesting::sub_balance(account_name owner, asset value) {
        accounts from_acnts(_self, owner);

        const auto &from = from_acnts.get(value.symbol.name(), "no balance object found");
        eosio_assert(c(from.balance.amount >= value.amount), "overdrawn balance");


        if (from.balance.amount == value.amount) {
            from_acnts.erase(from);
        } else {
            from_acnts.modify(from, owner, [&](auto &a) {
                a.balance -= value;
            });
        }
    }

    void vesting::add_balance(account_name owner, asset value, account_name ram_payer) {
        accounts to_acnts(_self, owner);
        auto to = to_acnts.find(value.symbol.name());
        if (to == to_acnts.end()) {
            to_acnts.emplace(ram_payer, [&](auto &a) {
                a.balance = value;
            });
        } else {
            to_acnts.modify(to, 0, [&](auto &a) {
                a.balance += value;
            });
        }
    }

    void vesting::sub_vesting(account_name owner, asset balance_delta, asset liquid_delta) {
        accounts from_acnts(_self, owner);

        const auto &from = from_acnts.get(balance_delta.symbol.name(), "no balance object found");
        eosio_assert(static_cast<uint32_t>(from.balance.amount >= balance_delta.amount), "overdrawn balance");


        if (from.balance.amount == balance_delta.amount) {
            from_acnts.erase(from);
        } else {
            from_acnts.modify(from, owner, [&](auto &a) {
                a.balance -= balance_delta;
                a.liquid -= liquid_delta;
            });
        }
    }

    void vesting::add_vesting(account_name owner, asset balance_delta, asset liquid_delta, account_name ram_payer) {
        accounts to_acnts(_self, owner);
        auto to = to_acnts.find(balance_delta.symbol.name());
        if (to == to_acnts.end()) {
            to_acnts.emplace(ram_payer, [&](auto &a) {
                a.balance = balance_delta;
                a.liquid = liquid_delta;
            });
        } else {
            to_acnts.modify(to, 0, [&](auto &a) {
                a.balance += balance_delta;
                a.liquid += liquid_delta;
            });
        }
    }

    uint32_t vesting::c(bool b) {
        return static_cast<uint32_t>(b);
    }

}
