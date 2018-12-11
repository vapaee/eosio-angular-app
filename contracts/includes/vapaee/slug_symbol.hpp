/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/system.h>
#include <eosiolib/print.h>
#include <eosiolib/name.hpp>
#include <eosiolib/serialize.hpp>
#include <tuple>
#include <limits>
#include <string_view>

namespace eosio {

  /**
   *  @defgroup slug_symbolapi slug_symbol API
   *  @brief Defines API for managing slug_symbols
   *  @ingroup contractdev
   */

  /**
   *  @defgroup slug_symbolcppapi slug_symbol CPP API
   *  @brief Defines %CPP API for managing slug_symbols
   *  @ingroup slug_symbolapi
   *  @{
   */

   /**
    * \struct Stores the slug_symbol code
    * @brief Stores the slug_symbol code
    */
   class slug_symbol_code {
   public:
      constexpr slug_symbol_code() : value(0) {}

      constexpr explicit slug_symbol_code( uint256_t raw ) : value(raw) {}

      constexpr explicit slug_symbol_code( std::string_view str )
      :value(0)
      {
         eosio_assert( str.size() <= 50, "string is too long to be a valid slug_symbol" );

         auto n = std::min( str.size(), 50u );
         for( decltype(n) i = 0; i < n; ++i ) {
            value <<= 5;
            value |= char_to_value( str[i] );
         }
         // value <<= ( 4 + 5*(12 - n) );
      }

      /**
       *  Converts a (eosio::name style) Base32 symbol into its corresponding value
       *
       *  @brief Converts a (eosio::name style) Base32 symbol into its corresponding value
       *  @param c - Character to be converted
       *  @return constexpr char - Converted value
       */
      static constexpr uint8_t char_to_value( char c ) {
         if( c == '.')
            return 0;
         else if( c >= '1' && c <= '5' )
            return (c - '1') + 1;
         else if( c >= 'a' && c <= 'z' )
            return (c - 'a') + 6;
         else
            eosio_assert( false, (string("") + "character '" + c + "' is not in allowed character set for slug_symbol").c_str() );

         return 0; // control flow will never reach here; just added to suppress warning
      }      

      /**
       * Checks if the slug_symbol code is valid
       * @return true - if slug_symbol is valid
       */
      constexpr bool is_valid() const {
         return true;
      }

      /**
       * Returns the character length of the provided slug_symbol
       *
       * @return length - character length of the provided slug_symbol
       */
      constexpr uint32_t length()const {
         auto sym = value;
         uint32_t len = 0;
         while (sym & 0x1F && len <= 50) {
            len++;
            sym >>= 5;
         }
         return len;
      }

      constexpr uint256_t raw() const { return value; }

      constexpr explicit operator bool()const { return value != 0; }

      /**
       *  Writes the slug_symbol_code as a string to the provided char buffer
       *
       *
       *  @brief Writes the slug_symbol_code as a string to the provided char buffer
       *  @pre Appropriate Size Precondition: (begin + 7) <= end and (begin + 7) does not overflow
       *  @pre Valid Memory Region Precondition: The range [begin, end) must be a valid range of memory to write to.
       *  @param begin - The start of the char buffer
       *  @param end - Just past the end of the char buffer
       *  @return char* - Just past the end of the last character written (returns begin if the Appropriate Size Precondition is not satisfied)
       *  @post If the Appropriate Size Precondition is satisfied, the range [begin, returned pointer) contains the string representation of the slug_symbol_code.
       */
      char* write_as_string( char* begin, char* end )const {
         constexpr uint64_t mask = 0x1Full;

         if( (begin + 50) < begin || (begin + 50) > end ) return begin;

         auto v = value;
         char c;
         for( auto i = 0; i < 50; ++i, v >>= 5 ) {
            if( v == 0 ) return begin;
            c = static_cast<char>(v & mask);
            *begin = c;
            ++begin;
         }

         return begin;
      }

      std::string to_string()const {
         char buffer[50];
         auto end = write_as_string( buffer, buffer + sizeof(buffer) );
         return {buffer, end};
      }

      /**
       * Equivalency operator. Returns true if a == b (are the same)
       *
       * @brief Equivalency operator
       * @return boolean - true if both provided slug_symbol_codes are the same
       */
      friend constexpr bool operator == ( const slug_symbol_code& a, const slug_symbol_code& b ) {
         return a.value == b.value;
      }

      /**
       * Inverted equivalency operator. Returns true if a != b (are different)
       *
       * @brief Inverted equivalency operator
       * @return boolean - true if both provided slug_symbol_codes are not the same
       */
      friend constexpr bool operator != ( const slug_symbol_code& a, const slug_symbol_code& b ) {
         return a.value != b.value;
      }

      /**
       * Less than operator. Returns true if a < b.
       * @brief Less than operator
       * @return boolean - true if slug_symbol_code `a` is less than `b`
       */
      friend constexpr bool operator < ( const slug_symbol_code& a, const slug_symbol_code& b ) {
         return a.value < b.value;
      }

   private:
      uint256_t value = 0;
   };

   /**
    * \struct Stores information about a slug_symbol
    *
    * @brief Stores information about a slug_symbol
    */
   class slug_symbol {
   public:
      constexpr slug_symbol() : value(0) {}

      constexpr explicit slug_symbol( uint256_t raw ) : value(raw) {}

      constexpr slug_symbol( slug_symbol_code sc, uint8_t precision )
      : value( sc.raw() )
      {}

      constexpr slug_symbol( std::string_view ss, uint8_t precision )
      : value( slug_symbol_code(ss).raw() )
      {}

      /**
       * Is this slug_symbol valid
       */
      constexpr bool is_valid()const                 { return code().is_valid(); }

      /**
       * This slug_symbol's precision
       */
      constexpr uint8_t precision()const             { return 0; }

      /**
       * Returns representation of slug_symbol name
       */
      constexpr slug_symbol_code code()const              { return slug_symbol_code{value};   }

      /**
       * Returns uint64_t repreresentation of the slug_symbol
       */
      constexpr uint256_t raw()const                  { return value; }

      constexpr explicit operator bool()const { return value != 0; }

      /**
       * %Print the slug_symbol
       *
       * @brief %Print the slug_symbol
       */
      void print( bool show_precision = true )const {
         if( show_precision ){
            printui( static_cast<uint64_t>(precision()) );
            prints(",");
         }
         char buffer[7];
         auto end = code().write_as_string( buffer, buffer + sizeof(buffer) );
         if( buffer < end )
            prints_l( buffer, (end-buffer) );
      }

      /**
       * Equivalency operator. Returns true if a == b (are the same)
       *
       * @brief Equivalency operator
       * @return boolean - true if both provided slug_symbols are the same
       */
      friend constexpr bool operator == ( const slug_symbol& a, const slug_symbol& b ) {
         return a.value == b.value;
      }

      /**
       * Inverted equivalency operator. Returns true if a != b (are different)
       *
       * @brief Inverted equivalency operator
       * @return boolean - true if both provided slug_symbols are not the same
       */
      friend constexpr bool operator != ( const slug_symbol& a, const slug_symbol& b ) {
         return a.value != b.value;
      }

      /**
       * Less than operator. Returns true if a < b.
       * @brief Less than operator
       * @return boolean - true if slug_symbol `a` is less than `b`
       */
      friend constexpr bool operator < ( const slug_symbol& a, const slug_symbol& b ) {
         return a.value < b.value;
      }

   private:
      uint64_t value = 0;
   };

   // }@ slug_symbolapi

} /// namespace eosio
