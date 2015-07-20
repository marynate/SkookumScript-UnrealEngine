//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Classes for expressions that can be conditionally evaluated/invoked
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkSymbol.hpp>


//=======================================================================================
// SkConditional Inline Methods
//=======================================================================================

#if (SKOOKUM & SK_COMPILED_IN)

//---------------------------------------------------------------------------------------
// Constructor from binary info
// Returns:    itself
// Arg         binary_pp - Pointer to address to read binary serialization info from and
//             to increment - previously filled using as_binary() or a similar mechanism.
// See:        as_binary()
// Notes:      Binary composition:
//               4 bytes - number of clauses
//               n bytes - clause binary }- Repeating 
//
//             Little error checking is done on the binary info as it assumed that it was
//             previously validated upon input.
// Author(s):   Conan Reis
A_INLINE SkConditional::SkConditional(const void ** binary_pp)
  {
  assign_binary(binary_pp);
  }

#endif // (SKOOKUM & SK_COMPILED_IN)



//=======================================================================================
// SkCase Inline Methods
//=======================================================================================

#if (SKOOKUM & SK_COMPILED_IN)

//---------------------------------------------------------------------------------------
// Constructor from binary info
// Returns:    itself
// Arg         binary_pp - Pointer to address to read binary serialization info from and
//             to increment - previously filled using as_binary() or a similar mechanism.
// See:        as_binary()
// Notes:      Binary composition:
//               4 bytes - number of clauses
//               n bytes - clause binary }- Repeating 
//
//             Little error checking is done on the binary info as it assumed that it was
//             previously validated upon input.
// Author(s):   Conan Reis
A_INLINE SkCase::SkCase(const void ** binary_pp)
  {
  assign_binary(binary_pp);
  }

#endif // (SKOOKUM & SK_COMPILED_IN)


//=======================================================================================
// SkWhen Inline Methods
//=======================================================================================

#if (SKOOKUM & SK_COMPILED_IN)

//---------------------------------------------------------------------------------------
// Constructor from binary info
// Returns:    itself
// Arg         binary_pp - Pointer to address to read binary serialization info from and
//             to increment - previously filled using as_binary() or a similar mechanism.
// See:        as_binary()
// Notes:      Binary composition:
//               n bytes - clause expression
//               n bytes - test expression
//
//             Little error checking is done on the binary info as it assumed that it was
//             previously validated upon input.
// Author(s):   Conan Reis
A_INLINE SkWhen::SkWhen(const void ** binary_pp)
  {
  assign_binary(binary_pp);
  }

#endif // (SKOOKUM & SK_COMPILED_IN)
