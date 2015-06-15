//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// SkookumScript Object Identifier
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkClass.hpp>
#include <SkookumScript/SkSymbol.hpp>


//=======================================================================================
// SkObjectId Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Constructor
// Returns:    itself
// Examples:   Called by SkParser
// See:        SkParser
// Author(s):   Conan Reis
A_INLINE SkObjectId::SkObjectId(
  const ASymbol & ident_name,
  SkClass * class_p,
  uint32_t flags
  ) :
  m_name(ident_name),
  m_class_p(class_p),
  m_flags(flags)
  {
  }


#if (SKOOKUM & SK_COMPILED_IN)

//---------------------------------------------------------------------------------------
// Constructor from binary info
// Returns:    itself
// Arg         binary_pp - Pointer to address to read binary serialization info from and
//             to increment - previously filled using as_binary() or a similar mechanism.
// See:        as_binary()
// Notes:      Binary composition:
//               4 bytes - identifier name id
//               4 bytes - class name id
//               1 byte  - flags
//
//             Little error checking is done on the binary info as it assumed that it
//             previously validated upon input.
// Author(s):   Conan Reis
A_INLINE SkObjectId::SkObjectId(const void ** binary_pp) :
  // 4 bytes - identifier name id
  m_name(ASymbol::create_from_binary(binary_pp))
  {
  // 4 bytes - class name id
  m_class_p = SkClass::from_binary_ref(binary_pp);

  // 1 byte - flags (only bother storing first 8 flags)
  m_flags = A_BYTE_STREAM_UI8_INC(binary_pp);
  }

#endif // (SKOOKUM & SK_COMPILED_IN)


#if (SKOOKUM & SK_CODE_IN)

//---------------------------------------------------------------------------------------
// Ensures this is name id is valid for the associated class.
//
// #Author(s) Conan Reis
A_INLINE SkClass * SkObjectId::validate(
  bool validate_deferred // = true
  )
  {
  return m_class_p->object_id_validate(this, validate_deferred);
  }


#endif // (SKOOKUM & SK_CODE_IN)

