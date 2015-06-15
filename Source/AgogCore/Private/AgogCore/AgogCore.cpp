//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2000 Agog Labs Inc.,
// All rights reserved.
//
//  Common stuff for AgogCore - plus all class data members that require a
//              specific initialization sequence.
// Author(s):    Conan Reis
// Create Date:   2000-01-12
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AgogCore.hpp>
#include <AgogCore/ARandom.hpp>
#include <AgogCore/AStringRef.hpp>
#include <AgogCore/AString.hpp>
#include <AgogCore/ASymbolTable.hpp>
#include <stdio.h>     // Uses: _vsnprintf(), va_list
#include <stdarg.h>    // Uses: va_start, va_end


// These files are not included elsewhere in the AgogCore library, so they are included
// here simply to ensure that they are compiling properly.
#include <AgogCore/ABinaryParse.hpp>
#include <AgogCore/ACompare.hpp>
#include <AgogCore/ACompareMethod.hpp>
#include <AgogCore/AFreePtr.hpp>
#include <AgogCore/AIdPtr.hpp>
#include <AgogCore/AIndexPointer.hpp>
#include <AgogCore/AList.hpp>
#include <AgogCore/AMethodArg.hpp>
#include <AgogCore/APCompactArray.hpp>


//=======================================================================================
// Local Global Structures
//=======================================================================================

// Enumerated constants
enum
  {
  AFormat_string_char_max =  1024,
  };


//=======================================================================================
// Common/Local Data
//=======================================================================================

// Ensures that this translation unit is initialized before all others except for
// compiler translation units
// ***Note: This may not work with all compilers (Borland for example).
#pragma warning( disable : 4073 )  // Disable warning message for the next line
#pragma init_seg(lib)


namespace
  {

  // Persistent C-string buffer for temporary strings
  char g_cstr_p[AFormat_string_char_max];

  // Default values for AgogCoreVals
  const uint32_t ADatum_pool_size     =   256u;
  const uint32_t ADatum_pool_incr     =    64u;
  const uint32_t AStringRef_pool_size = 40960u;
  const uint32_t AStringRef_pool_incr =   256u;
  const uint32_t ASymbolRef_pool_size =  2048u;
  const uint32_t ASymbolRef_pool_incr =   256u;

  };


//---------------------------------------------------------------------------------------
// Constructor
// See:        Agog::get_agog_core_vals()
// Notes:      This is placed in the .cpp file so changes can be made and only this
//             module will need to recompile.
// Author(s):   Conan Reis
AgogCoreVals::AgogCoreVals() :
  m_using_defaults(true),
  m_pool_init_datum(ADatum_pool_size),
  m_pool_incr_datum(ADatum_pool_incr),
  m_pool_init_string_ref(AStringRef_pool_size),
  m_pool_incr_string_ref(AStringRef_pool_incr),
  m_pool_init_symbol_ref(ASymbolRef_pool_size),
  m_pool_incr_symbol_ref(ASymbolRef_pool_incr)
  {
  }


//=======================================================================================
// AString Class Data Members
//=======================================================================================

bool               AString::ms_char_match_table[ACharMatch__length][AString_ansi_charset_length];
bool *             AString::ms_is_lowercase = AString::ms_char_match_table[ACharMatch_lowercase];
bool *             AString::ms_is_space     = AString::ms_char_match_table[ACharMatch_white_space];
bool *             AString::ms_is_uppercase = AString::ms_char_match_table[ACharMatch_uppercase];
bool *             AString::ms_is_digit     = AString::ms_char_match_table[ACharMatch_digit];
const AString      AString::ms_empty(AStringRef::get_empty());
//A_DSCOPE_LOG(AString_ms_empty, A_SOURCE_STR "AString::ms_empty - ctor\n", A_SOURCE_STR "AString::ms_empty - dtor\n");
const AString      AString::ms_comma(",", 1u);
const AString      AString::ms_dos_break("\r\n", 2u);
AConstructDestruct AString::ms_construct_destruct(init_match_table);  // ctor/dtor must be last data member

