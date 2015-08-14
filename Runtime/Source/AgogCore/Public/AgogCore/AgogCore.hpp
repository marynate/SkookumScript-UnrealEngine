//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2000 Agog Labs Inc.,
// All rights reserved.
//
//  Common stuff for AgogCore
// Author(s):    Conan Reis
// Notes:          Simple types, macros, etc. for ease of use, readability, and
//              type checking.
//              *** ALL code should use these types, etc. wherever appropriate ***
//              IMPORTANT:  Aspects of this file may be platform specific
//=======================================================================================

#ifndef __AGOGCORE_HPP
#define __AGOGCORE_HPP
#pragma once


//=======================================================================================
// Includes  (Additional include bottom of file)
//=======================================================================================

// If AGOG_EXT_INCLUDE is defined then include AgogExtHook.hpp for custom defines such
// as platform types, asserts, user breaks etc.
//#if defined(AGOG_EXT_INCLUDE) && !defined(AGOG_EXT_IGNORE)
  #include <AgogCore/_AgogExtHook.hpp>
//#endif

#include <stdint.h>  // For sized integer types, minimums and maximums: int##_t, uint##_t, INT##_MAX, UINT##_MAX
#include <limits.h>  // integer minimums and maximums: INT_MAX, UINT_MAX
#include <float.h>   // float minimums and maximums: FLT_DIG, FLT_MAX, FLT_EPSILON, DBL_DIG
#include <stddef.h>  // size_t on OS X
#include <new>       // new & delete

// Additional includes at end of file


//=======================================================================================
// Global Macros / Defines
//=======================================================================================

#define A_COPYRIGHT_TEXT  "Copyright (c) 2001-2015 Agog Labs Inc."

//---------------------------------------------------------------------------------------
// Platform Defines
#if !defined(A_PLAT_PC32) && !defined(A_PLAT_PC64) \
  && !defined(A_PLAT_X360) && !defined(A_PLAT_X_ONE) \
  && !defined(A_PLAT_PS3) && !defined(A_PLAT_PS4) \
  && !defined(A_PLAT_WIIU) \
  && !defined(A_PLAT_iOS) && !defined(A_PLAT_OSX) \
  && !defined(A_PLAT_LINUX64)
  #ifdef _WIN32
    // Assume it is a Windows PC platform (either 32-bit or 64-bit)
    #ifdef _WIN64
      // Assume 64-bit PC platform
      #define A_PLAT_PC64
    #else
      // Assume 32-bit PC platform
      #define A_PLAT_PC32
    #endif
  #else
    #error No known platform type is defined (A_PLAT_PC32, A_PLAT_PC64, A_PLAT_PS3, A_PLAT_PS4, A_PLAT_X360, A_PLAT_X_ONE, A_PLAT_WIIU, A_PLAT_iOS or A_PLAT_LINUX64)!
  #endif
#else
#endif


//---------------------------------------------------------------------------------------
// Microsoft PC 64-bit
#ifdef A_PLAT_PC64

  #define A_PLAT_STR_ID   "PC64"
  #define A_PLAT_STR_DESC "Windows PC 64-bit"
  #define A_PLAT_PC
  #define A_BITS64

#endif


//---------------------------------------------------------------------------------------
// Microsoft PC 32-bit
#ifdef A_PLAT_PC32

  #define A_PLAT_STR_ID   "PC32"
  #define A_PLAT_STR_DESC "Windows PC 32-bit"
  #define A_PLAT_PC
    
#endif


