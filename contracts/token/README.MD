## token vesting

This smart contract is designed to be compatible with eosio.token.

TODO:  
- [ ] token creator
  - [x] create token, set max supply and issuer
  - [ ] create vesting structure
  - [x] resign his/her private keys
- [ ] token issuer
  - [ ] create token market
  - [ ] close token market
- [ ] token owner
  - [x] transfer eos to contract
  - [x] grant vesting when buy
  - [x] unlock vesting
  - [ ] select vesting structure
  - [x] transfer to other user
- [ ] unit test with fast time forward
- [ ] integration test
- [ ] resource usage test
- [ ] security audit

## Details
there are three roles in this contract:
1. token creator
2. token issuer
3. token owner (user)

### token creator
token creator should
1. create token by set max supply and issuer
2. create vesting structure
3. resign his/her private keys to make this contract immutable.

### token issuer
token issuer should
1. create token market with token supply, time window and vesting structures
2. close token market after end time of time window

**token issuer can not issue more tokens than max supply created by creator.**

### token owner
token owner should
1. buy token by transfer eos to this contract and select vesting structure
2. call unlock to unlock vesting after locking period
3. transfer to others by transfer function
