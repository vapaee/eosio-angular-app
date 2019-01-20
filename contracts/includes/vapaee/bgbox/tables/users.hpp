#include <vapaee/bgbox/tables/_aux.hpp>

        // TABLE user_reg: lista de todos los publishers (authors que no son apps))
            // scope: contract
            // row: representa un usuario
            TABLE user_reg {
                uint64_t  id;
                string username;
                uint16_t primary_key() const { return id;  }
                uint64_t by_username_key() const {
                    switch (username.length()) {
                        case 0:  return (uint16_t) (0);
                        case 1:  return (uint16_t) (username[0] * 65536);
                        case 2:  return (uint16_t) (username[0] * 65536 + username[1] * 256);
                        default: return (uint16_t) (username[0] * 65536 + username[1] * 256 + username[2]);
                    }
                }
                std::string to_string() const {
                    return std::to_string((int) id) + " - " + username ;
                };
            };
            typedef eosio::multi_index<"users"_n, user_reg,
                indexed_by<"username"_n, const_mem_fun<user_reg, uint64_t, &user_reg::by_username_key>>
            > users;            
        // ------------------------------------