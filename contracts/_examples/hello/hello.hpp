#include <eosiolib/eosio.hpp>
// #include "test.hpp"

using namespace eosio;

CONTRACT hello : public eosio::contract {
  public:
      using contract::contract;

      ACTION hi( name user  );
};