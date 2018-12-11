#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

CONTRACT boardbagebox : public eosio::contract {
  public:
      using contract::contract;

      ACTION hi( name user );

      // accessor for external contracts to easily send inline actions to your contract
      using hi_action = action_wrapper<"hi"_n, &boardbagebox::hi>;


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



     
     */
};