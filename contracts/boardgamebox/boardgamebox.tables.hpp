#pragma once
#include <eosiolib/eosio.hpp>
#include <vapaee/slug_asset.hpp>
#include <vapaee/datatypes.hpp>

using namespace eosio;
namespace vapaee {
    namespace bgbox {

        // TABLE account (balances) -----------
            // scope: user
            // row: dice cuantas unidades tiene el usuario de determinado item identificado por un slug
            TABLE account { 
                uint64_t           id;
                uint64_t        asset; // item_asset.id
                slug_asset    balance; // cuantas unidades del item identificado por slug tiene el usuario
                uint64_t primary_key() const { return id;  }
                uint128_t slug_key() const { return balance.symbol.code().raw().to128bits(); }
            };
            typedef eosio::multi_index<"accounts"_n, account, 
                indexed_by<"slug"_n, const_mem_fun<account, uint128_t, &account::slug_key>>
            > accounts;
        // ------------------------------------

        // TABLE item_unit: donde están las unidades de items
            // scope: app (unit.asset.spec.app)
            // row: representa un slot conteniendo quantity unidades del item definido por asset en la posición position
            TABLE item_unit {
                uint64_t           id;
                name            owner;
                uint64_t        asset;  // item_asset
                slotinfo         slot; 
                int          quantity;  // quantity <= item_asset.spec.maxgroup
                uint64_t primary_key() const { return id;  }
                uint64_t asset_key() const { return asset;  }
                uint64_t container_key() const { return slot.container; }
                uint128_t slot_key() const { return slot.to128bits(); }
                uint64_t owner_key() const { return slot.container; }
            };
            typedef eosio::multi_index<"items"_n, item_unit,
                indexed_by<"asset"_n, const_mem_fun<item_unit, uint64_t, &item_unit::asset_key>>,
                indexed_by<"container"_n, const_mem_fun<item_unit, uint64_t, &item_unit::container_key>>,
                indexed_by<"slot"_n, const_mem_fun<item_unit, uint128_t, &item_unit::slot_key>>,
                indexed_by<"owner"_n, const_mem_fun<item_unit, uint64_t, &item_unit::owner_key>>
                
            > item_units;
        // containers ------------------------------------

        // TABLE container_spec: Lista de todos los contenedores definidos. Cada app tiene varios contenedores definidos
            // TODAS LAS APPS TIENEN AL MENOS UNA CONTENEDOR LLAMADO INVENTARIO (es una forma de saber si una app está registrada)
            // DEBE DEFINIR UN FILTRO PARA LOS ITEMS QUE SE PUEDEN PONER. dEBE ESPECIFICARSE QUE ITEM_SPEC SE PUEDEN PONER
            // scope: contract
            // row: representa un container definido para la aplicación app con el apodo nick
            TABLE container_spec {
                uint64_t          id;
                name            nick; // el apodo de este container debe ser único para cada app
                uint64_t         app; // table vapaeeaouthor::authors.id
                uint64_t primary_key() const { return id;  }
                uint128_t secondary_key() const { return vapaee::combine(app, nick); }
                uint64_t app_key() const { return app; }
            };
            typedef eosio::multi_index<"contspec"_n, container_spec,
                indexed_by<"second"_n, const_mem_fun<container_spec, uint128_t, &container_spec::secondary_key>>,
                indexed_by<"app"_n, const_mem_fun<container_spec, uint64_t, &container_spec::app_key>>
            > container_specs; 
        // ------------------------------------