//---------------------------------------------------------------------------------------
// PC (Microsoft Windows) Platform - common to both A_PLAT_PC32 and A_PLAT_PC64
#ifdef A_PLAT_PC

  #define AGOG_LITTLE_ENDIAN_HOST   1    // PC is little endian

  // Microsoft Developer Studio specific compiler pragmas
  #if defined(_MSC_VER)
  
    // Level 4 warnings that are acceptable
    #pragma warning( disable : 4097 ) // synonym used
    #pragma warning( disable : 4100 ) // unreferenced formal parameter
    #pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union (for MMSystem.h)
    #pragma warning( disable : 4238 ) // nonstandard extension used, class rvalue as lvalue
    #pragma warning( disable : 4348 ) // potential redefinition of template default parameter
    #pragma warning( disable : 4355 ) // 'this' : used in base member initializer list
    #pragma warning( disable : 4786 ) // Disable warning message for long identifiers
    #pragma warning( disable : 4800 ) // Forcing value to bool 'true' or 'false' (performance warning)
    #pragma warning( disable : 4996 ) // Used depreciated function - used since declaring _CRT_SECURE_NO_WARNINGS seems to have no effect
  
    #ifdef __INTEL_COMPILER
      #pragma warning( disable : 981 )   // operands are evaluated in unspecified order
      #pragma warning( disable : 383 )   // value copied to temporary, reference to temporary used
    #else
      #pragma warning( disable : 4616 )  // pragma warning :  warning number '1011' out of range, must be between '4001' and '4999'
    #endif
  
    // Generate intrinsics (inline assembly) for the following functions.
    // $Revisit - CReis There are more intrinsics to add, but they need to be combined with appropriate
    // compiler options - look into this further.
    //#pragma intrinsic(_strset, abs, fabs, memcmp, memcpy, memset, strcat, strcmp, strcpy, strlen)
  
    //#pragma optimize()
  
  
    #if (_M_IX86 < 600)
  
      #define A_NO_SSE
  
    #endif
  
  #endif  // _MSC_VER

#endif  // A_PLAT_PC


//---------------------------------------------------------------------------------------
// Sony PlayStation 4 (Orbis) Platform
#ifdef A_PLAT_PS4

  #define A_PLAT_STR_ID   "PS4"
  #define A_PLAT_STR_DESC "Sony PlayStation 4"
  #define A_BITS64
  #define AGOG_LITTLE_ENDIAN_HOST   1    // Little endian
  #define __FUNCSIG__ __func__
 
  // Use old POSIX call convention rather than new ISO convention
  #define _vsnprintf   vsnprintf
  #define _snprintf    snprintf

  // Indicate that _itoa(), _ultoa(), and _gcvt() are not defined
  #define A_NO_NUM2STR_FUNCS

#endif


//---------------------------------------------------------------------------------------
// Sony PlayStation 3 Platform
#ifdef A_PLAT_PS3

  #define A_PLAT_STR_ID   "PS3"
  #define A_PLAT_STR_DESC "Sony PlayStation 3"
  #define A_NO_SSE
  #define AGOG_ALIGNMENT32
  #define AGOG_LITTLE_ENDIAN_HOST   0    // Big endian

  // $Revisit - CReis Look into __func__ and other macros instead for GCC compiler.
  #define __FUNCSIG__  __PRETTY_FUNCTION__

  // Use old POSIX call convention rather than new ISO convention
  #define _vsnprintf   vsnprintf
  #define _snprintf    snprintf

  // Indicate that _itoa(), _ultoa(), and _gcvt() are not defined
  #define A_NO_NUM2STR_FUNCS

#endif


//---------------------------------------------------------------------------------------
// Microsoft Xbox One (Durango) Platform
#ifdef A_PLAT_X_ONE

  #define A_PLAT_STR_ID   "X_ONE"
  #define A_PLAT_STR_DESC "Microsoft Xbox One"
  #define A_BITS64
  #define AGOG_LITTLE_ENDIAN_HOST   1    // Little endian

#endif


//---------------------------------------------------------------------------------------
// Microsoft XBox 360 Platform
#ifdef A_PLAT_X360

  #define A_PLAT_STR_ID   "X360"
  #define A_PLAT_STR_DESC "Microsoft Xbox 360"
  #define A_NO_SSE
  #define AGOG_ALIGNMENT32
  #define AGOG_LITTLE_ENDIAN_HOST   0    // Big endian

#endif


