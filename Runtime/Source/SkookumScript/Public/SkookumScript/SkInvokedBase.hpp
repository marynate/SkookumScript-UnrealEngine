//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Base classes for executed/called/invoked objects declaration file
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKINVOKEDBASE_HPP
#define __SKINVOKEDBASE_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <new>  // For placement new
#include <AgogCore/APArray.hpp>
#include <AgogCore/AList.hpp>
#include <SkookumScript/SkInstance.hpp>
#include <SkookumScript/SkDebug.hpp>
#include <SkookumScript/SkSymbolDefs.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declaration
class SkClass;
class SkExpressionBase;
class SkIdentifier;
class SkInvokableBase;
class SkInvokeBase;
class SkInvokedMethod;
class SkList;
enum eSkMember;

class SkInvokedCoroutine;  // For circular reference


// Used when specifying an argument by index - a simple integer could be used, but these
// give a bit more context
enum eSkArgNum
  {
  SkArg_1 = 0,
  SkArg_2,
  SkArg_3,
  SkArg_4,
  SkArg_5,
  SkArg_6,
  SkArg_7,
  SkArg_8,
  SkArg_9,
  SkArg_10,
  SkArg_11,
  SkArg_12,
  SkArg_13,
  SkArg_14,
  SkArg_15,
  SkArg_16,
  SkArg_17,
  SkArg_18,
  SkArg_19,
  SkArg_20,
  };

// Used with abort commands
enum eSkNotify
  {
  SkNotify_ignore   = 0x0,  // Caller is not notified
  SkNotify_fail     = 0x1,  // Caller notified that this sub-expression did not successfully complete
  SkNotify_success  = 0x3   // Caller notified that this sub-expression successfully completed
  };

// Used with abort commands - also see eSkNotify 
enum eSkNotifyChild
  {
  SkNotifyChild_detach  = 0x0,  // Child/sub calls ignore the fact that their caller is being aborted
  SkNotifyChild_abort   = 0x1   // Abort child/sub calls
  };


