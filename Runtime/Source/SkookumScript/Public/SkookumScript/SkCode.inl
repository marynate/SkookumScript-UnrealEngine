//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Classes for custom/compound expressions
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkSymbol.hpp>
#include <SkookumScript/SkIdentifier.hpp>


//=======================================================================================
// SkCode Inline Methods
//=======================================================================================

#if (SKOOKUM & SK_COMPILED_IN)

//---------------------------------------------------------------------------------------
// Constructor from binary info
// Returns:    itself
// Arg         binary_pp - Pointer to address to read binary serialization info from and
//             to increment - previously filled using as_binary() or a similar mechanism.
// See:        as_binary()
// Notes:      Binary composition:
//               4 bytes - number of temp variables
//               4 bytes - temp variable name id }- Repeating 
//               4 bytes - number of statements
//               n bytes - statement typed binary }- Repeating 
//
//             Little error checking is done on the binary info as it assumed that it was
//             previously validated upon input.
// Author(s):   Conan Reis
A_INLINE SkCode::SkCode(const void ** binary_pp)
  {
  assign_binary(binary_pp);
  }

#endif // (SKOOKUM & SK_COMPILED_IN)


//=======================================================================================
// SkConcurrentSync Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Constructor
// Returns:    itself
// Arg         expr_p - 1st durational expression to run as a converging concurrent thread
// Examples:   Called by SkParser
// Author(s):   Conan Reis
A_INLINE SkConcurrentSync::SkConcurrentSync(SkExpressionBase * expr_p)
  {
  m_exprs.append(*expr_p);
  }

//---------------------------------------------------------------------------------------
// Constructor
// Returns:    itself
// Arg         exprs_p - address of durational expressions to take ownership of and to
//             run as converging concurrent threads.
// Examples:   Called by SkParser
// Author(s):   Conan Reis
A_INLINE SkConcurrentSync::SkConcurrentSync(APCompactArrayBase<SkExpressionBase> * exprs_p) :
  m_exprs(exprs_p)
  {
  }


//=======================================================================================
// SkConcurrentRace Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Constructor
// Returns:    itself
// Arg         expr_p - 1st durational expression to run as a converging concurrent thread
// Examples:   Called by SkParser
// Author(s):   Conan Reis
A_INLINE SkConcurrentRace::SkConcurrentRace(SkExpressionBase * expr_p)
  {
  m_exprs.append(*expr_p);
  }

//---------------------------------------------------------------------------------------
// Constructor
// Returns:    itself
// Arg         exprs_p - address of durational expressions to take ownership of and to
//             run as converging concurrent threads.
// Examples:   Called by SkParser
// Author(s):   Conan Reis
A_INLINE SkConcurrentRace::SkConcurrentRace(APCompactArrayBase<SkExpressionBase> * exprs_p) :
  m_exprs(exprs_p)
  {
  }


//=======================================================================================
// SkConcurrentBranch Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Constructor
// Returns:    itself
// Arg         statement_p - statement to branch
// Examples:   Called by SkParser
// See:        SkParser
// Notes:      The branch 'expression' is only valid as a statement within a code block.
// Author(s):   Conan Reis
A_INLINE SkConcurrentBranch::SkConcurrentBranch(SkExpressionBase * statement_p) :
  m_statement_p(statement_p)
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
//               n bytes - statement typed binary
//
//             Little error checking is done on the binary info as it assumed that it
//             previously validated upon input.
// Author(s):   Conan Reis
A_INLINE SkConcurrentBranch::SkConcurrentBranch(const void ** binary_pp) :
  m_statement_p(SkExpressionBase::from_binary_typed_new(binary_pp))
  {
  }

#endif // (SKOOKUM & SK_COMPILED_IN)


//=======================================================================================
// SkDivert Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Constructor
// Returns:    itself
// Arg         statement_p - statement to divert
// Examples:   Called by SkParser
// See:        SkParser
// Author(s):   Conan Reis
A_INLINE SkDivert::SkDivert(SkExpressionBase * statement_p) :
  m_statement_p(statement_p)
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
//               n bytes - statement typed binary
//
//             Little error checking is done on the binary info as it assumed that it
//             previously validated upon input.
// Author(s):   Conan Reis
A_INLINE SkDivert::SkDivert(const void ** binary_pp) :
  m_statement_p(SkExpressionBase::from_binary_typed_new(binary_pp))
  {
  }

#endif // (SKOOKUM & SK_COMPILED_IN)


//=======================================================================================
// SkBind Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Default Constructor
// Returns:    itself
// Examples:   Called by SkParser
// See:        SkParser
// Author(s):   Conan Reis
A_INLINE SkBind::SkBind() :
  m_ident_p(nullptr),
  m_expr_p(nullptr)
  {
  }

//---------------------------------------------------------------------------------------
// Constructor
// Returns:    itself
// Arg         ident_p - variable to rebind.  It must be dynamically allocated and this
//             structure will delete it when it is destructed.
// Arg         expr_p - expression to bind variable to.  It must be dynamically
//             allocated and this structure will delete it when it is destructed.
// Examples:   Called by SkParser
// See:        SkParser
// Author(s):   Conan Reis
A_INLINE SkBind::SkBind(
  SkIdentifier *     ident_p,
  SkExpressionBase * expr_p
  ) :
  m_ident_p(ident_p),
  m_expr_p(expr_p)
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
//               n bytes - identifier expression typed binary
//               n bytes - expression typed binary
//
//             Little error checking is done on the binary info as it assumed that it
//             previously validated upon input.
// Author(s):   Conan Reis
A_INLINE SkBind::SkBind(const void ** binary_pp)
  {
  // n bytes - identifier expression typed binary
  m_ident_p = static_cast<SkIdentifier *>(SkExpressionBase::from_binary_typed_new(binary_pp));

  // n bytes - expression typed binary
  m_expr_p = SkExpressionBase::from_binary_typed_new(binary_pp);
  }

#endif // (SKOOKUM & SK_COMPILED_IN)

