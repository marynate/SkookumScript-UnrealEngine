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


#ifndef __SKCOROUTINECALL_HPP
#define __SKCOROUTINECALL_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkInvocation.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// Notes      Info used to make a coroutine call/invocation - i.e. coroutine identifier
//            (name/index) and passed argument info.
// Author(s)  Conan Reis
class SkCoroutineCall : public SkInvokeBase
  {
  friend class SkParser;

  public:
	  SK_NEW_OPERATORS(SkCoroutineCall);
  // Common Methods

    SkCoroutineCall(const ASymbol & coroutine_name = ASymbol::get_null(), SkClass * scope_p = nullptr);
    SkCoroutineCall(SkInvokeBase * call_p);

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)

      SkCoroutineCall(const void ** binary_pp);

    #endif // (SKOOKUM & SK_COMPILED_IN)


  // Methods

    // Overriding from SkInvokeBase

    virtual eSkInvokeType   get_invoke_type() const;
    virtual SkInvokedBase * invoke_call(SkInstance * receiver_p, SkObjectBase * scope_p, SkInvokedBase * caller_p, SkInstance ** result_pp) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

  };  // SkCoroutineCall


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkCoroutineCall.inl>
#endif


#endif  // __SKCOROUTINECALL_HPP

