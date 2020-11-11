/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/system.h>
#include <eosiolib/serialize.hpp>
#include <string>
#include <string_view>

namespace eosio {

   /**
    *  Wraps a uint64_t to ensure it is only passed to methods that expect a Name2 and
    *  that no mathematical operations occur.  It also enables specialization of print
    *  so that it is printed as a base32 string.
    *
    *  @brief wraps a uint64_t to ensure it is only passed to methods that expect a Name2
    *  @ingroup types
    */
   struct name2 {
   public:
      enum class raw : uint64_t {};

      constexpr name2() : value(0) {}

      constexpr explicit name2( uint64_t v )
      :value(v)
      {}

      constexpr explicit name2( name2::raw r )
      :value(static_cast<uint64_t>(r))
      {}

      constexpr explicit name2( std::string_view str )
      :value(0)
      {
         if( str.size() > 13 ) {
            eosio_assert( false, "string is too long to be a valid name2" );
         }

         auto n = std::min( str.size(), 12u );
         for( decltype(n) i = 0; i < n; ++i ) {
            value <<= 5;
            value |= char_to_value( str[i] );
         }
         value <<= ( 4 + 5*(12 - n) );
         if( str.size() == 13 ) {
            uint64_t v = char_to_value( str[12] );
            if( v > 0x0Full ) {
               eosio_assert(false, "thirteenth character in name2 cannot be a letter that comes after j");
            }
            value |= v;
         }
      }

      /**
       *  Converts a (eosio::name2 style) Base32 symbol into its corresponding value
       *
       *  @brief Converts a (eosio::name2 style) Base32 symbol into its corresponding value
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
            eosio_assert( false, "character is not in allowed character set for name2s" );

         return 0; // control flow will never reach here; just added to suppress warning
      }

      /**
       *  Returns the length of the name2
       */
      constexpr uint8_t length()const {
         constexpr uint64_t mask = 0xF800000000000000ull;

         if( value == 0 )
            return 0;

         uint8_t l = 0;
         uint8_t i = 0;
         for( auto v = value; i < 13; ++i, v <<= 5 ) {
            if( (v & mask) > 0 ) {
               l = i;
            }
         }

         return l + 1;
      }

      /**
       *  Returns the suffix of the name2
       */
      constexpr name2 suffix()const {
         uint32_t remaining_bits_after_last_actual_dot = 0;
         uint32_t tmp = 0;
         for( int32_t remaining_bits = 59; remaining_bits >= 4; remaining_bits -= 5 ) { // Note: remaining_bits must remain signed integer
            // Get characters one-by-one in name2 in order from left to right (not including the 13th character)
            auto c = (value >> remaining_bits) & 0x1Full;
            if( !c ) { // if this character is a dot
               tmp = static_cast<uint32_t>(remaining_bits);
            } else { // if this character is not a dot
               remaining_bits_after_last_actual_dot = tmp;
            }
         }

         uint64_t thirteenth_character = value & 0x0Full;
         if( thirteenth_character ) { // if 13th character is not a dot
            remaining_bits_after_last_actual_dot = tmp;
         }

         if( remaining_bits_after_last_actual_dot == 0 ) // there is no actual dot in the name2 other than potentially leading dots
            return name2{value};

         // At this point remaining_bits_after_last_actual_dot has to be within the range of 4 to 59 (and restricted to increments of 5).

         // Mask for remaining bits corresponding to characters after last actual dot, except for 4 least significant bits (corresponds to 13th character).
         uint64_t mask = (1ull << remaining_bits_after_last_actual_dot) - 16;
         uint32_t shift = 64 - remaining_bits_after_last_actual_dot;

         return name2{ ((value & mask) << shift) + (thirteenth_character << (shift-1)) };
      }

      constexpr operator raw()const { return raw(value); }

      constexpr explicit operator bool()const { return value != 0; }

      /**
       *  Writes the name2 as a string to the provided char buffer
       *
       *
       *  @brief Writes the name2 as a string to the provided char buffer
       *  @pre Appropriate Size Precondition: (begin + 13) <= end and (begin + 13) does not overflow
       *  @pre Valid Memory Region Precondition: The range [begin, end) must be a valid range of memory to write to.
       *  @param begin - The start of the char buffer
       *  @param end - Just past the end of the char buffer
       *  @return char* - Just past the end of the last character written (returns begin if the Appropriate Size Precondition is not satisfied)
       *  @post If the Appropriate Size Precondition is satisfied, the range [begin, returned pointer) contains the string representation of the name2.
       */
      char* write_as_string( char* begin, char* end )const {
         static const char* charmap = ".12345abcdefghijklmnopqrstuvwxyz";
         constexpr uint64_t mask = 0xF800000000000000ull;

         if( (begin + 13) < begin || (begin + 13) > end ) return begin;

         auto v = value;
         for( auto i = 0;   i < 13; ++i, v <<= 5 ) {
            if( v == 0 ) return begin;

            auto indx = (v & mask) >> (i == 12 ? 60 : 59);
            *begin = charmap[indx];
            ++begin;
         }

         return begin;
      }

      std::string to_string()const {
         char buffer[13];
         auto end = write_as_string( buffer, buffer + sizeof(buffer) );
         return {buffer, end};
      }

      /**
       * Equivalency operator. Returns true if a == b (are the same)
       *
       * @brief Equivalency operator
       * @return boolean - true if both provided name2s are the same
       */
      friend constexpr bool operator == ( const name2& a, const name2& b ) {
         return a.value == b.value;
      }

      /**
       * Inverted equivalency operator. Returns true if a != b (are different)
       *
       * @brief Inverted equivalency operator
       * @return boolean - true if both provided name2s are not the same
       */
      friend constexpr bool operator != ( const name2& a, const name2& b ) {
         return a.value != b.value;
      }

      /**
       * Less than operator. Returns true if a < b.
       * @brief Less than operator
       * @return boolean - true if name2 `a` is less than `b`
       */
      friend constexpr bool operator < ( const name2& a, const name2& b ) {
         return a.value < b.value;
      }


      uint64_t value = 0;

      EOSLIB_SERIALIZE( name2, (value) )
   };
   
   /*
   namespace detail2 {
      template <char... Str>
      struct to_const_char_arr {
         static constexpr const char value[] = {Str...};
      };
   } /// namespace detail
   */
} /// namespace eosio

/**
 * name2 literal operator
 *
 * @brief "foo"_n is a shortcut for name2{"foo"}
 */
/*
template <typename T, T... Str>
inline constexpr eosio::name2 operator""_n() {
   constexpr auto x = eosio::name2{std::string_view{eosio::detail2::to_const_char_arr<Str...>::value, sizeof...(Str)}};
   return x;
}
*/