//---------------------------------------------------------------------------------------
// Apple iOS (iPhone/iPod/iPad) Platform
#ifdef A_PLAT_iOS

  #define A_PLAT_STR_ID   "iOS"
  #define A_PLAT_STR_DESC "Apple iOS"
  #define A_NO_SSE
  #define AGOG_ALIGNMENT32
  #define AGOG_LITTLE_ENDIAN_HOST   1    // Little endian

  // $Revisit - CReis Look into using __func__ and __PRETTY_FUNCTION__ instead
  #define __FUNCSIG__  __FUNCTION__

  #define A_BREAK()   __builtin_trap()

  // Use old POSIX call convention rather than new ISO convention
  #define _gcvt       gcvt
  #define _stricmp    strcasecmp
  #define _strnicmp   strncasecmp
  #define _snprintf   snprintf
  #define _vsnprintf  vsnprintf

  // Indicate that _itoa(), _ultoa(), and _gcvt() are not defined
  #define A_NO_NUM2STR_FUNCS

  #pragma clang diagnostic ignored "-Wundefined-bool-conversion"

#endif


//---------------------------------------------------------------------------------------
// Apple OS X Platform
#ifdef A_PLAT_OSX

  #define A_PLAT_STR_ID   "OSX"
  #define A_PLAT_STR_DESC "Apple OS X"
  #define A_BITS64
  #define A_NO_SSE // $Revisit MBreyer - off for now since intrinsics are different with Clang
  #define AGOG_LITTLE_ENDIAN_HOST   1    // Little endian

  #define __FUNCSIG__ __PRETTY_FUNCTION__

  #define A_BREAK()   asm("int $3")

  // Use old POSIX call convention rather than new ISO convention
  #define _gcvt       gcvt
  #define _stricmp    strcasecmp
  #define _strnicmp   strncasecmp
  #define _snprintf   snprintf
  #define _vsnprintf  vsnprintf

  // Indicate that _itoa(), _ultoa(), and _gcvt() are not defined
  #define A_NO_NUM2STR_FUNCS

#endif


//---------------------------------------------------------------------------------------
// Linux Platform 64-bit
#ifdef A_PLAT_LINUX64

  #define A_PLAT_STR_ID   "Linux64"
  #define A_PLAT_STR_DESC "Linux 64-bit"
  #define A_BITS64
  #define A_NO_SSE // $Revisit MBreyer - off for now since intrinsics are different with GCC
  #define AGOG_LITTLE_ENDIAN_HOST   1    // Little endian

  #define __FUNCSIG__ __PRETTY_FUNCTION__

  #define A_BREAK()   asm("int $3")
  
  // Use old POSIX call convention rather than new ISO convention
  #define _gcvt       gcvt
  #define _stricmp    strcasecmp
  #define _strnicmp   strncasecmp
  #define _snprintf   snprintf
  #define _vsnprintf  vsnprintf

  // Indicate that _itoa(), _ultoa(), and _gcvt() are not defined
  #define A_NO_NUM2STR_FUNCS

#endif


//---------------------------------------------------------------------------------------
// Nintendo WiiU Platform
#ifdef A_PLAT_WIIU

  #define A_PLAT_STR_ID   "WiiU"
  #define A_PLAT_STR_DESC "Nintendo WiiU"
  #define AGOG_LITTLE_ENDIAN_HOST   1    // Little endian

#endif


//---------------------------------------------------------------------------------------
// Generic 32/64-bit defines
#ifdef A_BITS64

  #define A_BITS_ID    "64"
  #define A_BITS_STR   "64-bit"

#else

  // Assume 32-bits if 64-bits not defined.
  #define A_BITS32
  #define A_BITS_ID    "32"
  #define A_BITS_STR   "32-bit"

#endif

//#pragma message("Using platform - " A_PLAT_STR_DESC)


//---------------------------------------------------------------------------------------
// Inlining related defines
//
// If 'A_INL_IN_CPP' is defined, let the compiler treat inlined code normally and place
// *.inl files in *.cpp files in a non-inlined form.  This makes it so that changes to
// *.inl files will only need to recompile their associated *.cpp files and it makes
// recompiles faster in general.  This creates slower, but more easily debuggable code
// with faster compile turnaround times.
//
// If 'A_INL_IN_CPP' is not defined, enable full inlining and place *.inl files in *.hpp
// files in an inlined form.  Changes to *.inl files will need to recompile any files
// that include the *.hpp files associated with the *.inl files.  This makes for the
// fastest running code, but it is harder to debug and has the longest compile times.
//
// Any functions or methods that are in *.inl files must be preceded with the 'A_INLINE'
// define which is used like the 'inline' keyword.
//
//   Default:  By default if '_DEBUG' is defined, 'A_INL_IN_CPP' is defined.
//   non-inlined:  When '_DEBUG' is not defined and non-inlining is desired, just define
//     'A_INL_IN_CPP' as a compiler preprocessor directive.
//   inlined:  If 'A_INL_IN_HPP' is defined as a compiler preprocessor
//     directive, 'A_INL_IN_CPP' will not be defined even if '_DEBUG' is defined.
#if defined(A_UNOPTIMIZED) && !defined(A_INL_IN_HPP) && !defined(A_INL_IN_CPP)
  #define A_INL_IN_CPP
