/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

namespace eosiosystem {
    class system_contract;
}

namespace token {

    using std::string;
    using namespace eosio;

    class vesting : public contract {
    public:
        explicit vesting(account_name self) : contract(self) {}

        //@abi action
        void create(account_name issuer,
                    asset maximum_supply);

        //@abi action
        void issue(account_name to, asset quantity, string memo);

        //@abi action
        void transfer(account_name from,
                      account_name to,
                      asset quantity,
                      string memo);


        inline asset get_supply(symbol_name sym) const;

        inline asset get_balance(account_name owner, symbol_name sym) const;

    private:
        //@abi table accounts i64
        struct account {
            asset balance;

            uint64_t primary_key() const { return balance.symbol.name(); }
            EOSLIB_SERIALIZE( account, (balance) )
        };

        //@abi table stat i64
        struct stat {
            asset supply;
            asset max_supply;
            account_name issuer;

            uint64_t primary_key() const { return supply.symbol.name(); }
            EOSLIB_SERIALIZE( stat, (supply)(max_supply)(issuer) )
        };

        typedef eosio::multi_index<N(accounts), account> accounts;
        typedef eosio::multi_index<N(stat), stat> stats;

        void sub_balance(account_name owner, asset value);

        void add_balance(account_name owner, asset value, account_name ram_payer);

    public:
        struct transfer_args {
            account_name from;
            account_name to;
            asset quantity;
            string memo;
        };
    };

    asset vesting::get_supply(symbol_name sym) const {
        stats statstable(_self, sym);
        const auto &st = statstable.get(sym);
        return st.supply;
    }

    asset vesting::get_balance(account_name owner, symbol_name sym) const {
        accounts accountstable(_self, owner);
        const auto &ac = accountstable.get(sym);
        return ac.balance;
    }

}

EOSIO_ABI(token::vesting, (create)(issue)(transfer))

