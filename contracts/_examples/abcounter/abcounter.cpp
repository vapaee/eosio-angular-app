#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

class [[eosio::contract]] abcounter : public eosio::contract {
  public:
    using contract::contract;

    abcounter(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}
    
    ACTION count(name user, std::string type) {
        print("abcounter.count() user: ", user, "type: ", type.c_str(), " | ");
      require_auth( name("addressbook"));
      count_index counts(name(_code), _code.value);
      auto iterator = counts.find(user.value);
      
      if (iterator == counts.end()) {
        counts.emplace("addressbook"_n, [&]( auto& row ) {
          row.key = user;
          row.emplaced = (type == "emplace") ? 1 : 0;
          row.modified = (type == "modify") ? 1 : 0;
          row.erased = (type == "erase") ? 1 : 0;
        });
      }
      else {
        counts.modify(iterator, "addressbook"_n, [&]( auto& row ) {
          if(type == "emplace") { row.emplaced += 1; }
          if(type == "modify") { row.modified += 1; }
          if(type == "erase") { row.erased += 1; }
        });
      }
    }

  private:
    TABLE counter {
      name key;
      uint64_t emplaced;
      uint64_t modified;
      uint64_t erased;
      uint64_t primary_key() const { return key.value; }
    };

    using count_index = eosio::multi_index<"counts"_n, counter>;
};

EOSIO_DISPATCH( abcounter, (count));
