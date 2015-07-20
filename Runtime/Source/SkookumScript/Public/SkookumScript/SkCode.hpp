//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Classes for custom/compound expressions
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKCODE_HPP
#define __SKCODE_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AVCompactSorted.hpp>
#include <SkookumScript/SkExpressionBase.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

// Forward declarations - to avoid unnecessary includes
class SkInvokedExpression;
class SkIdentifier;


//---------------------------------------------------------------------------------------
// Notes      SkookumScript Code block - sub part of custom methods & coroutines
//
//            code-block          = '[' {ws statement ws statement-delimiter} [ws statement] ws ']'
//            statement           = expression | create-temporary | loop-exit
//            statement-delimiter = '.' | ',' | '%'
// Author(s)  Conan Reis
class SkCode : public SkExpressionBase
  {
  friend class SkParser;
  friend class SkInvokedExpression;

  public:
  // Common Methods

    SK_NEW_OPERATORS(SkCode);

    SkCode() {}
    virtual ~SkCode();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkCode(const void ** binary_pp);
      void assign_binary(const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if (SKOOKUM & SK_CODE_IN)
      virtual const SkExpressionBase * find_expr_last_no_side_effect() const;
      virtual eSkSideEffect            get_side_effect() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    // Overridden from SkEXpressionBase

    virtual eSkExprType     get_type() const;
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual bool            is_immediate(uint32_t * durational_idx_p = nullptr) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

    // Called by SkInvokedExpression

    virtual bool invoke_iterate(SkInvokedExpression * iexpr_p, SkInstance ** result_pp = nullptr) const;
    virtual void invoke_exit(SkInvokedExpression * iexpr_p, SkInvokedExpression * sub_exit_p = nullptr) const;

    // Debugging Methods
    #if (SKOOKUM & SK_DEBUG)
      virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      virtual SkDebugInfo        get_debug_info(const SkInvokedExpression & iexpr) const;
      virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif

  protected:

  // Data Members

    // Temporary variables made by this code block.
    AVCompactSortedLogical<ASymbol> m_temp_vars;

    // Statements in order of iteration
    APCompactArrayFree<SkExpressionBase> m_statements;

  };  // SkCode

//---------------------------------------------------------------------------------------
// Notes      SkookumScript concurrent convergent threaded expressions.
//            Call each statement in code block  simultaneously and do not call the next
//            statement until the *last* statement - i.e. all the statements - in code
//            block are complete.
//
//            thread
//              [
//              _expr1()
//              _expr2()
//              _expr3()
//              ]
//
// Author(s)  Conan Reis
class SkConcurrentSync : public SkExpressionBase
  {
  friend class SkParser;

  public:
  // Common Methods

    SK_NEW_OPERATORS(SkConcurrentSync);

    SkConcurrentSync(SkExpressionBase * expr_p);
    SkConcurrentSync(APCompactArrayBase<SkExpressionBase> * exprs_p);

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkConcurrentSync(const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual eSkExprType     get_type() const;
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual void            invoke_exit(SkInvokedExpression * iexpr_p, SkInvokedExpression * sub_exit_p = nullptr) const;
    virtual bool            is_immediate(uint32_t * durational_idx_p = nullptr) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

    // Debugging Methods
    #if (SKOOKUM & SK_DEBUG)
      virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif

  protected:

  // Data Members

    // Durational expressions
    APCompactArrayFree<SkExpressionBase> m_exprs;

  };  // SkConcurrentSync


//---------------------------------------------------------------------------------------
// Notes      SkookumScript concurrent racing expressions
//            Call each statement in code block simultaneously and do not call the next
//            statement until the *first* statement is complete.
//
//            race
//              [
//              _expr1()
//              _expr2()
//              _expr3()
//              ]
//
//            This class is very similar to SkInvokeRace and changes to this class might
//            need to be reflected there and vice-versa.
// Author(s)  Conan Reis
class SkConcurrentRace : public SkExpressionBase
  {
  friend class SkParser;

  public:
  // Common Methods

    SK_NEW_OPERATORS(SkConcurrentRace);

    SkConcurrentRace(SkExpressionBase * expr_p);
    SkConcurrentRace(APCompactArrayBase<SkExpressionBase> * exprs_p);

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkConcurrentRace(const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual eSkExprType     get_type() const;
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual void            invoke_exit(SkInvokedExpression * iexpr_p, SkInvokedExpression * sub_exit_p = nullptr) const;
    virtual bool            invoke_iterate(SkInvokedExpression * iexpr_p, SkInstance ** result_pp = nullptr) const;
    virtual bool            is_immediate(uint32_t * durational_idx_p = nullptr) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

    // Debugging Methods
    #if (SKOOKUM & SK_DEBUG)
      virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif

  protected:

  // Data Members

    // Durational expressions
    APCompactArrayFree<SkExpressionBase> m_exprs;

  };  // SkConcurrentRace


//---------------------------------------------------------------------------------------
// Notes      SkookumScript concurrent branched expression
//            Call block at the same time as next statement, but do not wait for block to
//            return.
//
//            branch [ _expr() ]
//
// Author(s)  Conan Reis
class SkConcurrentBranch : public SkExpressionBase
  {
  friend class SkParser;

  public:

  // Common Methods

    SK_NEW_OPERATORS(SkConcurrentBranch);

    SkConcurrentBranch(SkExpressionBase * statement_p);
    virtual ~SkConcurrentBranch();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkConcurrentBranch(const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual eSkExprType     get_type() const;
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual bool            invoke_iterate(SkInvokedExpression * iexpr_p, SkInstance ** result_pp = nullptr) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

    // Debugging Methods
    #if (SKOOKUM & SK_DEBUG)
      virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif

  protected:

  // Data Members

    SkExpressionBase * m_statement_p;

  };  // SkConcurrentBranch


//---------------------------------------------------------------------------------------
// Notes      SkookumScript divert expression
//            Use receiver of coroutines as updater rather than updater of caller.
//
//            divert [ _expr() ]
//
// Author(s)  Conan Reis
class SkDivert : public SkExpressionBase
  {
  friend class SkParser;

  public:

  // Common Methods

    SK_NEW_OPERATORS(SkDivert);

    SkDivert(SkExpressionBase * statement_p);
    virtual ~SkDivert();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkDivert(const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if (SKOOKUM & SK_CODE_IN)
      virtual const SkExpressionBase * find_expr_last_no_side_effect() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual eSkExprType     get_type() const;
    virtual SkMind *        get_updater(const SkInvokedExpression & iexpr) const;
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual bool            is_immediate(uint32_t * durational_idx_p = nullptr) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

    // Debugging Methods
    #if (SKOOKUM & SK_DEBUG)
      virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif

  protected:

  // Data Members

    SkExpressionBase * m_statement_p;

  };  // SkDivert


//---------------------------------------------------------------------------------------
// Notes      SkookumScript variable bind expression.  Could be a member variable, a
//            parameter variable, or a temporary variable.
//
//            [expression "."] identifier_name ":" expr
//
//            * Instead of this data structure, a bind could have been a method called on
//            an inferred 'this_code' with a method call similar to:
//              this_code bind('identifier_name', expr)
//            However, using this data structure should be more efficient.
// Author(s)  Conan Reis
class SkBind : public SkExpressionBase
  {
  friend class SkParser;

  public:
  // Common Methods

    SK_NEW_OPERATORS(SkBind);

    SkBind();
    SkBind(SkIdentifier * ident_p, SkExpressionBase * expr_p);
    virtual ~SkBind();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkBind(const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if (SKOOKUM & SK_CODE_IN)
      virtual const SkExpressionBase * find_expr_last_no_side_effect() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual eSkExprType     get_type() const;
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual bool            is_immediate(uint32_t * durational_idx_p = nullptr) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

    // Debugging Methods
    #if (SKOOKUM & SK_DEBUG)
      virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif

  protected:

  // Data Members

    SkIdentifier *     m_ident_p;
    SkExpressionBase * m_expr_p;

  };  // SkBind


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkCode.inl>
#endif


#endif  // __SKCODE_HPP

