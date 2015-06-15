//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Invokable parameters & body class
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKINVOKABLEBASE_HPP
#define __SKINVOKABLEBASE_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkQualifier.hpp>
#include <SkookumScript/SkParameters.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declaration
struct SkApplyExpressionBase;
class  SkExpressionBase;

//---------------------------------------------------------------------------------------
// Notes      SkookumScript InvokableBase
// Subclasses SkMethodBase (SkMethod, SkMethodFunc, SkMethodMthd), SkCoroutineBase (SkCoroutine,
//            SkCoroutineMthd, SkCoroutineFunc)
// Author(s)  Conan Reis
class SkInvokableBase : public SkQualifier
  {
  public:

  // Common Methods

    SkInvokableBase();
    SkInvokableBase(const ASymbol & name, SkClass * scope_p);
    SkInvokableBase(const ASymbol & name, SkClass * scope_p, SkParameters * params_p);
    SkInvokableBase(const ASymbol & name, SkClassDescBase * result_type_p, SkParameterBase * param_p);
    virtual ~SkInvokableBase();

  // Methods

    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const = 0;
      virtual AString as_code_params() const = 0;
    #endif

    // Debugging Methods
    #if (SKOOKUM & SK_DEBUG)
      SkExpressionBase * find_expr_by_pos(uint32_t pos, eSkExprFind type = SkExprFind_all) const;
      SkExpressionBase * find_expr_on_pos(uint32_t pos, eSkExprFind type = SkExprFind_all) const;
      SkExpressionBase * find_expr_span(uint32_t source_idx, uint32_t * idx_begin_p = nullptr, uint32_t * idx_end_p = nullptr, eSkExprFind type = SkExprFind_all) const;
      void               get_expr_span(const SkExpressionBase & expr, uint32_t * idx_begin_p, uint32_t * idx_end_p) const;
      eSkMember          get_member_type() const;
      eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p);
    #endif

    AString                    as_string_name(bool qualified = true) const;
    SkInvokableBase &          assign(const SkInvokableBase & invokable)  { SkQualifier::operator=(invokable); m_params_p = invokable.m_params_p; return *this; }
    virtual SkExpressionBase * get_custom_expr() const;
    const SkParameters &       get_params() const                         { return *m_params_p; }
    SkParameters &             get_params()                               { return *m_params_p; }
    void                       set_params(SkParameters * params_p)        { m_params_p = params_p; }
    virtual eSkInvokable       get_invoke_type() const = 0;
    virtual bool               is_bound() const = 0;
    virtual bool               is_class_member() const = 0;
    virtual bool               is_placeholder();
    //virtual SkInvokableBase * evaluate(SkObjectBase * scope_p, SkInvokedBase * caller_p) = 0;
    //virtual SkInvokableBase * give_result(SkInstance * result_p, SkInvokableBase * sub_expr_p, SkInvokedBase * caller_p) = 0;

  protected:

  // Data Members

    ARefPtr<SkParameters> m_params_p;

    // Future: ADebug / content creation data structure for parameter descriptions

  };  // SkInvokableBase


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkInvokableBase.inl>
#endif


#endif  // __SKINVOKABLEBASE_HPP

