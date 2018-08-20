/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

namespace eosiosystem {
    class system_contract;
}

#define TOKEN_SYMBOL S(4,NEW)

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

        //@abi action
        void grantvesting(account_name from, account_name to, asset quantity, time_point_sec lock_until);

        //@abi action
        void unlock(account_name from);

    private:
        //@abi table
        struct vestingasset {
            uint64_t id;
            asset quantity = asset(0, TOKEN_SYMBOL);
            time_point_sec lock_until;

            uint64_t primary_key() const { return id; }

            EOSLIB_SERIALIZE(vestingasset, (id)(quantity)(lock_until))
        };

        //@abi table accounts i64
        struct account {
            asset balance = asset(0, TOKEN_SYMBOL);
            asset liquid = asset(0, TOKEN_SYMBOL);

            uint64_t primary_key() const { return balance.symbol.name(); }

            EOSLIB_SERIALIZE(account, (balance)(liquid)
            )
        };

        //@abi table stat i64
        struct stat {
            asset supply;
            asset max_supply;
            account_name issuer;
            time_point_sec base_lock = time_point_sec(now());

            uint64_t primary_key() const { return supply.symbol.name(); }

            EOSLIB_SERIALIZE(stat, (supply)(max_supply)(issuer)(base_lock)
            )
        };

        typedef multi_index<N(accounts), account> accounts;
        typedef multi_index<N(stat), stat> stats;
        typedef multi_index<N(vestingasset), vestingasset> vestingassets;

        void sub_balance(account_name owner, asset value);

        void add_balance(account_name owner, asset value, account_name ram_payer);

        void sub_vesting(account_name owner, asset balance_delta, asset liquid_delta);

        void add_vesting(account_name owner, asset balance_delta, asset liquid_delta, account_name ram_payer);

        uint32_t c(bool b);
    };

}

EOSIO_ABI(token::vesting, (create)(issue)
        (transfer)(grantvesting)(unlock))

