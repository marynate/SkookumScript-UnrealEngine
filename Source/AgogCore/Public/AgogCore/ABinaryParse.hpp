//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2000 Agog Labs Inc.,
// All rights reserved.
//
//  Binary data parsing header
// Author(s):    Conan Reis
// Create Date:   2001-11-28
// Notes:          It currently converts from host platform endian form to a little-endian
//              stream.  So the binary stream (which generally is written to or read from
//              a binary file) is the same for all platforms - i.e. little endian.
//              [*This could be switched to use big-endian streams instead.  Little endian
//              was originally chosen as the stream type since most authoring is done on
//              PCs.]
//
//              The defines AGOG_ALIGNMENT32 (defined or not defined) and
//              AGOG_LITTLE_ENDIAN_HOST (1 or 0) are expected to be defined elsewhere if
//              32-bit alignment or little endian / big endian data host data is desired.
//
//              $Revisit - CReis These macros and functions would be good candidates to be
//              written in platform specific assembly.
//
//              IMPORTANT:  Aspects of this file are platform specific
//=======================================================================================

#ifndef __ABINARYPARSE_HPP
#define __ABINARYPARSE_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AgogCore.hpp>      // For all the minimums and maximums defined there.


//=======================================================================================
// Alignment functions / macros
//=======================================================================================

#define A_ENDIAN_LITTLE         0
#define A_ENDIAN_BIG            1
#define A_BYTE_STREAM_ENDIAN    A_ENDIAN_LITTLE	// Target endian type used by byte stream

#if (AGOG_LITTLE_ENDIAN_HOST == 1)
  #if (A_BYTE_STREAM_ENDIAN == A_ENDIAN_LITTLE)
    #define A_BYTE_STREAM_NEEDS_SWAP 0  // No byte swap required
  #else   // stream is big endian
    #define A_BYTE_STREAM_NEEDS_SWAP 1  // Byte swap required
  #endif
#else     // host is big endian
  #if (A_BYTE_STREAM_ENDIAN == A_ENDIAN_LITTLE)
    #define A_BYTE_STREAM_NEEDS_SWAP 1  // Byte swap required
  #else   // stream is big endian
    #define A_BYTE_STREAM_NEEDS_SWAP 0  // No byte swap required
  #endif
#endif


//---------------------------------------------------------------------------------------
// Returns a 32-bit unsigned integer in host endian form from 'source_pp'
//             which is a pointer to a potentially non-aligned address to a little endian
//             form unsigned integer - then *source_pp is incremented by 4 bytes.
// Returns:    A properly aligned host endian form unsigned integer.
// Arg         _source_pp - pointer to a pointer of a potentially non-aligned address to
//             read a 32-bit integer in little endian form.  *_source_pp is incremented
//             by 4 bytes.
// See:        A_BYTE_STREAM32(), A_BYTE_STREAM_IN32(), A_BYTE_STREAM_UI16_INC(),
//             A_BYTE_STREAM16(), A_BYTE_STREAM_IN16()
// Notes:      This is useful when alignment may be important and the endian type is from
//             little-endian to host-endian
// Author(s):   Conan Reis
//A_BYTE_STREAM_UI32_INC(_source_pp)
// - implemented lower in file

//---------------------------------------------------------------------------------------
// Assigns a 32-bit integer (signed or unsigned) in little endian form from
//             one potentially non-aligned address to a another host endian form
//             potentially non-aligned address.
// Arg         _dest_p - pointer to potentially non-aligned address to store a 32-bit
//             integer in host endian form
// Arg         _source_p - pointer to potentially non-aligned address to read a 32-bit
//             integer in little endian form
// See:        A_BYTE_STREAM_UI32_INC(), A_BYTE_STREAM_IN32(), A_BYTE_STREAM_UI16_INC(),
//             A_BYTE_STREAM16(), A_BYTE_STREAM_IN16()
// Notes:      This is useful when alignment may be important and the endian type is from
//             little-endian to host-endian
// Author(s):   Conan Reis
//A_BYTE_STREAM32(_dest_p, _source_p)
// - implemented lower in file

