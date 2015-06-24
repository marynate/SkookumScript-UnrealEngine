//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Class wrapper for info used to make a method call/invocation - i.e. method
//             identifier (name/index) and passed argument info.
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKMETHODCALL_HPP
#define __SKMETHODCALL_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkInvocation.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// Notes      Info used to make a method call/invocation - i.e. method identifier
//            (name/index) and passed argument info.
// Author(s)  Conan Reis
class SkMethodCall : public SkInvokeBase
  {
  friend class SkParser;

  public:
  // Common Methods

    SK_NEW_OPERATORS(SkMethodCall);
    SkMethodCall(const ASymbol & method_name = ASymbol::get_null(), SkClass * scope_p = nullptr) : SkInvokeBase(method_name, scope_p) {}
    SkMethodCall(SkInvokeBase * call_p) : SkInvokeBase(call_p) {}

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)

      SkMethodCall(const void ** binary_pp) { assign_binary(binary_pp); }

    #endif // (SKOOKUM & SK_COMPILED_IN)


  // Methods

    #if (SKOOKUM & SK_CODE_IN)

      bool is_constructor() const;

    #endif // (SKOOKUM & SK_CODE_IN)


    // Overriding from SkInvokeBase

    virtual eSkInvokeType   get_invoke_type() const;
    virtual SkInvokedBase * invoke_call(SkInstance * receiver_p, SkObjectBase * scope_p, SkInvokedBase * caller_p, SkInstance ** result_pp) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

  };  // SkMethodCall


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkMethodCall.inl>
#endif


#endif  // __SKMETHODCALL_HPP

