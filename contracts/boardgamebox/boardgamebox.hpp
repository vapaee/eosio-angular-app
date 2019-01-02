#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <vapaee/slug_asset.hpp>
#include <vapaee/datatypes.hpp>
#include <vapaee/vapaee_aux_functions.hpp>

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
        ACTION newapp(name owner, uint64_t app, int invespace) {
            bgbox::core box;
            box.action_new_app(owner, app, invespace);
        }

        ACTION newitem(name author_owner, uint64_t author_app, name nickname, int maxgroup) {
            bgbox::core box;
            box.action_new_item_spec(author_owner, author_app, nickname, maxgroup);            
        };

        ACTION newcontainer(name author_owner, uint64_t author_app, name nickname, int space) {
            bgbox::core box;
            box.action_new_container_spec(author_owner, author_app, nickname, space);          
        };

        // --------------------- Publishers ------------------
        ACTION newasset(name author_owner, uint64_t author_issuer, slug_asset maximum_supply, uint64_t spec) {
            bgbox::core box;
            box.action_new_item_asset(author_owner, author_issuer, maximum_supply, spec);
        };

        ACTION newinventory(name author_owner, uint64_t author_publisher, slug container_slug, int space, uint64_t spec) {
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
        };

        ACTION droptables() {
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