//---------------------------------------------------------------------------------------
// Assigns a 32-bit integer (signed or unsigned) in little endian form from
//             one potentially non-aligned address to a another host endian form
//             potentially non-aligned address.
// Arg         _dest_p - pointer to potentially non-aligned address to store a 32-bit
//             integer in little host form
// Arg         _source_pp - pointer to a pointer of a potentially non-aligned address to
//             read a 32-bit integer in little endian form.  *_source_pp is incremented
//             by 4 bytes.
// See:        A_BYTE_STREAM_UI32_INC(), A_BYTE_STREAM32(), A_BYTE_STREAM_UI16_INC(),
//             A_BYTE_STREAM16(), A_BYTE_STREAM_IN16()
// Notes:      This is useful when alignment may be important and the endian type is from
//             little-endian to host-endian
// Author(s):   Conan Reis
//A_BYTE_STREAM_IN32(_dest_p, _source_pp)
// - implemented lower in file

//---------------------------------------------------------------------------------------
// Returns a 16-bit unsigned integer in host endian form from 'source_pp'
//             which is a pointer to a potentially non-aligned address to a little endian
//             form unsigned integer - then *source_pp is incremented by 2 bytes.
// Returns:    A properly aligned host endian form unsigned integer.
// Arg         _source_pp - pointer to a pointer of a potentially non-aligned address to
//             read a 16-bit integer in little endian form.  *_source_pp is incremented
//             by 2 bytes.
// See:        A_BYTE_STREAM32(), A_BYTE_STREAM_IN32(), A_BYTE_STREAM_UI16_INC(),
//             A_BYTE_STREAM16(), A_BYTE_STREAM_IN16()
// Notes:      This is useful when alignment may be important and the endian type is from
//             little-endian to host-endian
// Author(s):   Conan Reis
//A_BYTE_STREAM_UI16_INC(_source_pp)
// - implemented lower in file

//---------------------------------------------------------------------------------------
// Assigns a 16-bit integer (signed or unsigned) in little endian form from
//             one potentially non-aligned address to a another host endian form
//             potentially non-aligned address.
// Arg         _dest_p - pointer to potentially non-aligned address to store a 16-bit
//             integer in host endian form
// Arg         _source_p - pointer to potentially non-aligned address to read a 16-bit
//             integer in little endian form
// See:        A_BYTE_STREAM_UI32_INC(), A_BYTE_STREAM_IN32(), A_BYTE_STREAM_UI16_INC(),
//             A_BYTE_STREAM16(), A_BYTE_STREAM_IN16()
// Notes:      This is useful when alignment may be important and the endian type is from
//             little-endian to host-endian
// Author(s):   Conan Reis
//A_BYTE_STREAM16(_dest_p, _source_p)
// - implemented lower in file

//---------------------------------------------------------------------------------------
// Assigns a 16-bit integer (signed or unsigned) in little endian form from
//             one potentially non-aligned address to a another host endian form
//             potentially non-aligned address - then *source_pp is incremented by 2
//             bytes.
// Arg         _dest_p - pointer to potentially non-aligned address to store a 16-bit
//             integer in little host form
// Arg         _source_pp - pointer to a pointer of a potentially non-aligned address to
//             read a 16-bit integer in little endian form.  *_source_pp is incremented
//             by 2 bytes.
// See:        A_BYTE_STREAM_UI32_INC(), A_BYTE_STREAM32(), A_BYTE_STREAM_UI16_INC(),
//             A_BYTE_STREAM16(), A_BYTE_STREAM_IN16()
// Notes:      This is useful when alignment may be important and the endian type is from
//             little-endian to host-endian
// Author(s):   Conan Reis
//A_BYTE_STREAM_IN16(_dest_p, _source_pp)
// - implemented lower in file