//---------------------------------------------------------------------------------------
// Notes      Invoked Object Abstract Base Class
// Subclasses SkInvokedExpression, SkInvokedContextBase
// Author(s)  Conan Reis
class SkInvokedBase : public SkObjectBase, public AListNode<SkInvokedBase>
  {
  public:

  // Public Data Members

    // Path for data look-ups (the owner)
    AIdPtr<SkObjectBase> m_scope_p;

    // Object that invoked / called this invokable and expects to be returned to on the
    // completion of this invokable.  Essentially the call-stack *parent* of this object.
    // nullptr if there is nothing to return to.
    // $Revisit - CReis Now that invoked objects have a list of sub-calls, each sub-called
    // could be notified if its caller went out of scope and thus set this to nullptr - in
    // which case this would not need to be a smart pointer.  Though SkConcurrentBranch
    // might complicate things.
    AIdPtr<SkInvokedBase> m_caller_p;
    
    // Invoked objects that this invoked object has called/created.  Essentially the
    // call-stack *children* of this object.
    // [Needed for SkConcurrentRace, abort_subcalls() & detach_subcalls().]
    AList<SkInvokedBase> m_calls;


  // Common Methods

    SkInvokedBase(SkInvokedBase * caller_p = nullptr, SkObjectBase * scope_p = nullptr);
    ~SkInvokedBase();


  // Methods

    SkInvokedBase * get_caller() const                    { return m_caller_p; }
    SkObjectBase *  get_scope() const                     { return m_scope_p; }
    void            set_caller(SkInvokedBase * caller_p)  { m_caller_p = caller_p; }

    virtual SkInvokedContextBase *   get_caller_context() const;
    virtual const SkExpressionBase * get_expr() const     { return nullptr; }
    virtual SkMind *                 get_updater() const;
    uint32_t                         get_context_depth() const;
    bool                             is_caller(SkInvokedBase * caller_p) const;

    #if (SKOOKUM & SK_DEBUG)

      virtual SkExpressionBase * get_caller_expr() const = 0;
      virtual SkDebugInfo        get_debug_info() const = 0;
      virtual bool               is_in_use() const        { return is_valid_id(); }

      AString list_callstack_debug(uint32_t stack_flags = SkInvokeInfo__callstack_def) const;

    #endif

    // Pending Methods

      virtual void abort_invoke(eSkNotify notify_caller = SkNotify_fail, eSkNotifyChild notify_child = SkNotifyChild_abort) = 0;
      void         abort_subcalls(eSkNotify notify_caller = SkNotify_success);
      void         detach(eSkNotify notify_caller = SkNotify_success);
      void         detach_subcalls(eSkNotify notify_caller = SkNotify_success);
      uint32_t     pending_count() const                  { return m_pending_count; }
      void         pending_increment()                    { m_pending_count++; }
      void         pending_increment(uint32_t increment)      { m_pending_count += increment; }
      void         pending_set(uint32_t pending)              { m_pending_count = pending; }
      void         pending_clear()                        { m_pending_count = 0u; }  // Used by concurrent branch
      void         pending_deferred(SkInvokedBase * deferred_p);
      void         pending_unregister(SkInvokedBase * pending_p, bool completed = true);
      virtual void pending_return(bool completed = true) = 0;
    
    // Overriding from SkObjectBase

      virtual SkInstance *           find_data_by_name(const ASymbol & name) const override;
      virtual SkInstance *           get_data_by_name(const ASymbol & name) const;
      virtual uint32_t               get_obj_type() const { return SkObjectType_invoked_obj; } 
      virtual SkInvokedContextBase * get_scope_context() const;
      virtual SkInstance *           get_topmost_scope() const;
      virtual void                   set_data_by_name(const ASymbol & name, SkInstance * obj_p);

    // SkookumScript Atomic Methods

      static void register_bindings();
    
      static void mthd_abort(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_abort_subcalls(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_detach(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_detach_subcalls(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_data_by_name(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_op_equals(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_op_not_equal(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_pending_increment(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_pending_count(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_pending_decrement(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_data_by_name_set(SkInvokedMethod * scope_p, SkInstance ** result_pp);
      static void mthd_validQ(SkInvokedMethod * scope_p, SkInstance ** result_pp);

  protected:

    // Internal Methods

      void abort_common(eSkNotify notify_caller, eSkNotifyChild notify_child);

  // Data Members

    // Number of expressions / tasks currently executing concurrently that the invoked
    // object is waiting for a return value from.
    uint32_t m_pending_count;

  };  // SkInvokedBase


//---------------------------------------------------------------------------------------
// Notes      Invoked Object to wrap around expressions that may not evaluate immediately
//            - i.e. they take more than one frame to complete their invocation.  This
//            currently includes the expressions SkCode, SkLoop, SkConcurrentRace and
//            SkInvokeCascade.
// Author(s)  Conan Reis
class SkInvokedExpression : public SkInvokedBase
  {
  public:

  // Public Data

    // Index position in multi-part expression
    uint32_t m_index;

    // Optional extra data - when wrapped around a SkInvokeCascade object, it is the
    // receiver instance object (SkInstance *).
    uintptr_t m_data;

  // Common Methods

    SK_NEW_OPERATORS(SkInvokedExpression);

    SkInvokedExpression(const SkExpressionBase & expr, SkInvokedBase * caller_p = nullptr, SkObjectBase * scope_p = nullptr) : SkInvokedBase(caller_p, scope_p), m_index(0u), m_expr_p(&expr) {}

  // Methods

    // Overriding from SkInvokedBase

    virtual void abort_invoke(eSkNotify notify_caller = SkNotify_fail, eSkNotifyChild notify_child = SkNotifyChild_abort);
    virtual void pending_return(bool completed = true);
    
    virtual const SkExpressionBase * get_expr() const       { return m_expr_p; }
    virtual SkMind *                 get_updater() const override;

    #if (SKOOKUM & SK_DEBUG)

      virtual SkExpressionBase * get_caller_expr() const;
      virtual SkDebugInfo        get_debug_info() const;

    #endif

  // Class Methods

    static void                  pool_delete(SkInvokedExpression * iexpr_p);
    static SkInvokedExpression * pool_new(const SkExpressionBase & expr, SkInvokedBase * caller_p, SkObjectBase * scope_p);
    static AObjReusePool<SkInvokedExpression> & get_pool();

  protected:

  // Internal Methods

    friend class AObjBlock<SkInvokedExpression>;  // So constructor can only be accessed by pool_new()

    SkInvokedExpression() {}  // Intentionally uninitialized for pool

  // Data Members

    // The expression that is being invoked
    const SkExpressionBase * m_expr_p;

  };  // SkInvokedExpression


//---------------------------------------------------------------------------------------
// Notes      Invoked Object with Context (temporary variables) Abstract Base Class
//            The 'm_scope_p' member will always be an object derived from SkInstanceBase
// Subclasses SkInvokedMethod, SkInvokedCoroutine
// Author(s)  Conan Reis
class SkInvokedContextBase : public SkInvokedBase
  {
  public:

    // Public Data

    #if (SKOOKUM & SK_DEBUG)

      // Copied from the expression that invoked this method/coroutine - see SkExpressionBase.

      // Debug:  Source character index position where this expression starts in the 
      // originally parsed code file/string.  m_debug_info describes the source code origin.
      uint16_t m_source_idx; 

      // Debug: Debug flags & misc. info - such as breakpoint set flag - see SkDebug::eInfo
      uint16_t m_debug_info;

    #endif

    #if defined(SKDEBUG_HOOKS)
      // Last expression about to invoke a method/coroutine - passed globally (yuck!) to
      // avoid different parameters for debug/non-debug.
      static const SkExpressionBase * ms_last_expr_p;
    #endif


  // Common Methods

    
    SkInvokedContextBase(SkInvokedBase * caller_p = nullptr, SkObjectBase * scope_p = nullptr) : SkInvokedBase(caller_p, scope_p) {}

  // Methods

    SkInstance *              get_this();
    virtual eSkMember         get_invoke_type() const = 0;
    virtual SkInvokableBase * get_invokable() const = 0;

    // !!DEPRECATED!!
    template <typename _UserType>
      _UserType * this_as_data() const;

    template <class _BindingClass>
      typename _BindingClass::tDataType & this_as() { return get_this()->as<_BindingClass>(); }

    #if (SKOOKUM & SK_DEBUG)
      virtual SkExpressionBase * get_caller_expr() const;
      virtual SkDebugInfo        get_debug_info() const;
    #endif

    #if defined(SK_AS_STRINGS)
      AString         as_invoke_string(uint32_t flags = SkInvokeInfo_scope) const;
      virtual AString as_string_debug() const = 0;
    #endif


    // Data Context Methods (for manipulating the 'temporary variable stack')

      // See eSkArgNum earlier in the file

      SkInstance *   get_arg(uint32_t pos) const                           { return m_data.get_at(pos)->m_data_p; }
      SkInstance *   get_arg_by_name(const ASymbol & name) const           { return m_data.get(name)->m_data_p; }

      // !!DEPRECATED!!
      template <typename _UserType>
      _UserType *    get_arg_data(uint32_t pos) const;

      template <class _BindingClass>
        typename _BindingClass::tDataType & get_arg(uint32_t pos) const { return m_data.get_at(pos)->m_data_p->as<_BindingClass>(); }

      template <class _BindingClass>
        void set_arg_(uint32_t pos, const typename _BindingClass::tDataType & value) const { m_data.get_at(pos)->m_data_p->as<_BindingClass>() = value; }

      void           set_arg(uint32_t pos, SkInstance * obj_p);
      void           set_arg_and_ref(uint32_t pos, SkInstance * obj_p);

      const tSkDataIdxTable & get_data() const                             { return m_data; }

      template <typename _UserType, typename... _ParamClasses>
        _UserType * append_user_data(_ParamClasses&... args);

      template <typename _UserType>
        _UserType * get_user_data() const;
    
      void data_append_args_exprs(const APArrayBase<SkExpressionBase> & args, const SkInvokableBase & invokable_params, SkObjectBase * arg_scope_p);
      void data_append_args(SkInstance ** arguments_pp, uint32_t arg_count, const SkInvokableBase & invokable_params);
      void data_append_arg(const ASymbol & param_name, SkInstance * arg_p) { m_data.append(*SkData::pool_new(param_name, arg_p)); }
      void data_append_vars(const AVCompactArrayBase<ASymbol> & var_names);
      void data_ensure_size(uint32_t arg_count) { m_data.ensure_size(arg_count); }
      void data_bind_return_args(const APArrayBase<SkIdentifier> & return_args, const SkInvokableBase & invokable_params);
      void data_remove_vars(const AVCompactArrayBase<ASymbol> & var_names, SkInstance * delay_collect_p);
      void data_remove_vars(uint32_t count, SkInstance * delay_collect_p);
      void data_append_var(const ASymbol & var_name);
      void data_empty()                                                    { SkData::empty_table(&m_data); }
      void data_empty_compact()                                            { SkData::empty_table(&m_data); m_data.empty_compact(); }
      void data_compact()                                                  { m_data.compact(); }

    // Overriding from SkObjectBase -> SkInvokedBase

      virtual SkInvokedContextBase * get_caller_context() const;
      virtual SkInvokedContextBase * get_scope_context() const;
      virtual SkInstance *           find_data_by_name(const ASymbol & name) const override;
      virtual SkInstance *           get_data_by_name(const ASymbol & name) const;
      virtual void                   set_data_by_name(const ASymbol & name, SkInstance * obj_p);

  protected:

  // Types

    template <typename _UserType, typename... _ParamClasses>
    class SkAutoDestroyInstance : public SkInstance
      {
      public:

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        SkAutoDestroyInstance(_ParamClasses... args) : SkInstance(SkBrain::ms_object_class_p)
          {
          //A_DPRINT("SkAutoDestroyInstance::ctor() - called.\n");
          m_ref_count = 1u;
          // Call _UserType constructor on the user data area of this instance
          A_ASSERTX(as_data<_UserType>() == reinterpret_cast<_UserType *>(&m_user_data), "This only works for types that are stored by value.");
          new (as_data<_UserType>()) _UserType(args...);
          }

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        virtual void on_no_references() override
          {
          //A_DPRINT("SkAutoDestroyInstance::on_no_references() - called.\n");
          // Call destructor for _UserType
          as_data<_UserType>()->~_UserType();
          // Change virtual table for this instance back to SkInstance
          new (this) SkInstance();
          // Put it back on the pool for reuse
          delete_this();
          }
      };

  // Data Members

    // Array of arguments and temporary variables - accessed directly by index or found
    // by symbol name.
    // $Revisit - CReis [Index Look-up] To save data wrangling when more code accesses data
    // by index, the instances could be stored directly without a name.  If a named
    // look-up is needed then the parameter descriptor could be referenced.
    tSkDataIdxTable m_data;

  };  // SkInvokedContextBase

//=======================================================================================
// Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Storage specialization - SkInvokedBase stored indirectly as pointer in SkUserData rather than whole structure
template<> inline SkInvokedBase * SkUserDataBase::as<SkInvokedBase>() const { return as_stored<AIdPtr<SkInvokedBase>>()->get_obj(); }

//---------------------------------------------------------------------------------------
// Storage specialization - SkInvokedContextBase stored indirectly as pointer in SkUserData rather than whole structure
template<> inline SkInvokedContextBase * SkUserDataBase::as<SkInvokedContextBase>() const { return static_cast<SkInvokedContextBase *>(as_stored<AIdPtr<SkInvokedBase>>()->get_obj()); }

//---------------------------------------------------------------------------------------
// Gets the receiver scope object / "this" of this invoked object and and returns it as
// desired type.
// 
// Does any casting etc. that is necessary from SkInstance to the type requested.
// 
// Examples:  
//   // Get the pointer back from an AString
//   AString * str_p = obj_p->this_as<AString>()
//    
// Author(s):   Conan Reis
template <typename _UserType>
inline _UserType * SkInvokedContextBase::this_as_data() const
  {
  const SkInstance * instance_p = static_cast<const SkInstance *>(m_scope_p.get_obj());
  return instance_p->as_data<_UserType>();
  }

//---------------------------------------------------------------------------------------
// Get argument at specified index and return it as desired type.
// 
// Does any casting etc. that is necessary from SkInstance to the type requested.
// 
// Params:  
//   idx: index position of parameter to retrieve
//
// Examples:  
//   // Get the pointer back from an AString
//   AString * str_p = scope_p->get_arg_data<AString>(SkArg_1)
// 
// Author(s):   Conan Reis
template <typename _UserType>
inline _UserType * SkInvokedContextBase::get_arg_data(uint32_t idx) const
  {
  return m_data.get_at(idx)->m_data_p->as_data<_UserType>();
  }

//---------------------------------------------------------------------------------------
// Store user data structure for later retrieval with `get_user_data<>()`. The structure
// can be no more than 2 uintptr_t in size (32-bit: 8 bytes, 64-bit: 16 bytes)
// 
// This is useful for coroutines that need access to persistent info on successive calls
// to their bound C++ routines - for polling etc.
// 
// Params:  
//   ...args: Whatever arguments 0+ are needed for a _UserType constructor.
// 
// Examples:  
//   // Store a smart pointer to a SkActor object.
//   scope_p->append_user_data<AIdPtr<SkActor> >(actor_p);
//   
//   // Or store a float
//   scope_p->append_user_data<float>(num);
//
// 
//   // Get the pointer back when you need it
//   SkActor * actor_p = scope_p->get_user_data<AIdPtr<SkActor> >()
//   
//   // Or get a float back
//   float * num_p = scope_p->get_user_data<float>();
// 
// Notes:  
//   ***This mechanism assumes that the user data is stored at the *end* of this invoked
//   object's data table - so only *one* user data object may be stored in this manner.***
//   
//   The user data has an appropriate constructor called when it is initially stored and
//   its destructor is called on it when this invoked object has finished its invocation.
//   The user data structure is stored in the user data of a SkInstance object that comes
//   from a preallocated pool so it should not need to allocate additional memory.
//   [Though the dynamic pointer array in the data table `m_data` may need to increase.]
// 
// See:         get_user_data<>()
// Author(s):   Conan Reis
template <typename _UserType, typename... _ParamClasses>
_UserType * SkInvokedContextBase::append_user_data(_ParamClasses&... args)
  {
  // Get new SkInstance from the pool, change its virtual table to SkAutoDestroyInstance and
  // call its constructor which in turn calls the _UserType constructor.
  SkInstance * user_obj_p = new (SkInstance::get_pool().pop()) SkAutoDestroyInstance<_UserType, _ParamClasses...>(args...);

  // Add it to the end of this invoked object's data table.
  data_append_arg(ASymbol__user_, user_obj_p);

  return user_obj_p->as_data<_UserType>();
  }

//---------------------------------------------------------------------------------------
// Get pointer to user data that was previously stored with `append_user_data<>()`.
// 
// This is useful for coroutines that need access to persistent info on successive calls
// to their bound C++ routines - for polling etc.
// 
// Examples:  
//   // Get the pointer back from smart pointer to a SkActor object
//   actor_p = scope_p->get_user_data<AIdPtr<SkActor> >()
//   
//   // Or get a float back
//   float * num_p = scope_p->get_user_data<float>();
// 
// Notes:  
//   The user data structure is stored in an SkInstance user data that is added to the
//   end of this invoked object's data table.
// 
// See:         append_user_data<>()
// Author(s):   Conan Reis
template <typename _UserType>
inline _UserType * SkInvokedContextBase::get_user_data() const
  {
  return m_data.get_last()->m_data_p->as_data<_UserType>();
  }

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkInvokedBase.inl>
#endif

#endif  // __SKINVOKEDBASE_HPP
