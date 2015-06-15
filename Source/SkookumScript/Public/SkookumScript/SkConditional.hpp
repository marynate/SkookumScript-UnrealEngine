//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Classes for expressions that can be conditionally evaluated/invoked
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKCONDITIONAL_HPP
#define __SKCONDITIONAL_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkExpressionBase.hpp>


//=======================================================================================
// Global Macros / Defines
//=======================================================================================


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// This is used by both SkConditional and SkCase
struct SkClause
  {
  // Methods

	SK_NEW_OPERATORS(SkClause);

    SkClause(SkExpressionBase * test_p = nullptr, SkExpressionBase * clause_p = nullptr) : m_test_p(test_p), m_clause_p(clause_p) {}
    SkClause(SkClause * clause_p) : m_test_p(clause_p->m_test_p), m_clause_p(clause_p->m_clause_p) { clause_p->m_test_p = nullptr; clause_p->m_clause_p = nullptr; }
    ~SkClause()   { delete m_test_p; delete m_clause_p; }

    virtual void track_memory(AMemoryStats * mem_stats_p) const;

    #if (SKOOKUM & SK_COMPILED_IN)

      SkClause(const void ** binary_pp)
        { m_test_p = SkExpressionBase::from_binary_typed_new(binary_pp); m_clause_p = SkExpressionBase::from_binary_typed_new(binary_pp); }

    #endif // (SKOOKUM & SK_COMPILED_IN)


    #if (SKOOKUM & SK_COMPILED_OUT)

      void as_binary(void ** binary_pp) const
        { SkExpressionBase::as_binary_typed(m_test_p, binary_pp); m_clause_p->as_binary_typed(binary_pp); }

      uint32_t as_binary_length() const
        { return SkExpressionBase::as_binary_typed_length(m_test_p) + m_clause_p->as_binary_typed_length(); }

    #endif // (SKOOKUM & SK_COMPILED_OUT)


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


    // Debugging Methods
    #if (SKOOKUM & SK_DEBUG)
      SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif


  // Data Members

    // Expression to test.  If it is nullptr it acts an 'else' clause which always succeeds.
    // When used by SkConditional, it should evaluate to a Boolean (true or false).
    // When used by SkCase, it should evaluate as a valid argument to be passed to the
    // equals operator (=) compared against the comparison expression.
    // It must return immediately when invoked and thus may not contain any calls to
    // coroutines.
    SkExpressionBase * m_test_p;

    // Expression to evaluate if the test succeeds - the "then" of the clause.
    SkExpressionBase * m_clause_p;

  };  // SkClause


//---------------------------------------------------------------------------------------
// Notes      SkookumScript "if" (conditional) expression
// See Also   SkCase, SkWhen, SkUnless
// Examples:      if test_expr1 [clause1]
//              test_expr2  [clause2]
//              else        [else_clause]
// Author(s)  Conan Reis
class SkConditional : public SkExpressionBase
  {
  friend class SkParser;

  public:

  // Common Methods

    SK_NEW_OPERATORS(SkConditional);

    SkConditional() {}
    virtual ~SkConditional();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)

      SkConditional(const void ** binary_pp);
      void assign_binary(const void ** binary_pp);

    #endif // (SKOOKUM & SK_COMPILED_IN)


    #if (SKOOKUM & SK_COMPILED_OUT)

      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;

    #endif // (SKOOKUM & SK_COMPILED_OUT)


    #if (SKOOKUM & SK_CODE_IN)
      virtual const SkExpressionBase * find_expr_last_no_side_effect() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    // Overridden from SkExpressionBase

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

    // If there is more than one clause, the last clause is a default/else clause if its
    // test expression is nullptr
    APCompactArrayFree<SkClause> m_clauses;

  };  // SkConditional


