//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Invocation classes: SkInvokeBase, SkInvocation, SkInvokeSync, SkInvokeRace,
// SkInvokeCascade, SkInstantiate and SkCopyInvoke
// 
// Author(s):   Conan Reis
//=======================================================================================


#ifndef __SKINVOCATION_HPP
#define __SKINVOCATION_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkExpressionBase.hpp>
#include <SkookumScript/SkQualifier.hpp>
#include <AgogCore/APCompactArray.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declarations
class SkMethodCall;
class SkIdentifier;

//---------------------------------------------------------------------------------------
// Notes      SkookumScript invocation / call selector / descriptor
// Subclasses SkMethodCall, SkCoroutineCall
// Author(s)  Conan Reis
class SkInvokeBase : public SkQualifier

  {
  friend class SkParser;

  public:
	  SK_NEW_OPERATORS(SkInvokeBase);
  // Common Methods
    SkInvokeBase(const ASymbol & call_name = ASymbol::get_null(), SkClass * scope_p = nullptr);
    SkInvokeBase(SkInvokeBase * call_p);
    virtual ~SkInvokeBase();

  // Converter Methods

    // Binary

    #if (SKOOKUM & SK_COMPILED_OUT)

      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
      void             as_binary_typed(void ** binary_pp) const;
      uint32_t         as_binary_typed_length() const;
      //void             assign_binary(const void ** binary_pp);

    #endif // (SKOOKUM & SK_COMPILED_OUT)


    #if (SKOOKUM & SK_COMPILED_IN)
      void                  assign_binary(const void ** binary_pp);
      static SkInvokeBase * from_binary_typed_new(const void ** binary_pp);
    #endif


    // AString Methods
    #if defined(SK_AS_STRINGS)

      virtual AString as_code() const;
      AString         as_string_name() const;

    #endif


    // Debugging Methods
    #if (SKOOKUM & SK_DEBUG)
      SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif


  // Methods

    virtual eSkInvokeType                    get_invoke_type() const = 0;
    const APCompactArray<SkExpressionBase> & get_args() const           { return m_arguments; }
    const APCompactArray<SkIdentifier> &     get_return_args() const    { return m_return_args; }
    virtual SkInvokedBase *                  invoke_call(SkInstance * receiver_p, SkObjectBase * scope_p, SkInvokedBase * caller_p, SkInstance ** result_pp) const = 0;
    virtual void                             track_memory(AMemoryStats * mem_stats_p) const = 0;

  protected:
    
  // Data Members

    // $Note - CReis Future: An index into the invokable (method, coroutine) table
    // in this call's invoker's class.  This would replace the 'm_name' data member.
    //uint32_t m_call_index;

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
    // to have this class without them and a separate SkInvokeRArgsBase with them.
    APCompactArray<SkIdentifier> m_return_args;
  };  // SkInvokeBase


