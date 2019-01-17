#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug_asset.hpp>
#include <vapaee/datatypes.hpp>
#include <vapaee/vapaee_utils.hpp>

#define inventary_default_space 8
#define NULL_MASTERY 0xFFFFFFFFFFFFFFFF

#include "boardgamebox.tables.hpp"
#include "boardgamebox.core.hpp"


using namespace eosio;

namespace vapaee {

using namespace bgbox;

CONTRACT boardgamebox : public eosio::contract {

    private:

    public:
        using contract::contract;

        // --------------------- Apps ------------------
        ACTION newapp(name owner, name contract, uint64_t app, int invespace) {
            print("\nACTION newapp()\n");
            bgbox::core box;
            box.action_new_app(owner, contract, app, invespace);
        }

        ACTION newitem(name author_owner, uint64_t author_app, name nickname, int maxgroup) {
            bgbox::core box;
            box.action_new_item_spec(author_owner, author_app, nickname, maxgroup);            
        };

        ACTION newcontainer(name author_owner, uint64_t author_app, name nickname, int space) {
            print("\nACTION newcontainer()\n");
            bgbox::core box;
            box.action_new_container_spec(author_owner, author_app, nickname, space);          
        };

        // --------------------- Publishers ------------------
        ACTION newasset(name author_owner, uint64_t author_issuer, slug_asset maximum_supply, uint64_t spec) {
            bgbox::core box;
            box.action_new_item_asset(author_owner, author_issuer, maximum_supply, spec);
        };

        ACTION newinventory(name author_owner, uint64_t author_publisher, slug container_slug, int space, uint64_t spec) {
            print("\nACTION newinventory()\n");
            bgbox::core box;
            box.action_new_container_asset(author_owner, author_publisher, container_slug, space, spec);
        };
        
        ACTION issueunits(name to, slug_asset quantity, string memo) {
            bgbox::core box;
            box.action_issue_item_units(to, quantity, memo);
        };

        // --------------------- Users ------------------
        ACTION transfer( name from, name to, slug_asset quantity, string memo) {
            bgbox::core box;
            box.action_transfer_item_unit_quantity(from, to, quantity, memo);
        };

        ACTION transfunits( name from, name to, slug_asset quantity, const vector<uint64_t> &items,const vector<int> &quantities, string memo ) {
            bgbox::core box;
            box.action_transfer_item_unit_list(from, to, quantity, items, quantities, memo);
        };

        ACTION newusercont (name user, slug container_asset, name ram_payer) {
            bgbox::core box;
            box.action_new_container_instance(user, container_asset, ram_payer);            
        }

        ACTION newuser4app (name user, uint64_t app, name ram_payer) {
            bgbox::core box;
            box.action_create_containers_for_user(user, app, ram_payer);              
        };
        
        ACTION swap( name from, uint64_t unit, uint64_t app, int quantity, name to, const slotinfo &target_slot, name ram_payer) {
            bgbox::core box;
            box.action_swap_slots_of_item_unit(from, unit, app, quantity, to, target_slot, ram_payer);
        }

        ACTION open( name owner, const slug_symbol& symbol, name ram_payer ) {
        };

        ACTION close( name owner, const slug_symbol& symbol ) {
        };

        ACTION burn( name owner, const slug_asset& quantity, slotinfo & slot) {
        };

        // -------------------- debugging porpuses ---------------------
        void dropuser(name owner) {
            // para poder desarrollar más fácil
            users user_table(get_self(), get_self().value);
            auto itr = user_table.find(owner.value);
            eosio_assert(itr != user_table.end(), (string("user not found: ") + owner.to_string()).c_str());
            user_table.erase(itr);
            // TODO: falta borrar todas sus cosas: items, containers, experiences, etc...

            // borro todos los containers definidos por este usuario
            containers cont_table(get_self(), owner.value);
            for (auto itr = cont_table.begin(); itr != cont_table.end(); ) {
                itr = cont_table.erase(itr);
            }

            // borro todos las experiences definidos por este usuario
            experiences expt_table(get_self(), owner.value);
            for (auto itr = expt_table.begin(); itr != expt_table.end(); ) {
                itr = expt_table.erase(itr);
            }

        };

        ACTION droptables() {
            name user;
            uint64_t author;
            users user_table(get_self(), get_self().value);
            for (auto itr = user_table.begin(); itr != user_table.end(); itr = user_table.begin()) {
                user = itr->user;
                dropuser(user);

                vector<uint64_t> list;
                list.clear(); // no se, por las dudas
                vapaee::get_authors_for_owner(user, list);
                for (int i=0; i<list.size(); i++) {
                    author = list[i];

                    // borro todos los item_units definidos para esta app
                    item_units units_table(get_self(), author);
                    for (auto itr = units_table.begin(); itr != units_table.end(); ) {
                        itr = units_table.erase(itr);
                    }                    

                    // borro todos los containers definidos por esta app
                    container_specs cont_table(get_self(), get_self().value);
                    auto index_cont = cont_table.template get_index<"app"_n>();
                    for (auto itr = index_cont.lower_bound(author); itr != index_cont.end() && itr->app == author;) {
                        itr = index_cont.erase(itr);
                    }

                    // borro todos los containers definidos por esta app
                    container_assets contasset_table(get_self(), get_self().value);
                    auto index_contasset = contasset_table.template get_index<"publisher"_n>();
                    for (auto itr = index_contasset.lower_bound(author); itr != index_contasset.end() && itr->publisher == author;) {
                        itr = index_contasset.erase(itr);
                    }                    

                    // borro todos los items_spec definidos por esta app
                    item_specs spec_table(get_self(), get_self().value);
                    auto index_spec = spec_table.template get_index<"app"_n>();
                    for (auto itr = index_spec.lower_bound(author); itr != index_spec.end() && itr->app == author;) {
                        itr = index_spec.erase(itr);
                    }

                    // borro todos los items_asset definidos por este publisher
                    item_assets assets_table(get_self(), get_self().value);
                    auto index_asset = assets_table.template get_index<"publisher"_n>();
                    for (auto itr = index_asset.lower_bound(author); itr != index_asset.end() && itr->publisher == author;) {
                        itr = index_asset.erase(itr);
                    }

                    // borro todos los mastery_spec definidos por esta app
                    mastery_specs mastery_table(get_self(), get_self().value);
                    auto index_mty = mastery_table.template get_index<"app"_n>();
                    for (auto itr = index_mty.lower_bound(author); itr != index_mty.end() && itr->app == author;) {
                        // borro todos los mastery_prop definidos por este mastery_spec
                        mastery_props props_table(get_self(), itr->id);
                        for (auto prop_itr = props_table.begin(); prop_itr != props_table.end();) {
                            prop_itr = props_table.erase(prop_itr);
                        }

                        itr = index_mty.erase(itr);
                    }
                }

            }
        };



/*
    -- ACTIONS for developers --
    - registerPublisher(owner, publisher, prefix, isapp): registra un publisher co un pefix (que deberá usar en el slug de las cartas y albums que publique)
    - addInventory(owner, publisher, settings...) // solo si publisher isapp
    - registerItemSpec(publisher, associated_app, settings...)
    - registerContainerSpec(publisher, settings...)

    -- ACTIONS for clients --
    - registerItemSpec(publisher, "cardsntokens", settings...) // se repite porque en C&T sería el cliente que quiere hacer una carta nueva
    - issueItems(publisher, quantity);
    - transferItems(from, to, units); // el destinatario debe tener espacio en su inventario de la aplicación asociada al item
    - moveItem() // swap del contenido de dos slots en mysql


*/


/*

    -- Items & Containers --

    -- Market --
    - subasta, ordenes de compra o venta

    -- Masteries --
    -- Masteries: Auras --
    -- Masteries: Tokens --

    -- Gaming --
    - ranking

    -- Tournament --

    -- Dice -- servicio de randomization con parametros para otros smart contracts.


    */
};

}; // namespace