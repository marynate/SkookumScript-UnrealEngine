//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Classes for expressions that can be evaluated/invoked
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKEXPRESSIONBASE_HPP
#define __SKEXPRESSIONBASE_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/ASymbol.hpp>
#include <AgogCore/APCompactArray.hpp>
#include <AgogCore/ARefCount.hpp>
#include <SkookumScript/SkDebug.hpp>


//=======================================================================================
// Global Macros / Defines
//=======================================================================================

// Store character position for debugging
#if (SKOOKUM & SK_DEBUG)
  #define SKDEBUG_SET_CHAR_POS(_expr_p, _pos)  (_expr_p)->m_source_idx = ((_pos) < UINT16_MAX) ? uint16_t(_pos) : UINT16_MAX
#else
  #define SKDEBUG_SET_CHAR_POS(_expr_p, _pos)  (void(0))
#endif


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// This is used to differentiate between different types of expressions when it is only
// known that an instance is of type SkExpressionBase, but not the specific subclass.
// It is returned by the method SkExpressionBase::get_type()
//
// *** NOTE *** These values are put directly into the compiled code binary using
// SkExpression::as_binary_typed() - and related methods - as a single byte (giving 255
// possible types) [SkUnaryParam and SkGroupParam actually squeeze it into 6 bits (63
// possible types) when reading/writing to binary - they will have to be modified if the
// number of types increases beyond 63], so changing these values requires changing the
// version number of the compiled code binary SkBrain_bin_code_id_version found in
// SkBrain.cpp.
enum eSkExprType
  {
  SkExprType__default                 =  0,  // Use default argument / receiver / clause (valid only as an argument, a receiver of an invocation, or as a default / else clause)
  SkExprType_identifier               =  1,  // SkIdentifier  temp
  SkExprType_identifier_member        =  2,  // SkIdentifierMember  m_value
  SkExprType_identifier_class_member  =  3,  // SkIdentifierClassMember  ms_random
  SkExprType_object_id                =  4,  // SkObjectId  MyClass@'some_name'
  SkExprType_literal                  =  5,  // SkLiteral (Boolean, Character, Integer, Real, String, Symbol, Class, nil, this, this_class, this_code)
  SkExprType_literal_list             =  6,  // SkLiteralList {elem1, elem2}
  SkExprType_closure_method           =  7,  // SkLiteralClosure(Method)  ^[do_this() do_that()]
  SkExprType_closure_coroutine        =  8,  // SkLiteralClosure(Coroutine)  ^[_do_this() _do_that()]
  SkExprType_bind                     =  9,  // SkBind - variable bind expression.  Could be a member variable, a parameter variable, or a temporary variable.
  SkExprType_cast                     = 10,  // SkCast - expr<>Class
  SkExprType_conversion               = 11,  // SkConversion - expr>>Class
  SkExprType_code                     = 12,  // SkCode - code block (generally nested)
  SkExprType_conditional              = 13,  // SkConditional - if bool1 [clause1] bool2 [clause2] else [else_clause]
  SkExprType_case                     = 14,  // SkCase - case compare test1 [clause1] test2 [clause2] else [else_clause]
  SkExprType_when                     = 15,  // SkWhen - clause when test
  SkExprType_unless                   = 16,  // SkUnless - clause unless test
  SkExprType_loop                     = 17,  // SkLoop - loop [ if loop_test() [exit] loop_code() ]
  SkExprType_loop_exit                = 18,  // SkLoopExit - (valid only in the scope of the loop it references)
  SkExprType_invoke                   = 19,  // SkInvocation - receiver.call()
  SkExprType_invoke_sync              = 20,  // SkInvokeSync - receiver%call()
  SkExprType_invoke_race              = 21,  // SkInvokeRace - receiver%>call()
  SkExprType_invoke_cascade           = 22,  // SkInvokeCascade - receiver :call1() :call2() :call3()
  SkExprType_invoke_closure_method    = 23,  // SkInvokeClosureMethod - closure_expr()
  SkExprType_invoke_closure_coroutine = 24,  // SkInvokeClosureCoroutine - closure_expr()
  SkExprType_instantiate              = 25,  // SkInstantiate - Instantiates / allocates / creates an object and initializes its data members to nil - called just prior to a constructor
  SkExprType_copy_invoke              = 26,  // SkCopyInvoke - Instantiates object via !copy() and calls initial method on it - expr!method() -> ExprType!copy(expr).method() or ExprType!copy(expr).[method() self()]
  SkExprType_concurrent_sync          = 27,  // SkConcurrentSync - convergent concurrent threaded expressions - sync [ _expr1() _expr2() _expr3() ]
  SkExprType_concurrent_race          = 28,  // SkConcurrentRace - concurrent racing expressions - race [ _expr1() _expr2() _expr3() ]
  SkExprType_concurrent_branch        = 29,  // SkConcurrentBranch - branched concurrent expression - branch [ _expr() ]
  SkExprType_divert                   = 30   // SkDivert - diverted expression - divert [ _expr() ]
  };