// Convert ASCII character to uppercase.
// $Revisit - Note that these are ASCII only! They will need to be revised if library converted to Unicode with UTF-8.
const char AString::ms_char2uppper[256] =
  {
  '\0',   '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\a',
  '\b',   '\t',   '\n',   '\v',   '\f',   '\r',   '\x0e', '\x0f', 
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', 
  '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f', 
  ' ',    '!',    '"',    '#',    '$',    '%',    '&',    '\'',
  '(',    ')',    '*',    '+',    ',',    '-',    '.',    '/',
  '0',    '1',    '2',    '3',    '4',    '5',    '6',    '7',
  '8',    '9',    ':',    ';',    '<',    '=',    '>',    '?',
  '@',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
  'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',
  'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
  'X',    'Y',    'Z',    '[',    '\\',   ']',    '^',    '_',
  '`',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
  'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',
  'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
  'X',    'Y',    'Z',    '{',    '|',    '}',    '~',    '\x7f',
  '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87',
  '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f',
  '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97',
  '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f',
  '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7',
  '\xa8', '\xa9' ,'\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
  '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7', 
  '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf', 
  '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7', 
  '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf', 
  '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7', 
  '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf', 
  '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7', 
  '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef', 
  '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7', 
  '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff' 
  };  

// Convert ASCII character to lowercase.
// $Revisit - Note that these are ASCII only! They will need to be revised if library converted to Unicode with UTF-8.
const char AString::ms_char2lower[256] =
  {
  '\0',   '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\a',
  '\b',   '\t',   '\n',   '\v',   '\f',   '\r',   '\x0e', '\x0f', 
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', 
  '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f', 
  ' ',    '!',    '"',    '#',    '$',    '%',    '&',    '\'',
  '(',    ')',    '*',    '+',    ',',    '-',    '.',    '/',
  '0',    '1',    '2',    '3',    '4',    '5',    '6',    '7',
  '8',    '9',    ':',    ';',    '<',    '=',    '>',    '?',
  '@',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
  'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',
  'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
  'x',    'y',    'z',    '[',    '\\',   ']',    '^',    '_',
  '`',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
  'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',
  'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
  'x',    'y',    'z',    '{',    '|',    '}',    '~',    '\x7f',
  '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87',
  '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f',
  '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97',
  '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f',
  '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7',
  '\xa8', '\xa9' ,'\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
  '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7', 
  '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf', 
  '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7', 
  '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf', 
  '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7', 
  '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf', 
  '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7', 
  '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef', 
  '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7', 
  '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff' 
  };  


//=======================================================================================
// ASymbol Class Data Members
//=======================================================================================

// This is the null symbol representing an empty string ""
#if defined(A_SYMBOL_REF_LINK)
  const ASymbol ASymbol::ms_null(&ASymbolRef::get_null());
#else
  const ASymbol ASymbol::ms_null;
#endif
//A_DSCOPE_LOG(AString_ms_null, A_SOURCE_STR "ASymbol::ms_null - ctor\n", A_SOURCE_STR "ASymbol::ms_null - dtor\n");


//=======================================================================================
// ASymbolTable Class Data Members
//=======================================================================================

#if defined(A_SYMBOLTABLE_CLASSES)
  ASymbolTable * ASymbolTable::ms_main_p = &ASymbolTable::get_main();
#endif


//=======================================================================================
// ARandom Class Data Members
//=======================================================================================

// Common random number generator
ARandom ARandom::ms_gen;


//=======================================================================================
// Executable Watermarking
//=======================================================================================

//#pragma comment( user, "Agog Executable Compiled on " __DATE__ " at " __TIME__ )


//=======================================================================================
// Global Functions
//=======================================================================================