//---------------------------------------------------------------------------------------
// Returns an 8-bit unsigned integer from _source_stream_pp - then
//             *_source_stream_pp is incremented by 1 byte.
// Returns:    an 8-bit unsigned integer
// Arg         _source_pp - pointer to a pointer of a byte stream
// See:        A_BYTE_STREAM_UI32_INC(), A_BYTE_STREAM32(), A_BYTE_STREAM_IN32(), A_BYTE_STREAM_UI16_INC(),
//             A_BYTE_STREAM16(), A_BYTE_STREAM_IN16()
// Author(s):   Conan Reis
//A_BYTE_STREAM_UI8_INC(_source_stream_pp)
// - implemented lower in file


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Alignment safe byte writing
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static void a_byte_stream_out32(void ** dest_stream_pp, const void * source_p)
  {
  uint8_t * dest_p = (uint8_t *)*dest_stream_pp;

  *((uint8_t *)dest_p)       = *((uint8_t *)source_p);
  *(((uint8_t *)dest_p) + 1) = *(((uint8_t *)source_p) + 1);
  *(((uint8_t *)dest_p) + 2) = *(((uint8_t *)source_p) + 2);
  *(((uint8_t *)dest_p) + 3) = *(((uint8_t *)source_p) + 3);

  (*(uint32_t **)(dest_stream_pp))++;
  }

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static void a_byte_stream_out16(void ** dest_stream_pp, const void * source_p)
  {
  **((uint8_t **)dest_stream_pp)      = *((uint8_t *)source_p);
  *(((uint8_t *)*dest_stream_pp) + 1) = *(((uint8_t *)source_p) + 1);
  (*(uint16_t **)(dest_stream_pp))++;
  }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Alignment safe byte writing with bytes swapped
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static void a_byte_stream_swap_out32(void ** dest_stream_pp, const void * source_p)
  {
  uint8_t * dest_p = (uint8_t *)*dest_stream_pp;

  *((uint8_t *)dest_p)       = *(((uint8_t *)source_p) + 3);
  *(((uint8_t *)dest_p) + 1) = *(((uint8_t *)source_p) + 2);
  *(((uint8_t *)dest_p) + 2) = *(((uint8_t *)source_p) + 1);
  *(((uint8_t *)dest_p) + 3) = *((uint8_t *)source_p);

  (*(uint32_t **)(dest_stream_pp))++;
  }

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static void a_byte_stream_swap_out16(void ** dest_stream_pp, const void * source_p)
  {
  **((uint8_t **)dest_stream_pp)      = *(((uint8_t *)source_p) + 1);
  *(((uint8_t *)*dest_stream_pp) + 1) = *((uint8_t *)source_p);
  (*(uint16_t **)(dest_stream_pp))++;
  }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Alignment safe byte reading
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static void a_assign32_byte(void * dest_p, const void * source_p)
  {
  *((uint8_t *)dest_p)       = *(uint8_t *)source_p;
  *(((uint8_t *)dest_p) + 1) = *((uint8_t *)source_p + 1);
  *(((uint8_t *)dest_p) + 2) = *((uint8_t *)source_p + 2);
  *(((uint8_t *)dest_p) + 3) = *((uint8_t *)source_p + 3);
  }

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static void a_assign32_byte_inc(void * dest_p, const void ** source_pp)
  {
  uint8_t * data_p = (uint8_t *)*source_pp;

  *((uint8_t *)dest_p)       = *data_p;
  *(((uint8_t *)dest_p) + 1) = *(data_p + 1);
  *(((uint8_t *)dest_p) + 2) = *(data_p + 2);
  *(((uint8_t *)dest_p) + 3) = *(data_p + 3);

  (*(uint32_t **)(source_pp))++;
  }

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static uint32_t a_as_uint32_t_byte_inc(const void ** source_pp)
  {
  uint32_t  value;
  uint8_t * dest_p = (uint8_t *)&value;
  uint8_t * data_p = (uint8_t *)*source_pp;

  *dest_p       = *data_p;
  *(dest_p + 1) = *(data_p + 1);
  *(dest_p + 2) = *(data_p + 2);
  *(dest_p + 3) = *(data_p + 3);

  (*(uint32_t **)(source_pp))++;

  return value;
  }

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static uint32_t a_as_uint32_t_byte(const void * source_p)
  {
  uint32_t  value;
  uint8_t * dest_p = (uint8_t *)&value;
  uint8_t * data_p = (uint8_t *)source_p;

  *dest_p       = *data_p;
  *(dest_p + 1) = *(data_p + 1);
  *(dest_p + 2) = *(data_p + 2);
  *(dest_p + 3) = *(data_p + 3);

  return value;
  }

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static void a_assign16_byte(void * dest_p, const void ** source_pp)
  {
  *((uint8_t *)dest_p)       = **((uint8_t **)source_pp);
  *(((uint8_t *)dest_p) + 1) = *(((uint8_t *)*source_pp) + 1);
  }