#endif

#ifdef A_INL_IN_CPP

  // Don't use inlines - Include *.inl files into *.cpp files

  #define A_INLINE

  // MS Dev specific
  #if defined(_MSC_VER)
    #pragma warning( disable : 4710 )  // Function not inlined
  #endif

#else

  // Use inlines - Include *.inl files into *.hpp files

  #define A_INL_IN_HPP  // Note that test for inclusion in hpp should be absence of A_INL_IN_CPP
  #define A_INLINE      inline

  // MS Dev specific
  #if defined(_MSC_VER)
    #pragma inline_depth(255)          // No limits on inline depth.

    // Prevents the compiler from second guessing the inlining of functions marked as inline
    // $Revisit - CReis The C++ Standard Library forbids macroizing keywords.
    //#define inline __forceinline
  #endif

#endif

#ifdef _MSC_VER
  #pragma warning( disable : 4514 ) // unreferenced inline function has been removed
#endif


//---------------------------------------------------------------------------------------
// Macro Functions

// Swaps two 32-bit values without the need of a third temporary variable
#define A_SWAP32(_value1, _value2) {*(uint32_t *)&_value1 ^= *(uint32_t *)&_value2; *(uint32_t *)&_value2 ^= *(uint32_t *)&_value1; *(uint32_t *)&_value1 ^= *(uint32_t *)&_value2;}

// Converts sized signed integer to AEquate_less(-1), AEquate_equal(0), AEquate_greater(1)
#define A_INT32_AS_EQUATE(_num)  static_cast<eAEquate>((_num > 0) ? 1 : _num >> 31)
#define A_INT64_AS_EQUATE(_num)  static_cast<eAEquate>((_num > 0) ? 1 : _num >> 63)
#ifdef A_BITS64
  // Converts system sized integer to AEquate_less(-1), AEquate_equal(0), AEquate_greater(1)
  #define A_INT_AS_EQUATE(_num)  A_INT64_AS_EQUATE(_num)
  // Ensures difference works as 32-bit value
  #define A_INT_AS_DIFF32(_num)  ((_num > 0) ? 1 : _num >> 63)
#else
  #define A_INT_AS_EQUATE(_num)  A_INT32_AS_EQUATE(_num)
  #define A_INT_AS_DIFF32(_num)  _num
#endif


// Determine number of elements in an array
#define A_COUNT_OF(_array) (sizeof(_array) / sizeof(_array[0]))

// Some worker defines unlikely to be used elsewhere
#define A__INT_LITERAL_TO_STR(_val) # _val
#define A_INT_LITERAL_TO_STR(_val)  A__INT_LITERAL_TO_STR(_val)

// To be used when __LINE__ is needed as a string rather than an integer
#define A_LINE_STR A_INT_LITERAL_TO_STR(__LINE__)

// Use this when writing to the IDE output window to make a line that will load up the
// appropriate file and line when it is double clicked on.
#define A_SOURCE_STR __FILE__ "(" A_LINE_STR ") : "

#define A_SOURCE_FUNC_STR  A_SOURCE_STR __FUNCSIG__

// Use #pragma A_LOG("IDE Output to double click") to write to the IDE output window
// during compilation with a line that includes _msg and will load up the appropriate file
// and line when it is double clicked on.
// $Revisit - CReis Look into using C99/C++0X _Pragma() and MSVC _pragma() to shorten this
#define A_LOG(_msg)  message(A_SOURCE_STR _msg " - " __FUNCTION__ "()")