//---------------------------------------------------------------------------------------
// Creates a formatted C-string.  The character buffer is guaranteed to be
//             available between calls of this function.  This method should be used
//             instead of sprintf and related functions.  [This method is handy if the
//             AString class is difficult to access at a particular point in code or if
//             memory might be low.]
// Returns:    formatted C-string character buffer.  It is a reused buffer - copy its
//             contents if not used immediately and do not delete.
// Arg         format_str_p - follows the same format as the C printf(), sprintf(), etc.
//             See the MSDev online help for 'Format Specification Fields' for a
//             description. 
// Arg         ... - variable length arguments expected by the formatted string.
//
//             #### IMPORTANT #### Since this method currently uses the standard C string
//             formatting functions, passing a AString object as an argument will try to
//             use the address of the object with disastrous results.  Use the %s
//             specifier and 'str.as_cstr()' as an argument for AString objects.
//
//             In the future, a %$ type could be added to use AString objects natively.
// See:        AString, ADebug::print(), ADebug::print_format(), ADebug::print_args(),
//             ADebug::register_print_func(), ADebug::unregister_print_func()
// Modifiers:   static
// Author(s):   Conan Reis
char * a_cstr_format(const char * format_cstr_p, ...)
  {
  va_list args;  // initialize argument list

  va_start(args, format_cstr_p);

  #if defined (A_PLAT_PS3) || defined(A_PLAT_PS4)
    int length = vsnprintf(g_cstr_p, size_t(AFormat_string_char_max), format_cstr_p, args);
  #else
    int length = _vsnprintf(g_cstr_p, size_t(AFormat_string_char_max - 1), format_cstr_p, args);
  #endif

  va_end(args);  // end argument list processing

  if ((length == -1) || (length == (AFormat_string_char_max - 1)))
    {
    // More characters than buffer has, so truncate with null-terminator
    g_cstr_p[AFormat_string_char_max - 1] = '\0';
    }

  return g_cstr_p;
  }

//---------------------------------------------------------------------------------------
// Creates a formatted AString reusing a common buffer.  The character buffer
//             is guaranteed to be available between calls of this function.  This method
//             should be used instead of sprintf and related functions.  [This method can
//             be handier than using AString(max_size, format_str_p, ...) since it already
//             has a buffer allocated.]
// Returns:    formatted AString.  It is a reused buffer - copy its contents if not used
//             immediately.  It is properly tagged so that other AStrings will know not
//             to modify it or delete it without making a copy first.
// Arg         format_str_p - follows the same format as the C printf(), sprintf(), etc.
//             See the MSDev online help for 'Format Specification Fields' for a
//             description. 
// Arg         ... - variable length arguments expected by the formatted string.
//
//             #### IMPORTANT #### Since this method currently uses the standard C string
//             formatting functions, passing a AString object as an argument will try to
//             use the address of the object with disastrous results.  Use the %s
//             specifier and 'str.as_cstr()' as an argument for AString objects.
//
//             In the future, a %$ type could be added to use AString objects natively.
// See:        AString, ADebug::print(), ADebug::print_format(), ADebug::print_args(),
//             ADebug::register_print_func(), ADebug::unregister_print_func()
// Modifiers:   static
// Author(s):   Conan Reis
AString a_str_format(const char * format_cstr_p, ...)
  {
  va_list args;  // initialize argument list

  va_start(args, format_cstr_p);

  #if defined (A_PLAT_PS3) || defined(A_PLAT_PS4)
    int length = vsnprintf(g_cstr_p, size_t(AFormat_string_char_max), format_cstr_p, args);
  #else
    int length = _vsnprintf(g_cstr_p, size_t(AFormat_string_char_max - 1), format_cstr_p, args);
  #endif

  va_end(args);  // end argument list processing

  if (length == -1)
    {
    // More characters than buffer has, so truncate with null-terminator
    length = AFormat_string_char_max - 1;
    g_cstr_p[length] = '\0';
    }

  return AString(g_cstr_p, AFormat_string_char_max, length, false);
  }