//---------------------------------------------------------------------------------------
// Indicates whether a given expression (and its sub expressions) has side effects or not.
// [Can be used in the place of bool to give more obvious context.]
enum eSkSideEffect
  {
  // Has no side-effects
  SkSideEffect_none      = 0,  // [implicitly coercible to false]
  // Has primary call, clause, elements, etc. effects
  SkSideEffect_present   = 1,  // [implicitly coercible to true]
  // Has secondary/incidental receiver, test expression, etc. effects though no primary
  // effects.  [Probably a mistake]
  SkSideEffect_secondary = 2 
  };


// Pre-declarations
class SkInstance;
class SkObjectBase;
class SkInvokedBase;
class SkInvokedExpression;
class SkParser;
class SkClassDescBase;
class SkClass;


//---------------------------------------------------------------------------------------
// Abstract base structure for applying some operation / iterating through expression
// structures - used in SkClass, SkExpressionBase, SkInvokableBase, their subclasses and
// any other structures that contain expressions.
// 
// [There may be a more sophisticated methodology for a callback with context though an
// abstract base base class is simple and effective.]
// 
// #See Also
//   SkClass::iterate_expressions(), SkClass::iterate_expressions_recurse(),
//   SkExpressionBase::iterate_expressions()
//   
// #Author(s) Conan Reis
struct SkApplyExpressionBase
  {
  // Called with each expression being iterated over.
  // Return true to stop/abort the iteration and false to continue iterating.
  // expr_p is the expression being iterated over and invokable_p is
  // the method/coroutine where the expression originates or nullptr if that info isn't
  // available.
  virtual eAIterateResult apply_expr(SkExpressionBase * expr_p, const SkInvokableBase * invokable_p) = 0;
  };