        // TABLE container_asset: Lista de todos los CONTAINERS definidos por alguien (los cuales tienen un TIPO de container)
            // scope: contract
            // row: representa un item, del cual existe una cantidad supply de unidades que no supera el max_supply
            TABLE container_asset {
                uint64_t              id;
                slug_asset        supply;
                uint64_t            spec; // table container_spec
                uint64_t       publisher; // table vapaeeaouthor::authors.id
                uint64_t           block;
                int                space; // espacio base que tendrá el container cuando se lo instancie para un usuario. 0 para infinito.
                uint64_t primary_key() const { return id;  }
                uint128_t slug_key() const { return supply.symbol.code().raw().to128bits(); }
                uint64_t publisher_key() const { return publisher; }
            };
            typedef eosio::multi_index<"stat"_n, container_asset,
                indexed_by<"slug"_n, const_mem_fun<container_asset, uint128_t, &container_asset::slug_key>>,
                indexed_by<"publisher"_n, const_mem_fun<container_asset, uint64_t, &container_asset::publisher_key>>
            > container_assets;
        // ------------------------------------  

        // TABLE container_instance: Los contenedores que tiene un usuario
            // scope: user
            // row: representa un container para ese usuario que indica cuantos lugares libres tienen y que espacio total tiene
            TABLE container_instance {
                uint64_t      id;
                uint64_t   asset; // table container_assets.id
                int        empty;
                int        space;
                vector<uint64_t> slots;
                uint64_t primary_key() const { return id;  }
                uint64_t asset_key() const { return asset; }
            };
            typedef eosio::multi_index<"containers"_n, container_instance,
                indexed_by<"asset"_n, const_mem_fun<container_instance, uint64_t, &container_instance::asset_key>>
            > containers;
        // ------------------------------------

        // TABLE user_reg: lista de todos los usuarios que usaron el sistema. es para poder iterar sobre todos los nombres
            // scope: contract
            // row: representa un usuario
            TABLE user_reg {
                name user;
                uint64_t primary_key() const { return user.value;  }
            };
            typedef eosio::multi_index<"users"_n, user_reg> users;
        // ------------------------------------

        // TABLE item_spec: Lista de todos los TIPOS de items que existen. Cada App define varios tipos de items
            // scope: contract
            // row: representa un tipo de item definido por una aplicación app con el nombre nick
            TABLE item_spec { 
                uint64_t          id;
                name            nick; 
                uint64_t         app; // table vapaeeaouthor::authors.id
                int         maxgroup; // MAX (max quantity in same slot), 1 (no agroup), 0 (no limit)
                uint64_t primary_key() const { return id;  }
                uint128_t secondary_key() const { return vapaee::combine(app, nick); }
                uint64_t app_key() const { return app; }
            };
            typedef eosio::multi_index<"itemspec"_n, item_spec,
                indexed_by<"second"_n, const_mem_fun<item_spec, uint128_t, &item_spec::secondary_key>>,
                indexed_by<"app"_n, const_mem_fun<item_spec, uint64_t, &item_spec::app_key>>
            > item_specs;
        // ------------------------------------

        // TABLE item_asset: Lista de todos los ITEMS definidos por alguien (los cuales tienen un TIPO de item)
            // scope: contract
            // row: representa un item, del cual existe una cantidad supply de unidades que no supera el max_supply
            TABLE item_asset {
                uint64_t              id;
                slug_asset        supply;
                slug_asset    max_supply;
                uint64_t            spec; // table item_spec
                uint64_t       publisher; // table vapaeeaouthor::authors.id
                uint64_t           block; 
                uint64_t primary_key() const { return id;  }
                uint128_t slug_key() const { return supply.symbol.code().raw().to128bits(); }
                uint64_t publisher_key() const { return publisher; }
            };
            typedef eosio::multi_index<"stat"_n, item_asset,
                indexed_by<"slug"_n, const_mem_fun<item_asset, uint128_t, &item_asset::slug_key>>,
                indexed_by<"publisher"_n, const_mem_fun<item_asset, uint64_t, &item_asset::publisher_key>>
            > item_assets;
        // ------------------------------------