// Work-in-progress note - just like A_LOG() above though only shown if A_SHOW_NOTES is
// defined and ignored otherwise.  For example:
// #pragma A_NOTE("***Incomplete***")
#ifdef A_SHOW_NOTES
  #define A_NOTE(_msg)  A_LOG(_msg)
#else
  #define A_NOTE(_msg)
#endif

// This acts as an intention comment for a non-terminating (i.e. infinite) loop.
#define A_LOOP_INFINITE for (;;)



//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declarations
class AString;

//---------------------------------------------------------------------------------------
// Number Type Shorthand
//   - also see stdint.h uint8_t, uint32_t, int32_t, etc.
// Note that pointer storage should use uintptr_t, intptr_t, ptrdiff_t - which are
// included via stdint.h (and other headers).
// 
// $Note - May need to be in their own namespace to avoid collisions with other libraries.

#ifndef A_UINT
  // Standard/unsized unsigned integer - has no t/_t to match int
  typedef unsigned int uint;

  #define A_UINT
#endif


// Floating point shorthand.
typedef float   f32;  // 3.4E +/-   38  (7 digits)
typedef double  f64;  // 1.7E +/-  308 (15 digits)


//=======================================================================================
// Constants
//=======================================================================================

//---------------------------------------------------------------------------------------
// Common default/special operation/self-documenting integer constants
// - Defaults used when 0, 1, -1, 42, etc. will not do.

const int      ADef_int     = INT32_MAX;  // INT_MAX;
const uint     ADef_uint    = UINT32_MAX; // UINT_MAX;
const int8_t   ADef_int8    = INT8_MAX;
const uint8_t  ADef_uint8   = UINT8_MAX;
const int16_t  ADef_int16   = INT16_MAX;
const uint16_t ADef_uint16  = UINT16_MAX;
const int32_t  ADef_int32   = INT32_MAX;
const uint32_t ADef_uint32  = UINT32_MAX;
const int64_t  ADef_int64   = INT64_MAX;
const uint64_t ADef_uint64  = UINT64_MAX;

// Substitute with the length of the object or the length remaining after any supplied
// starting position.
const uint ALength_remainder = ADef_uint;

// Substitute with the length of the supplied arguments - for example with a supplied
// C-String determine its length by finding a null character.
const uint ALength_calculate = ADef_uint;

// The length of the buffer is included in the first few byte(s) of the buffer.
const uint ALength_in_header = ADef_uint;

// If searching some sort of collection where there is the possibility of multiple
// matching elements, this indicates that the last matching element is desired
// - i.e. the matching element that is closest to the end of the collection.
const uint AMatch_last = ADef_uint;

// If searching some sort of collection where there is the possibility of multiple
// matching elements, this indicates that the matching element that is first *found* is
// desired.  This could be any matching element in the collection it effectively means
// ignore element order.  This type of a search can be faster than specifying that
// matching element order is important.
const uint AMatch_first_found = 0u;

const uint32_t ADef_no_change = ADef_uint32;  // Indicates that no change should be made


//---------------------------------------------------------------------------------------
// Common default/special operation float constants
// - Defaults used when 0.0, 1.0, -1.0, 42.0, etc. will not do.

// $Revisit - CReis Could add float defaults
// const f32 ADef_f32 = nan;
// const f64 ADef_f64 = nan;


//---------------------------------------------------------------------------------------
// Misc. Common Constants

enum eAFlag
  {
  AFlag_off       = 0x0,
  AFlag_on        = 1 << 0,
  AFlag__toggle   = 1 << 1
  };

// Used for a Boolean flag/value over a collection of elements where they could all have
// it set, they could all have it not set or they could be mixed with some set and some
// not.
enum eAGroupFlag
  {
  AGroupFlag_uninitialized = 0,
  AGroupFlag_present       = 1 << 0,
  AGroupFlag_absent        = 1 << 1,
  AGroupFlag_mixed         = AGroupFlag_present | AGroupFlag_absent  // Some in group with flag set and flag cleared
  };

enum eAConfirm
  {
  AConfirm_abort = -1,
  AConfirm_no    =  0,
  AConfirm_yes   =  1
  };

enum eAProgess
  {
  AProgess_skip,
  AProgess_queued,
  AProgess_processing,
  AProgess_processed
  };

