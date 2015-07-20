//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Invoked closure classes: SkInvokeClosureBase, SkInvokeClosureMethod and
//             SkInvokeClosureCoroutine
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKINVOKECLOSURE_HPP
#define __SKINVOKECLOSURE_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkExpressionBase.hpp>
#include <AgogCore/APCompactArray.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declarations
class SkIdentifier;
class SkClosure;

//---------------------------------------------------------------------------------------
class SkInvokeClosureBase : public SkExpressionBase
  {
  friend class SkParser;

  public:
  // Common Methods

    SkInvokeClosureBase(SkExpressionBase * receiver_p = nullptr);
    SkInvokeClosureBase(SkExpressionBase * receiver_p, APCompactArray<SkExpressionBase> * send_args_p, APCompactArray<SkIdentifier> * return_args_p);
    virtual ~SkInvokeClosureBase();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkInvokeClosureBase(const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual void null_receiver(SkExpressionBase * receiver_p);
    virtual void track_memory(AMemoryStats * mem_stats_p) const;

  // Debugging Methods

    #if (SKOOKUM & SK_DEBUG)
      virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif

  protected:

  // Internal Methods

    void invoke_method(SkClosure * closure_p, SkObjectBase * scope_p, SkInvokedBase * caller_p, SkInstance ** result_pp) const;

    
  // Data Members

    // The object that results from the evaluation of this expression is the
    // target/subject/receiver of subroutine 'm_call_p'.  If 'm_receiver_p' is nullptr
    // then 'this' - i.e. the topmost scope - is inferred.
    SkExpressionBase * m_receiver_p;

    // Array of dynamic expressions to be invoked and have their instance results passed
    // to the method/coroutine.
    //   - Arguments fill the parameters based on the order of their index position
    //   - Arguments that are nullptr indicate that they should be filled with the
    //     corresponding default argument from the parameter list
    //
    // $Revisit - CReis Future: Each default expression could be put directly in place [the
    // same data structure as the parameter list could be used, but a mechanism so that
    // they are not deleted more than once must be determined].  This future plan for
    // argument lists requires that the available invokables (and their parameter lists)
    // are in memory and that it is possible to determine the class scope of the
    // invokable call.  [See SkInvokedContextBase::data_append_args()]
    APCompactArray<SkExpressionBase> m_arguments;

    // Return argument bindings
    //   - Argument identifiers fill the return parameters based on the order of their index position
    //   - Argument identifiers that are nullptr indicate that they are skipped/ignored.
    // $Revisit - [Memory] CReis since few closures use return args it would save memory
    // to have this class without them and a separate SkInvokeClosureRArgs with them.
    APCompactArray<SkIdentifier> m_return_args;

  };  // SkInvokeClosureBase


//---------------------------------------------------------------------------------------
class SkInvokeClosureMethod : public SkInvokeClosureBase
  {
  friend class SkParser;

  public:
  // Common Methods

    SK_NEW_OPERATORS(SkInvokeClosureMethod);

    SkInvokeClosureMethod(SkExpressionBase * receiver_p = nullptr)
      : SkInvokeClosureBase(receiver_p) {}
    SkInvokeClosureMethod(SkExpressionBase * receiver_p, APCompactArray<SkExpressionBase> * send_args_p, APCompactArray<SkIdentifier> * return_args_p)
      : SkInvokeClosureBase(receiver_p, send_args_p, return_args_p) {}

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkInvokeClosureMethod(const void ** binary_pp)
        : SkInvokeClosureBase(binary_pp) {}
    #endif

  // Methods

    virtual eSkExprType     get_type() const                                    { return SkExprType_invoke_closure_method; }
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual bool            is_immediate(uint32_t * durational_idx_p = nullptr) const  { return true; }

  };  // SkInvokeClosureMethod

//---------------------------------------------------------------------------------------
class SkInvokeClosureCoroutine : public SkInvokeClosureBase
  {
  friend class SkParser;

  public:
  // Common Methods

    SK_NEW_OPERATORS(SkInvokeClosureCoroutine);

    SkInvokeClosureCoroutine(SkExpressionBase * receiver_p = nullptr)
      : SkInvokeClosureBase(receiver_p) {}
    SkInvokeClosureCoroutine(SkExpressionBase * receiver_p, APCompactArray<SkExpressionBase> * send_args_p, APCompactArray<SkIdentifier> * return_args_p)
      : SkInvokeClosureBase(receiver_p, send_args_p, return_args_p) {}

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkInvokeClosureCoroutine(const void ** binary_pp)
        : SkInvokeClosureBase(binary_pp) {}
    #endif

  // Methods

    virtual eSkExprType     get_type() const                                    { return SkExprType_invoke_closure_coroutine; }
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual bool            is_immediate(uint32_t * durational_idx_p = nullptr) const;

  };  // SkInvokeClosureCoroutine


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkInvokeClosure.inl>
#endif


#endif  // __SKINVOKECLOSURE_HPP