//---------------------------------------------------------------------------------------
// Notes      SkookumScript ExpressionBase
// Subclasses See eSkExprType above.
// Author(s)  Conan Reis
class SkExpressionBase
  {
  public:

  // Debug Data Members - public for quick access

    #if (SKOOKUM & SK_DEBUG)

      // Debug:  Character index position where this expression starts in the originally
      // parsed code file/string.  m_debug_info describes the source code origin.
      uint16_t m_source_idx; 

      // Debug: Debug flags & misc. info - such as breakpoint set flag - see SkDebug::eInfo
      uint16_t m_debug_info;

      // Debug: Any time a new expression is constructed, it's 'm_debug_info' will be set
      // to this value.  It can be changed when creating expressions that have varying
      // starting debug information like different origin locations
      // - such as SkDebugInfo::Flag_origin_internal (default), SkDebugInfo::Flag_origin_source, or
      // SkDebugInfo::Flag_origincustom.
      static uint16_t ms_new_expr_debug_info;

    #endif


  // Common Methods

    virtual ~SkExpressionBase();

  // Converter Methods

    virtual SkExpressionBase * as_copy() const;

    #if (SKOOKUM & SK_COMPILED_OUT)

      virtual void     as_binary(void ** binary_pp) const = 0;
      virtual uint32_t as_binary_length() const = 0;
      void             as_binary_typed(void ** binary_pp) const;
      uint32_t         as_binary_typed_length() const;

      static void      as_binary_typed(SkExpressionBase * expr_p, void ** binary_pp);
      static uint32_t  as_binary_typed_length(SkExpressionBase * expr_p);

    #endif // (SKOOKUM & SK_COMPILED_OUT)


    #if (SKOOKUM & SK_COMPILED_IN)

      static SkExpressionBase * from_binary_new(eSkExprType expr_type, const void ** binary_pp);
      static SkExpressionBase * from_binary_typed_new(const void ** binary_pp);

    #endif // (SKOOKUM & SK_COMPILED_IN)


    #if (SKOOKUM & SK_CODE_IN)

      virtual const SkExpressionBase * find_expr_last_no_side_effect() const;
      virtual eSkSideEffect            get_side_effect() const;

    #endif // (SKOOKUM & SK_CODE_IN)


    #if defined(SK_AS_STRINGS)

      virtual AString as_code() const = 0;
      AString         as_code_block() const;

    #endif // defined(SK_AS_STRINGS)


  // Methods

    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const = 0;
    SkInstance *            invoke_now(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr) const;
    void                    invoke_now_proc(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr) const;
    virtual void            null_receiver(SkExpressionBase * receiver_p);
    virtual void            track_memory(AMemoryStats * mem_stats_p) const = 0;

    // Type-checking Methods

      virtual eSkExprType get_type() const = 0;
      virtual bool        is_loop(const ASymbol & loop_name) const  { return false; }
      virtual bool        is_immediate(uint32_t * durational_idx_p = nullptr) const;
      virtual bool        is_nil() const                            { return (this == nullptr); }

    // Called by SkInvokedExpression

      virtual SkMind * get_updater(const SkInvokedExpression & iexpr) const;
      virtual bool     invoke_iterate(SkInvokedExpression * iexpr_p, SkInstance ** result_pp = nullptr) const;
      virtual void     invoke_exit(SkInvokedExpression * iexpr_p, SkInvokedExpression * sub_exit_p = nullptr) const  {}


    #if (SKOOKUM & SK_DEBUG)

      // Debugging Methods

      SkExpressionBase() : m_source_idx(SkExpr_char_pos_invalid), m_debug_info(ms_new_expr_debug_info) {}

      SkExpressionBase *         find_expr_on_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      SkExpressionBase *         find_expr_by_pos_first() const   { return find_expr_by_pos(0u); }
      SkExpressionBase *         find_expr_by_pos_last() const;
      SkExpressionBase *         find_expr_span(uint32_t source_idx, uint32_t * idx_begin_p = nullptr, uint32_t * idx_end_p = nullptr, eSkExprFind type = SkExprFind_all) const;
      void                       get_expr_span(const SkExpressionBase & expr, uint32_t * idx_begin_p = nullptr, uint32_t * idx_end_p = nullptr) const;
      virtual SkDebugInfo        get_debug_info(const SkInvokedExpression & iexpr) const;
      bool                       is_valid_origin_source() const   { return ((m_debug_info & SkDebugInfo::Flag_origin__mask) == SkDebugInfo::Flag_origin_source) && (m_source_idx != SkExpr_char_pos_invalid); }

      virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);

      static void set_new_expr_debug_info(uint16_t debug_info)    { ms_new_expr_debug_info = debug_info; }

    #endif

  };  // SkExpressionBase


