//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Class wrapper for info used to make a method call/invocation - i.e. coroutine
//             identifier (name/index) and passed argument info.
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================


//=======================================================================================
// Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
//  Constructor
// Returns:     itself
// Arg          coroutine_name - name of the coroutine to call (Default ASymbol::ms_null)
// Arg          scope_p - optional class scope to use (Default nullptr)
// Author(s):    Conan Reis
A_INLINE SkCoroutineCall::SkCoroutineCall(
  const ASymbol & coroutine_name, // = ASymbol::ms_null,
  SkClass *       scope_p        // = nullptr
  ) :
  SkInvokeBase(coroutine_name, scope_p)
  {
  }

//---------------------------------------------------------------------------------------
// Transfer copy constructor - takes over internal contents of supplied
//             'call_p' and sets it to default values.
// Returns:    itself
// Arg         call_p - SkInvokeBase object to take over contents of
// Notes:      This method is useful when the contents of a local stack allocated
//             SkInvokeBase derived object needs to be promoted to a dynamic heap
//             allocated object.
// Author(s):   Conan Reis
A_INLINE SkCoroutineCall::SkCoroutineCall(SkInvokeBase * call_p) :
  SkInvokeBase(call_p)
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
//               4 bytes - coroutine name id
//               4 bytes - scope name id or ASymbol_id_null if not used
//               4 bytes - number of arguments
//               1 byte  - argument type     \_ Repeating
//               n bytes - argument binary   / 
//
//             Little error checking is done on the binary info as it assumed that it
//             previously validated upon input.
// Author(s):   Conan Reis
A_INLINE SkCoroutineCall::SkCoroutineCall(const void ** binary_pp)
  {
  assign_binary(binary_pp);
  }

#endif // (SKOOKUM & SK_COMPILED_IN)