//---------------------------------------------------------------------------------------
// Notes      SkookumScript Invocation.
//            This class is similar to the SkInvokeCascade class except that it has
//            only one 'command' to give to the receiver and similar to SkInstantiate
//            except that its receiver can be any expression and not just a new instance.
// See Also   SkInvokeSync, SkInvokeRace, SkInvokeCascade, SkInstantiate
// Examples:      receiver.call()
// Author(s)  Conan Reis
class SkInvocation : public SkExpressionBase
  {
  friend class SkParser;

  public:
  // Common Methods

    SK_NEW_OPERATORS(SkInvocation);

    SkInvocation(SkInvokeBase * call_p, SkExpressionBase * receiver_p = nullptr);
    virtual ~SkInvocation();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkInvocation(const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual eSkExprType     get_type() const      { return SkExprType_invoke; }
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual bool            is_immediate(uint32_t * durational_idx_p = nullptr) const;
    virtual void            null_receiver(SkExpressionBase * receiver_p);
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

  // Debugging Methods

    #if (SKOOKUM & SK_DEBUG)
      virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif

  protected:
    
  // Data Members

    // The object that results from the evaluation of this expression is the
    // target/subject/receiver of subroutine 'm_call_p'.  If 'm_receiver_p' is nullptr
    // then 'this' - i.e. the topmost scope - is inferred.
    SkExpressionBase * m_receiver_p;

    // Subroutine to call - SkCoroutineCall or SkMethodCall
    SkInvokeBase * m_call_p;

  };  // SkInvocation


//---------------------------------------------------------------------------------------
// Notes      SkookumScript Apply Invocation.
//            This class is similar to the SkInvocation class except that the command is
//            applied only to valid receiver objects based on a runtime test of the
//            receiver class type.
//
//            There are 3 receiver categories:
//              nil (class None)
//                - the invocation is ignored
//              list object (class List)
//                - the invocation is applied to each element concurrently (for durational
//                  calls).  If the list is empty the invocation is ignored.
//              some other object type (anything other than None and List)
//                - the invocation is called normally
// See Also   SkInvocation, SkInvokeRace, SkInvokeCascade, SkInstantiate
// Examples:      receiver%call()
//            receiver_or_nil%call()
//            list%call()
//            {1, 2, 3}%add_assign(1)  // becomes {2, 3, 4}
// Author(s)  Conan Reis
class SkInvokeSync : public SkInvocation
  {
  friend class SkParser;

  public:

  // Common Methods

	SK_NEW_OPERATORS(SkInvokeSync);

    SkInvokeSync(SkInvokeBase * call_p, SkExpressionBase * receiver_p = nullptr) : SkInvocation(call_p, receiver_p) {}
    virtual ~SkInvokeSync()  {}

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkInvokeSync(const void ** binary_pp) : SkInvocation(binary_pp) {}
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual eSkExprType     get_type() const      { return SkExprType_invoke_sync; }
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

  };  // SkInvokeSync


//---------------------------------------------------------------------------------------
// Notes      SkookumScript Race Invocation.
//            This class is similar to the SkInvocation class except that the command is
//            applied only to valid receiver objects based on a runtime test of the
//            receiver class type.
//
//            There are 3 receiver categories:
//              nil (class None)
//                - the invocation is ignored
//              list object (class List)
//                - the invocation is applied to each element concurrently (for durational
//                  calls).  If the list is empty the invocation is ignored.
//              some other object type (anything other than None and List)
//                - the invocation is called normally
//
//            This class is very similar to SkConcurrentRace and changes to this class might
//            need to be reflected there and vice-versa.
// See Also   SkInvocation, SkInvokeCascade, SkInstantiate, SkConcurrentRace
// Examples:      receiver%>call()
//            receiver_or_nil%>call()
//            list%>call()
//            {1, 2, 3}%>add_assign(1)               // becomes {2, 3, 4}
//            {guy1, guy2, guy3}%>_path_to_pos(pos)  // After first guy gets to pos other guys stop
// Author(s)  Conan Reis
class SkInvokeRace : public SkInvocation
  {
  friend class SkParser;

  public:

  // Common Methods

	SK_NEW_OPERATORS(SkInvokeRace);

    SkInvokeRace(SkInvokeBase * call_p, SkExpressionBase * receiver_p = nullptr) : SkInvocation(call_p, receiver_p) {}
    virtual ~SkInvokeRace()  {}

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkInvokeRace(const void ** binary_pp) : SkInvocation(binary_pp) {}
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual eSkExprType     get_type() const      { return SkExprType_invoke_race; }
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual bool            invoke_iterate(SkInvokedExpression * iexpr_p, SkInstance ** result_pp = nullptr) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

  };  // SkInvokeRace


//---------------------------------------------------------------------------------------
// Notes      SkookumScript invocation cascade - i.e. different subroutines called
//            serially (one after another) that act upon the same receiver.
//            This class is similar to the SkInvocation class except that it has more
//            than one 'command' to give to the receiver.
// See Also   SkInvocation, SkInvokeSync, SkInvokeRace, SkInstantiate
// Examples:      receiver
//              :call1()
//              :call2()
//              :call3()
// Author(s)  Conan Reis
class SkInvokeCascade : public SkExpressionBase
  {
  friend class SkParser;
  friend class SkInvokedExpression;

  public:
  // Common Methods

    SK_NEW_OPERATORS(SkInvokeCascade);

    SkInvokeCascade(SkExpressionBase * receiver_p = nullptr) : m_receiver_p(receiver_p) {}
    virtual ~SkInvokeCascade();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkInvokeCascade(const void ** binary_pp);
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
    virtual bool            is_immediate(uint32_t * durational_idx_p = nullptr) const;
    virtual void            null_receiver(SkExpressionBase * receiver_p);
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

  // Class Methods

    static void clean_up(SkInvokedExpression * iexpr_p);
    
  // Data Members

    // The object that results from the evaluation of this expression is the target for
    // subroutines 'm_invoke_calls'.  If 'm_receiver_p' is nullptr then 'this' - i.e. the topmost
    // scope - is inferred.
    SkExpressionBase * m_receiver_p;

    // Subroutines to call - SkCoroutineCall or SkMethodCall
    APArrayFree<SkInvokeBase> m_invoke_calls;

  };  // SkInvokeCascade


//---------------------------------------------------------------------------------------
// SkookumScript Instantiation expression.
// Instantiates / allocates / creates an object (instance of a class) initializes its
// data members to nil and calls its constructor.
// 
// #Examples
//   Class!()
// 
// #See Also   SkCopyInvoke, SkInvocation, SkInvokeSync, SkInvokeRace, SkInvokeCascade
// #Author(s)  Conan Reis
class SkInstantiate : public SkExpressionBase
  {
  friend class SkParser;

  public:
  // Common Methods

    SK_NEW_OPERATORS(SkInstantiate);

    SkInstantiate(SkClass * class_p, SkMethodCall * ctor_p) : m_class_p(class_p), m_ctor_p(ctor_p) {}
    virtual ~SkInstantiate();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkInstantiate(const void ** binary_pp);
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
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

    // Debugging Methods

    #if (SKOOKUM & SK_DEBUG)
      virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif

  protected:

  // Data Members

    // Instance of class to create
    SkClass * m_class_p;

    // Constructor method call
    SkMethodCall * m_ctor_p;

  };  // SkInstantiate


//---------------------------------------------------------------------------------------
// SkookumScript Copy Instantiation & method call expression.
// Instantiates object via !copy() and calls initial method on it.
// 
// * Could derive from SkInstantiate though every method would need to be rewritten anyway.
// 
// #Examples
//   expr!method()
//   
//   // Equivalent to:
//     ExprType!copy(expr).method()
//     ExprType!copy(expr).[method() self()]
// 
// #See Also   SkInstantiation, SkInvocation, SkInvokeSync, SkInvokeRace, SkInvokeCascade
// #Author(s)  Conan Reis
class SkCopyInvoke : public SkExpressionBase
  {
  friend class SkParser;

  public:
  // Common Methods

    SK_NEW_OPERATORS(SkCopyInvoke);

    SkCopyInvoke(SkClass * class_p, SkMethodCall * ctor_p, SkMethodCall * method_p) :
      m_class_p(class_p), m_ctor_p(ctor_p), m_method_p(method_p) {}
    virtual ~SkCopyInvoke();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkCopyInvoke(const void ** binary_pp);
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
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

    // Debugging Methods

    #if (SKOOKUM & SK_DEBUG)
      virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif

  protected:

  // Data Members

    // Instance of class to create
    SkClass * m_class_p;

    // Constructor method call
    SkMethodCall * m_ctor_p;

    // Initial method call
    SkMethodCall * m_method_p;

  };  // SkInstantiate


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkInvocation.inl>
#endif


#endif  // __SKINVOCATION_HPP