//---------------------------------------------------------------------------------------
// Notes      SkookumScript loop expression
// Subclasses 
// See Also   SkLoopExit
// Examples:      loop
//              [
//              ? loop_test() [exit].
//              loop_code()
//              ]
// Author(s)  Conan Reis
class SkLoop : public SkExpressionBase
  {
  friend class SkParser;

  public:

  // Common Methods

    SK_NEW_OPERATORS(SkLoop);

    SkLoop(SkExpressionBase * expr_p = nullptr, const ASymbol & name = ASymbol::get_null() ) : m_name(name), m_expr_p(expr_p) {}
    virtual ~SkLoop();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkLoop(const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual bool is_loop(const ASymbol & loop_name) const  { return loop_name.is_null() || (loop_name == m_name); }

    // Overridden from SkExpressionBase

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
      virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif

  protected:

  // Data Members

    // Optional loop name - so that a nested 'exit' can refer to it explicitly.
    ASymbol m_name;

    // Expression to loop
    SkExpressionBase * m_expr_p;

  // Class Data Members

  };  // SkLoop


//---------------------------------------------------------------------------------------
// Notes      SkookumScript loop exit expression
// Subclasses 
// See Also   
// Examples:      loop
//              [
//              ? loop_test() [exit].
//              loop_code()
//              ]
// Author(s)  Conan Reis
class SkLoopExit : public SkExpressionBase
  {
  friend class SkParser;

  public:

  // Common Methods

    SK_NEW_OPERATORS(SkLoopExit);

    SkLoopExit(const ASymbol & name = ASymbol::get_null()) : m_name(name) {}
    virtual ~SkLoopExit();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkLoopExit(const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    // Overridden from SkExpressionBase

    virtual eSkExprType     get_type() const;
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

  protected:

  // Data Members

    // Optional loop name - so that a nested 'exit' can refer to it explicitly.
    ASymbol m_name;

  };  // SkLoopExit


//---------------------------------------------------------------------------------------
// Notes      SkookumScript Conversion to Class Type Primitive Expression
// See Also   SkCast
// Examples:      expr>>Class
// Author(s)  Conan Reis
class SkConversion : public SkExpressionBase
  {
  friend class SkParser;

  public:

  // Common Methods

    SK_NEW_OPERATORS(SkConversion);

    SkConversion(SkClass * type_p, SkExpressionBase * expr_p);
    virtual ~SkConversion();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkConversion(const void ** binary_pp);
      void assign_binary(const void ** binary_pp);
    #endif

    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif

    #if (SKOOKUM & SK_CODE_IN)
      virtual const SkExpressionBase * find_expr_last_no_side_effect() const  { return this; }
      virtual eSkSideEffect            get_side_effect() const;
    #endif

    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    // Overridden from SkExpressionBase

      virtual eSkExprType     get_type() const;
      virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
      virtual void            null_receiver(SkExpressionBase * receiver_p);
      virtual void            track_memory(AMemoryStats * mem_stats_p) const;

      // Debugging Methods
      #if (SKOOKUM & SK_DEBUG)
        virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
        virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
      #endif

  protected:

  // Data Members

    // Desired class type
    SkClass * m_type_p;

    // Expression to convert
    SkExpressionBase * m_expr_p;

  };  // SkConversion


//---------------------------------------------------------------------------------------
// Notes      SkookumScript Class Cast Primitive Expression
// See Also   SkCast
// Examples:      expr<>Class
// Author(s)  Conan Reis
class SkCast : public SkExpressionBase
  {
  friend class SkParser;

  public:

  // Common Methods

    SK_NEW_OPERATORS(SkCast);

    SkCast(SkClassDescBase * type_p, SkExpressionBase * expr_p);
    virtual ~SkCast();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkCast(const void ** binary_pp);
      void assign_binary(const void ** binary_pp);
    #endif

    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif

    #if (SKOOKUM & SK_CODE_IN)
      virtual const SkExpressionBase * find_expr_last_no_side_effect() const  { return this; }
      virtual eSkSideEffect            get_side_effect() const;
    #endif

    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif

  // Methods

    // Overridden from SkExpressionBase

      virtual eSkExprType     get_type() const;
      virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
      virtual void            null_receiver(SkExpressionBase * receiver_p);
      virtual void            track_memory(AMemoryStats * mem_stats_p) const;

      // Debugging Methods
      #if (SKOOKUM & SK_DEBUG)
        virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
        virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
      #endif

  protected:

  // Data Members

    // Desired class type
    // $Revisit - CReis In theory this hint should not be needed during run-time (just
    // m_expr_p would be needed) if not debugging or parsing - i.e. if only SK_COMPILED_IN
    // is defined.  Currently only used if SK_CODE_IN, SK_CODE_OUT or SK_COMPILED_OUT is
    // defined.]
    ARefPtr<SkClassDescBase> m_type_p;

    // Expression to convert
    SkExpressionBase * m_expr_p;

  };  // SkCast


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkExpressionBase.inl>
#endif


#endif  // __SKEXPRESSIONBASE_HPP