//---------------------------------------------------------------------------------------
inline static void a_assign16_byte_inc(void * dest_p, const void ** source_pp)
  {
  *((uint8_t *)dest_p)       = **((uint8_t **)source_pp);
  *(((uint8_t *)dest_p) + 1) = *(((uint8_t *)*source_pp) + 1);
  (*(uint16_t **)(source_pp))++;
  }

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static uint16_t a_as_uint16_t_byte_inc(const void ** source_pp)
  {
  uint16_t  value;
  uint8_t * dest_p = (uint8_t *)&value;

  *(dest_p)     = *(((uint8_t *)*source_pp) + 1);
  *(dest_p + 1) = **((uint8_t **)source_pp);

  (*(uint16_t **)(source_pp))++;

  return value;
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Alignment safe byte reading with bytes swapped
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static void a_assign32_swap(void * dest_p, const void * source_p)
  {
  *((uint8_t *)dest_p)       = *((uint8_t *)source_p + 3);
  *(((uint8_t *)dest_p) + 1) = *((uint8_t *)source_p + 2);
  *(((uint8_t *)dest_p) + 2) = *((uint8_t *)source_p + 1);
  *(((uint8_t *)dest_p) + 3) = *(uint8_t *)source_p;
  }

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static void a_assign32_swap_inc(void * dest_p, const void ** source_pp)
  {
  uint8_t * data_p = (uint8_t *)*source_pp;

  *((uint8_t *)dest_p)       = *(data_p + 3);
  *(((uint8_t *)dest_p) + 1) = *(data_p + 2);
  *(((uint8_t *)dest_p) + 2) = *(data_p + 1);
  *(((uint8_t *)dest_p) + 3) = *data_p;

  (*(uint32_t **)(source_pp))++;
  }

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static uint32_t a_as_uint32_t_swap_inc(const void ** source_pp)
  {
  uint32_t  value;
  uint8_t * dest_p = (uint8_t *)&value;
  uint8_t * data_p = (uint8_t *)*source_pp;

  *dest_p       = *(data_p + 3);
  *(dest_p + 1) = *(data_p + 2);
  *(dest_p + 2) = *(data_p + 1);
  *(dest_p + 3) = *data_p;

  (*(uint32_t **)(source_pp))++;

  return value;
  }

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static uint32_t a_as_uint32_t_swap(const void * source_p)
  {
  uint32_t  value;
  uint8_t * dest_p = (uint8_t *)&value;
  uint8_t * data_p = (uint8_t *)source_p;

  *dest_p       = *(data_p + 3);
  *(dest_p + 1) = *(data_p + 2);
  *(dest_p + 2) = *(data_p + 1);
  *(dest_p + 3) = *data_p;

  return value;
  }

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static void a_assign16_swap(void * dest_p, const void ** source_pp)
  {
  *((uint8_t *)dest_p)       = *(((uint8_t *)*source_pp) + 1);
  *(((uint8_t *)dest_p) + 1) = **((uint8_t **)source_pp);
  }

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static void a_assign16_swap_inc(void * dest_p, const void ** source_pp)
  {
  *((uint8_t *)dest_p)       = *(((uint8_t *)*source_pp) + 1);
  *(((uint8_t *)dest_p) + 1) = **((uint8_t **)source_pp);
  (*(uint16_t **)(source_pp))++;
  }

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
inline static uint16_t a_as_uint16_t_swap_inc(const void ** source_pp)
  {
  uint16_t  value;
  uint8_t * dest_p = (uint8_t *)&value;

  *(dest_p)     = *(((uint8_t *)*source_pp) + 1);
  *(dest_p + 1) = **((uint8_t **)source_pp);

  (*(uint16_t **)(source_pp))++;

  return value;
  }


//---------------------------------------------------------------------------------------
// Platform independent Macros
//---------------------------------------------------------------------------------------

#if (A_BYTE_STREAM_NEEDS_SWAP == 1)

  // Note that alignment does not matter if bytes need to be swapped.

  //---------------------------------------------------------------------------------------
  // Assumes byte stream is in different endian form from host and swaps bytes

  #define A_BYTE_STREAM32(_dest_p, _source_p)               a_assign32_swap((_dest_p), (_source_p))
  #define A_BYTE_STREAM_OUT32(_dest_stream_pp, _source_p)   a_byte_stream_swap_out32(reinterpret_cast<void **>(_dest_stream_pp), (_source_p))
  #define A_BYTE_STREAM_IN32(_dest_p, _source_pp)           a_assign32_swap_inc((_dest_p), (const void **)(_source_pp))
  #define A_BYTE_STREAM_UI32_INC(_source_pp)                a_as_uint32_t_swap_inc((const void **)(_source_pp))
  #define A_BYTE_STREAM_UI32(_source_p)                     a_as_uint32_t_swap(_source_p)

  #define A_BYTE_STREAM16(_dest_p, _source_p)               a_assign16_swap((_dest_p), (_source_p))
  #define A_BYTE_STREAM_OUT16(_dest_stream_pp, _source_p)   a_byte_stream_swap_out16(reinterpret_cast<void **>(_dest_stream_pp), (_source_p))
  #define A_BYTE_STREAM_IN16(_dest_p, _source_pp)           a_assign16_swap_inc((_dest_p), (const void **)(_source_pp))
  #define A_BYTE_STREAM_UI16_INC(_source_pp)                a_as_uint16_t_swap_inc((const void **)(_source_pp))


#else  // No byte swap needed

  #ifdef AGOG_ALIGNMENT32

    //---------------------------------------------------------------------------------------
    // Assumes byte stream is in same endian form and leaves bytes in the same order, but it
    // copies one byte at a time to allow for alignment offsets

    #define A_BYTE_STREAM32(_dest_p, _source_p)               a_assign32_byte((_dest_p), (_source_p))
    #define A_BYTE_STREAM_OUT32(_dest_stream_pp, _source_p)   a_byte_stream_out32(reinterpret_cast<void **>(_dest_stream_pp), (_source_p))
    #define A_BYTE_STREAM_IN32(_dest_p, _source_pp)           a_assign32_byte_inc((_dest_p), (const void **)(_source_pp))
    #define A_BYTE_STREAM_UI32_INC(_source_pp)                a_as_uint32_t_byte_inc((const void **)(_source_pp))
    #define A_BYTE_STREAM_UI32(_source_p)                     a_as_uint32_t_byte(_source_p)

    #define A_BYTE_STREAM16(_dest_p, _source_p)               a_assign16_byte((_dest_p), (_source_p))
    #define A_BYTE_STREAM_OUT16(_dest_stream_pp, _source_p)   a_byte_stream_out16(reinterpret_cast<void **>(_dest_stream_pp), (_source_p))
    #define A_BYTE_STREAM_IN16(_dest_p, _source_pp)           a_assign16_byte_inc((_dest_p), (const void **)(_source_pp))
    #define A_BYTE_STREAM_UI16_INC(_source_pp)                a_as_uint16_t_byte_inc((const void **)(_source_pp))


  #else  // Not word aligned

    //---------------------------------------------------------------------------------------
    // Assumes byte stream and destination are in the same endian form and alignment is not important


    #define A_BYTE_STREAM32(_dest_p, _source_p)               ( *((uint32_t *)(_dest_p)) = *((const uint32_t *)(_source_p)) )
    #define A_BYTE_STREAM_OUT32(_dest_stream_pp, _source_p)   ( *((*(uint32_t **)(_dest_stream_pp))++) = *((const uint32_t *)(_source_p)) )
    #define A_BYTE_STREAM_IN32(_dest_p, _source_pp)           ( *((uint32_t *)(_dest_p)) = *((*(const uint32_t **)(_source_pp))++) )
    #define A_BYTE_STREAM_UI32_INC(_source_pp)                ( *((*(const uint32_t **)(_source_pp))++) )
    #define A_BYTE_STREAM_UI32(_source_p)                     ( *(const uint32_t *)(_source_p) )

    #define A_BYTE_STREAM16(_dest_p, _source_p)               ( *((uint16_t *)(_dest_p)) = *((const uint16_t *)(_source_p)) )
    #define A_BYTE_STREAM_OUT16(_dest_stream_pp, _source_p)   ( *((*(uint16_t **)(_dest_stream_pp))++) = *((const uint16_t *)(_source_p)) )
    #define A_BYTE_STREAM_IN16(_dest_p, _source_pp)           ( *((uint16_t *)(_dest_p)) = *((*(const uint16_t **)(_source_pp))++) )
    #define A_BYTE_STREAM_UI16_INC(_source_pp)                ( *((*(const uint16_t **)(_source_pp))++) )

  #endif  // AGOG_ALIGNMENT32

#endif // (A_BYTE_STREAM_NEEDS_SWAP == 1)


#define A_BYTE_STREAM_OUT8(_dest_stream_pp, _source_p)    ( *((*(uint8_t **)(_dest_stream_pp))++) = *((const uint8_t *)(_source_p)) )
#define A_BYTE_STREAM_UI8_INC(_source_stream_pp)          ( *((*(const uint8_t **)(_source_stream_pp))++) )


#endif  // __ABINARYPARSE_HPP