// Comparison result value - also see a_compare() below and A_INT*_AS_EQUATE() macros above.
enum eAEquate
  {
  AEquate_less    = -1,
  AEquate_equal   = 0,
  AEquate_greater = 1
  };

// Used to specify lifetime of an argument or task.
// Commonly used with memory buffers like strings and serialization.
enum eATerm
  {
  // transient/temporary/volatile - will not last through task at hand must use
  // immediately or make copy
  ATerm_short = 0,

  // persistent/reserved/non-volatile - can use for lifetime of task at hand
  ATerm_long  = 1
  };

// Used to specify whether something should be remembered/archived in some sort of a
// history/memory or not.  [Used in the place of bool to give more obvious context.]
enum eAHistory
  {
  AHistory_forget   = 0,   // Doomed to repeat it
  AHistory_remember = 1
  };

// Indicates whether an iteration or operation did full span or action or quit early.
// [Often used in the place of bool to give more obvious context.]
enum eAIterateResult
  {
  AIterateResult_entire     = 0,  // [implicitly coercible to false]
  AIterateResult_early_exit = 1   // [implicitly coercible to true]
  };

enum eAHorizAlign
  {
  AHorizAlign_left      = 0,
  AHorizAlign_right     = 1,
  AHorizAlign_centered  = 2,
  AHorizAlign_justified = 3
  };

enum eAVertAlign
  {
  AVertAlign_top      = 0,
  AVertAlign_bottom   = 1,
  AVertAlign_centered = 2
  };

// Flags for iterating through a hierarchy
enum eAHierarchy
  {
  // Stop iterating through a hierarchy
  AHierarchy__stop    = 0,
  // Apply operation to current object in hierarchy
  AHierarchy_current  = 1 << 0,
  // Apply operation recursively sub-objects in hierarchy
  AHierarchy_recurse  = 1 << 1,
  // Iterate over current object and recurse through sub-objects
  AHierarchy__all     = AHierarchy_current | AHierarchy_recurse
  };

// Used to control information / logging / tracing output.
// If some levels are skipped for a particular function try to use relative operators <>
// to ensure that all verbosity levels may be used.
// Also note that Verbosity_none = 0 may be used as a "false" and that greater amounts of
// verbosity are ordinally greater in value so AVerbosity_full > AVerbosity_critical.
enum eAVerbosity
  {
  AVerbosity_none = 0,  // Display nothing
  AVerbosity_critical,  // Only display info if something important occurs
  AVerbosity_brief,     // Display major/high-level information
  AVerbosity_full       // Display both major/minor high/low-level info in all its glory
  };


//---------------------------------------------------------------------------------------
// Initial/starting values for AgogCore classes.
struct AgogCoreVals
  {
  // Methods

    AgogCoreVals();

  // Public Data Members

    // 'true' when initially constructed.  If an app needs to override the default values,
    // check for 'true', set any desired custom values, and then set to 'false'.
    bool m_using_defaults;

    // Object pool size for ADatum objects.  Initial size and amount to increment when
    // more objects are needed than are available.
    uint m_pool_init_datum;
    uint m_pool_incr_datum;

    // Object pool size for AStringRef objects.  Initial size and amount to increment when
    // more objects are needed than are available.
    uint m_pool_init_string_ref;
    uint m_pool_incr_string_ref;

    // Object pool size for AStringRef objects.  Initial size and amount to increment when
    // more objects are needed than are available.
    uint m_pool_init_symbol_ref;
    uint m_pool_incr_symbol_ref;

  };


namespace Agog
  {

  // This must be defined somewhere by the application - See _AgogCoreDefaults.cpp
  AgogCoreVals & get_agog_core_vals();

  }


//=======================================================================================
// Global Functions
//=======================================================================================

//---------------------------------------------------------------------------------------
// Compares two types and returns the result as an eAEquate
// Returns:    AEquate_less, AEquate_equal or AEquate_greater
// Author(s):   Conan Reis
template<class _Type>
inline eAEquate a_compare(_Type lhs, _Type rhs)
  {
  return (lhs < rhs) ? AEquate_less : eAEquate(1 - static_cast<int>(lhs == rhs));
  }