        // TABLE mastery_spec: lista de todas las maestrías definidas por una app
            // scope: contract
            TABLE mastery_spec {
                uint64_t          id;
                name            nick;
                uint64_t   container; // container_spec
                uint64_t         app; // table vapaeeaouthor::authors.id
                name           table; // table affected: "", "item_spec", "item_asset"
                uint64_t         row; // mastery_prop.id 
                    // "item_spec": esta maestría es para tunear un objeto de tipo descrito en la fila row
                    // "item_asset": esta maestría es para tunear un objeto identificado con un slug item_asset[row].supply.symbol.row()
                uint64_t primary_key() const { return id; }
                uint128_t secondary_key() const { return vapaee::combine(app, nick); }
                uint64_t app_key() const { return app; }
            };
            typedef eosio::multi_index<"mastery"_n, mastery_spec,
                indexed_by<"second"_n, const_mem_fun<mastery_spec, uint128_t, &mastery_spec::secondary_key>>,
                indexed_by<"app"_n, const_mem_fun<mastery_spec, uint64_t, &mastery_spec::app_key>>
            > mastery_specs;
        // ------------------------------------

        // TABLE mastery_prop: lista de properties que tiene una mastery
            // scope: mastery
            // row: representa una property de la maestría
            TABLE mastery_prop {
                uint64_t                   id;
                name                    title; // localstrings.strings.key
                name                     desc; // localstrings.strings.key
                slug                 property;
                iconinfo                 icon;
                std::vector<levelinfo> levels;
                uint64_t primary_key() const { return id;  }
                uint128_t property_key() const { return property.to128bits(); }
            };
            typedef eosio::multi_index<"masteryprop"_n, mastery_prop,
                indexed_by<"property"_n, const_mem_fun<mastery_prop, uint128_t, &mastery_prop::property_key>>
            > mastery_props;
        // ------------------------------------

        // TABLE experience: Los contenedores que tiene un usuario
            // scope: user
            // row: representa un container para ese usuario que indica cuantos lugares libres tienen y que espacio total tiene
            TABLE experience {
                uint64_t         id;
                uint64_t    mastery; // mastery_spec
                uint64_t  container; // container_instance donde están los Mastery-Tokens
                name          table; // table affected: "", "itemunit", "itemasset"
                uint64_t        row; // row.id
                    // "itemunit": esta unidad está tuneada
                    // "itemasset": esta maestría es para tunear un objeto identificado con un slug item_asset[row].supply.symbol.row()
                // TODO: ver la posibilidad de implementar un cache aca
                uint64_t primary_key() const { return id; }
                uint64_t mastery_key() const { return mastery; }
            };
            typedef eosio::multi_index<"experience"_n, experience,
                indexed_by<"mastery"_n, const_mem_fun<experience, uint64_t, &experience::mastery_key>>
            > experiences;
        // ------------------------------------

        // TABLE aura_spec: lista de todas las auras definidas por una app sobre las props de una mastery
            // scope: contract
            TABLE aura_spec {
                uint64_t          id;
                name            nick;
                uint64_t         app; // table vapaeeaouthor::authors.id
                uint64_t     mastery; // tabla mastery_spec.
                uint64_t  acumulable; // indica la cantidad de veces que se puede acumular
                // TODO: implementar los points: lista de tuplas (prop-level-points-value)
                uint64_t primary_key() const { return id; }
                uint128_t secondary_key() const { return vapaee::combine(app, nick); }
                uint64_t app_key() const { return app; }
            };
            typedef eosio::multi_index<"aura"_n, aura_spec,
                indexed_by<"second"_n, const_mem_fun<aura_spec, uint128_t, &aura_spec::secondary_key>>,
                indexed_by<"app"_n, const_mem_fun<aura_spec, uint64_t, &aura_spec::app_key>>
            > aura_specs;
        // ------------------------------------

        // TABLE aura_fx: las auras que tiene prendidas el usuario
            // scope: user
            // row: significa que el usuario tiene activa esta aura quantity veces
            TABLE aura_fx {
                uint64_t         id;
                uint64_t    mastery;   // mastery_spec
                uint64_t  container;   // container_instance
                // TODO: ver la posibilidad de implementar un cache aca
                uint64_t primary_key() const { return id;  }
            };
            typedef eosio::multi_index<"aurafx"_n, aura_fx> aura_fxs;
        // ------------------------------------
        
    };

}; // namespace