//---------------------------------------------------------------------------------------
// Notes      SkookumScript case (switch) expression
// See Also   SkConditional, SkWhen, SkUnless
// Examples:      case compare_expr
//              test_expr1 [clause1]
//              test_expr2 [clause2]
//              else       [else_clause]
// Author(s)  Conan Reis
class SkCase : public SkExpressionBase
  {
  friend class SkParser;

  public:

  // Common Methods

    SK_NEW_OPERATORS(SkCase);

    SkCase() : m_compare_expr_p(nullptr) {}
    virtual ~SkCase();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)

      SkCase(const void ** binary_pp);
      void assign_binary(const void ** binary_pp);

    #endif // (SKOOKUM & SK_COMPILED_IN)


    #if (SKOOKUM & SK_COMPILED_OUT)

      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;

    #endif // (SKOOKUM & SK_COMPILED_OUT)


    #if (SKOOKUM & SK_CODE_IN)
      virtual const SkExpressionBase * find_expr_last_no_side_effect() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    // Overridden from SkExpressionBase

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

    // Expression to compare the clauses' test expression against
    // Note - this pointer should never be nullptr (representing an implied this) since it
    // would be parsed as a SkConditional if this were implied.
    SkExpressionBase * m_compare_expr_p;

    // If there is more than one clause, the last clause is a default/else clause if its
    // test expression is nullptr
    APCompactArrayFree<SkClause> m_clauses;

  // Class Data Members

  };  // SkCase


//---------------------------------------------------------------------------------------
// SkookumScript "when" (conditional) statement modifier expression - the clause
// expression is only evaluated if the test expression evaluates to true.
// 
// Similar to if expression though *modifies* an earlier statement, has only one
// test/clause and does not need a code block surrounding either the test or the clause.
// 
// #Examples
//   clause when test_expr
//   
// #Notes
//   "if" is not used since the parser would not be able to determine if it was two
//   expressions or a single *modified* expression.
// 
//   Could be written in terms of SkConditional though this is simpler, takes up less
//   memory and is slightly more efficient.
//   
// #See Also   SkUnless, SkConditional, SkCase
// #Author(s)  Conan Reis
class SkWhen : public SkExpressionBase
  {
  friend class SkParser;

  public:

  // Common Methods

    SK_NEW_OPERATORS(SkWhen);

    SkWhen(SkExpressionBase * test_p, SkExpressionBase * clause_p)  : m_test_p(test_p), m_clause_p(clause_p) {}
    virtual ~SkWhen()                                               { delete m_test_p; delete m_clause_p; }

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)

      SkWhen(const void ** binary_pp);
      void assign_binary(const void ** binary_pp);

    #endif // (SKOOKUM & SK_COMPILED_IN)


    #if (SKOOKUM & SK_COMPILED_OUT)

      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;

    #endif // (SKOOKUM & SK_COMPILED_OUT)


    #if (SKOOKUM & SK_CODE_IN)
      virtual const SkExpressionBase * find_expr_last_no_side_effect() const;
      virtual eSkSideEffect            get_side_effect() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    // Overridden from SkExpressionBase

    virtual eSkExprType     get_type() const                { return SkExprType_when; }
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

    // Expression to test - it should evaluate to a Boolean (true or false).
    // It must be an immediate (non-durational) call when invoked.
    SkExpressionBase * m_test_p;

    // Expression to evaluate if the test succeeds - the "then".
    SkExpressionBase * m_clause_p;

  };  // SkWhen


//---------------------------------------------------------------------------------------
// SkookumScript "unless" (negated conditional) statement modifier expression - the clause
// expression is only evaluated if the test expression evaluates to false.
// 
// Same as when expression though test is negated.  Similar to if expression though test
// negated and *modifies* an earlier statement, has only one test/clause and does not need
// a code block surrounding either the test or the clause.
// 
// #Examples
//   clause unless test_expr
//   
// #See Also   SkWhen, SkConditional, SkCase
// #Author(s)  Conan Reis
class SkUnless : public SkWhen
  {
  friend class SkParser;

  public:

  // Common Methods

    SK_NEW_OPERATORS(SkUnless);

    SkUnless(SkExpressionBase * test_p, SkExpressionBase * clause_p)  : SkWhen(test_p, clause_p) {}
    virtual ~SkUnless()                                       {}

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)

      SkUnless(const void ** binary_pp)  : SkWhen(binary_pp)  {}

    #endif // (SKOOKUM & SK_COMPILED_IN)


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    // Overridden from SkExpressionBase

    virtual eSkExprType     get_type() const                { return SkExprType_unless; }
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

  };  // SkUnless


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkConditional.inl>
#endif


#endif  // __SKCONDITIONAL_HPP