//---------------------------------------------------------------------------------------
// Common C-string format functions - use instead of sprintf() and related functions or
// even better use the AString class if possible.  See the comments in the cpp file for
// more info.

char *  a_cstr_format(const char * format_cstr_p, ...);
AString a_str_format(const char * format_cstr_p, ...);


//---------------------------------------------------------------------------------------
// Memory allocation declarations 
// alloca() will always return 16-byte aligned memory

#define a_align_up(x,align)        (((x)+((align)-1)) & (-((int)(align))))
#define a_stack_allocate(count,T) (T*)alloca((count)*sizeof(T))

// $Revisit - CReis These should probably be macros to allow for easier redefinition/memory tracking

#ifdef A_PLAT_X360
  //_Ret_bytecap_(_Size) void * operator new[](size_t size);
#else
  //void * operator new[](size_t size);
#endif
//void  operator delete(void * ptr);
//void  operator delete[](void * ptr);

void * operator new(size_t size, const char * desc_cstr_p);
void * operator new[](size_t size, const char * desc_cstr_p);

// $Note - *** delete operators with additional arguments cannot be called explicitly
// - they are only called on class destruction on a paired new with similar arguments.
// This means that a simple delete call will often call the single argument operator
// delete(void * mem_p) so take care to ensure that the proper delete is paired with a
// corresponding new.

void  operator delete(void * ptr, const char * desc_cstr_p);
void  operator delete[](void * ptr, const char * desc_cstr_p);


//---------------------------------------------------------------------------------------
// Memory allocation definitions
// Standard inlined memory allocation functions used if A_MEMORY_FUNCS_PRESENT not present
// and custom definitions need to be present if A_MEMORY_FUNCS_PRESENT is present.
#if !defined(A_MEMORY_FUNCS_PRESENT)
  #define A_MEMORY_FUNCS_PRESENT

  #if defined(A_PLAT_iOS)
    #include <malloc/malloc.h>
  #elif defined(A_PLAT_OSX)
    #include <stdlib.h>
  #else
    #include <malloc.h>
  #endif

  //---------------------------------------------------------------------------------------
  inline void * operator new(size_t size, const char * desc_cstr_p)
    {
    return ::malloc(size);
    }
    
  //---------------------------------------------------------------------------------------
  inline void * operator new[](size_t size, const char * desc_cstr_p)
    {
    return ::malloc(size);
    }

  //---------------------------------------------------------------------------------------
  inline void operator delete(void * buffer_p, const char * desc_cstr_p)
    {
    ::free(buffer_p);
    }

  //---------------------------------------------------------------------------------------
  inline void operator delete[](void * buffer_p, const char * desc_cstr_p)
    {
    ::free(buffer_p);
    }


#endif  // A_MEMORY_FUNCS_PRESENT

// Define placement new
#if !defined(__PLACEMENT_NEW_INLINE) && !defined(NO_AGOG_PLACEMENT_NEW)
#define __PLACEMENT_NEW_INLINE

//---------------------------------------------------------------------------------------
// Allows the creation of objects in place using supplied memory
// Arg         size_t size - [ignored] 'memory_p' is assumed to be large enough
// Arg         void * buffer_p - memory to create instance in
// Examples:   AString str_p = new (memory_p) AString("In place AString");
// See:        placement delete below
// Notes:      This function should be completely optimized out by the compiler and
//             simply become 'memory_p'.
// Author(s):   Conan Reis
inline void * operator new(size_t, void * buffer_p)
  {
  return buffer_p;
  }

//---------------------------------------------------------------------------------------
// This is the placement delete to mirror the placement new.  It serves no
//             purpose other than to prevent a compiler warning since all operator new
//             functions expect a matching operator delete function.
// Arg         void * obj_p - [ignored] pointer to object
// Arg         void * memory_p - [ignored] memory instance was created in
// Examples:   str_p->~AString();
// See:        placement new above
// Notes:      This function should be completely optimized out by the compiler.
// Author(s):   Conan Reis
inline void operator delete(void *, void *)
  {
  }

#endif  // __PLACEMENT_NEW_INLINE

//=======================================================================================
// Includes (Special Order)
//=======================================================================================

#include <AgogCore/ADebug.hpp>  // For all debugging related stuff.


#endif  // __AGOGCORE_HPP


