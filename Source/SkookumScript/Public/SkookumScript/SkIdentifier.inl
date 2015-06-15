//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Identifier for named code objects - i.e. temporary variables, etc.
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkSymbol.hpp>


//=======================================================================================
// SkIdentifier Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Constructor
// Returns:    itself
// Arg         identifier_name - Name of a parameter variable or a temporary variable to
//             identify/refer to.
// Examples:   Called by SkParser
// See:        SkParser
// Author(s):   Conan Reis
A_INLINE SkIdentifier::SkIdentifier(const ASymbol & ident_name) :
  m_ident_name(ident_name)
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
//
//             Little error checking is done on the binary info as it assumed that it
//             previously validated upon input.
// Author(s):   Conan Reis
A_INLINE SkIdentifier::SkIdentifier(const void ** binary_pp) :
  m_ident_name(ASymbol::create_from_binary(binary_pp))
  {
  }

#endif // (SKOOKUM & SK_COMPILED_IN)


//=======================================================================================
// SkIdentifierMember Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Constructor
// Returns:    itself
// Arg         identifier_name - Name of a member data variable to identify/refer to.
// Examples:   Called by SkParser
// See:        SkParser
// Author(s):   Conan Reis
A_INLINE SkIdentifierMember::SkIdentifierMember(
  const ASymbol &    member_name,
  SkExpressionBase * owner_p
  ) :
  SkIdentifier(member_name),
  m_owner_p(owner_p)
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
//               4 bytes - data member name id
//               n bytes - owner expression typed binary or inferred this (nullptr)
//
//             Little error checking is done on the binary info as it assumed that it
//             previously validated upon input.
// Author(s):   Conan Reis
A_INLINE SkIdentifierMember::SkIdentifierMember(const void ** binary_pp) :
  // 4 bytes - identifier name id
  SkIdentifier(binary_pp)
  {
  // n bytes - owner expression typed binary or inferred this (nullptr)
  m_owner_p = SkExpressionBase::from_binary_typed_new(binary_pp);
  }

#endif // (SKOOKUM & SK_COMPILED_IN)


//=======================================================================================
// SkIdentifierClassMember Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Constructor
// Returns:    itself
// Arg         identifier_name - Name of a member data variable to identify/refer to.
// Examples:   Called by SkParser
// See:        SkParser
// Author(s):   Conan Reis
A_INLINE SkIdentifierClassMember::SkIdentifierClassMember(
  const ASymbol &    class_member_name,
  SkExpressionBase * owner_p
  ) :
  SkIdentifierMember(class_member_name, owner_p)
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
//               4 bytes - class data member name id
//               n bytes - owner expression typed binary or inferred this (nullptr)
//
//             Little error checking is done on the binary info as it assumed that it
//             previously validated upon input.
// Author(s):   Conan Reis
A_INLINE SkIdentifierClassMember::SkIdentifierClassMember(const void ** binary_pp) :
  SkIdentifierMember(binary_pp)
  {
  }

#endif // (SKOOKUM & SK_COMPILED_IN)

