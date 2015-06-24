//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001-2015 Agog Labs Inc. All rights reserved.
//
// Data structure for simplest type of object in language - instance of a
// class without data members
//
// Author: Conan Reis, Markus Breyer
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/ARefCount.hpp>
#include <AgogCore/AString.hpp>
#include <SkookumScript/SkObjectBase.hpp>
#include <SkookumScript/SkUserData.hpp>

//=======================================================================================
// Global Macros / Defines
//=======================================================================================

//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declarations
class SkBrain;
class SkClass;
class SkList;
class SkCoroutineCall;
class SkExpressionBase;
class SkMethodCall;
class SkInvokedBase;
class SkInvokedCoroutine;
class SkInvokedMethod;


//---------------------------------------------------------------------------------------
// Flags used when converting an SkInstance to a code string
enum eSkCodeFlag
  {
  // If set and a new line character is encountered then replace with escaped new line
  // and also split string literal into multiple lines for better readability:
  //   This:
  //     "First line
  //      Second line"
  //      
  //   if true becomes:
  //     "First line\n" +
  //     "Second line"
  //     
  //   if false becomes:
  //     "First line\nSecond line"
  SkCodeFlag_break_lines   = 1 << 0,

  // Append the class type after the code string. Only used when a String() method is
  // called and ignored for simple types where the type is obvious.
  SkCodeFlag_include_type  = 1 << 1,

  SkCodeFlag__none    = 0,
  SkCodeFlag__default = SkCodeFlag__none
  };


//---------------------------------------------------------------------------------------
// SkookumScript user/reusable instance object / class instance - simplest object without
// any data members and with reference counting and user data
// 
// Subclasses: SkBoolean, SkClosure, SkDataInstance(SkActor), SkInstanceUnreffed(SkMetaClass)
// Author(s):  Conan Reis
class SkInstance : public SkObjectBase, public ARefCountMix<SkInstance>
  {
  // Accesses protected elements
  friend class SkBrain;
  friend class SkClosure;

  public:

  // Methods

    SK_NEW_OPERATORS(SkInstance);

    SkInstance();
    virtual ~SkInstance();

    virtual bool is_metaclass() const;
    SkClass *    get_class() const;
    SkClass *    get_key_class() const;
    void         set_class(SkClass * class_p);

    virtual void delete_this();

    virtual void as_code_append(AString * str_p, uint32_t code_flags = SkCodeFlag__none, SkInvokedBase * caller_p = nullptr) const;
    AString      as_code(uint32_t code_flags = SkCodeFlag__none, SkInvokedBase * caller_p = nullptr) const  { AString str; as_code_append(&str, code_flags, caller_p); return str; }

    #if defined(SK_AS_STRINGS)
      virtual AString         as_string_debug() const;
      virtual const ASymbol & get_name_debug() const                             { return ASymbol::ms_null; }
    #endif

    // !!DEPRECATED!!
    template <typename _UserType> _UserType * as_data() const                   { return m_user_data.as<_UserType>(); }
    template <typename _UserType> void        set_data(_UserType const & value) { return m_user_data.set(value); }


    //---------------------------------------------------------------------------------------
    // Allocate (from pool) and construct a new SkInstance of using _BindingClass
    template <class _BindingClass, typename... _ParamClasses>
    static SkInstance * new_instance(_ParamClasses... args) 
      { 
      return _BindingClass::new_instance(args...); 
      }

    //---------------------------------------------------------------------------------------
    // Allocate memory for internal data (if necessary) and invoke constructor
    template <class _BindingClass, typename... _ParamClasses>
    typename _BindingClass::tDataType & construct(_ParamClasses... constructor_args)
      { 
      return static_cast<_BindingClass*>(this)->construct(constructor_args...); 
      }

    //---------------------------------------------------------------------------------------
    // Invoke destructor of internal data and free memory (if necessary)
    template <class _BindingClass>
    void destruct()
      { 
      static_cast<_BindingClass*>(this)->destruct(); 
      }

    //---------------------------------------------------------------------------------------
    // Get const reference to internal data
    template <class _BindingClass>
    const typename _BindingClass::tDataType & as() const
      { 
      return static_cast<const _BindingClass*>(this)->get_data();
      }

    //---------------------------------------------------------------------------------------
    // Get writable reference to internal data
    template <class _BindingClass>
    typename _BindingClass::tDataType & as()
      {
      return static_cast<_BindingClass*>(this)->get_data();
      }

    // Method Invocation

      void         call_default_constructor();
      void         call_destructor();
      virtual void method_invoke(const SkMethodCall & mcall, SkObjectBase * scope_p, SkInvokedBase * caller_p, SkInstance ** result_pp);

  //---------------------------------------------------------------------------------------
  // Evaluates the method with 0 or more arguments and returns immediately
  // 
  // Params:
  //   method_name: name of method to call
  //   args_pp:
  //     Optional pointers to object instances to use as arguments - each one present
  //     should have its reference count incremented and each defaulted/skipped argument
  //     should be a `nullptr` element. If `arg_count` is 0 this is ignored
  //   arg_count:
  //     number of arguments to use in `args_pp`. If it is 0 then no arguments are passed
  //     and `args_pp` is ignored.
  //   caller_p:
  //     object that called/invoked this expression and that may await a result.  If it is
  //     nullptr, then there is no object that needs to be notified when this invocation is
  //     complete.
  //   result_pp:
  //     Pointer to a pointer to store the instance resulting from the invocation of this
  //     expression.  If it is nullptr, then the result does not need to be returned and
  //     only side-effects are desired.
  //     
  // Notes:
  //   This is a convenience method to use instead of method_invoke() - if more arguments
  //   or control is desired, then use method_invoke()
  //   
  // See:
  //   method_call(), method_query(), method_invoke(), call_destructor(),
  //   call_default_constructor()
  virtual void method_call(
    const ASymbol & method_name,
    SkInstance **   args_pp,
    uint32_t        arg_count,
    SkInstance **   result_pp = nullptr,
    SkInvokedBase * caller_p = nullptr
    );

  //---------------------------------------------------------------------------------------
  // Evaluates the method with 0/1 arguments and returns immediately
  // 
  // Params:
  //   method_name: name of method to call
  //   arg_p:
  //     Optional pointer to object instance to use as an argument.  If it is present it
  //     should have its reference count incremented.  If it is nullptr, then no arguments
  //     are passed.
  //   caller_p:
  //     object that called/invoked this expression and that may await a result.  If it is
  //     nullptr, then there is no object that needs to be notified when this invocation is
  //     complete.
  //   result_pp:
  //     Pointer to a pointer to store the instance resulting from the invocation of this
  //     expression.  If it is nullptr, then the result does not need to be returned and
  //     only side-effects are desired.
  //     
  // Notes:
  //   This is a convenience method to use instead of `method_call(name, args_pp, --)`
  //   - if more arguments or control is desired, then use it instead
  //   
  // See:
  //   method_call(), method_query(), method_invoke(), call_destructor(),
  //   call_default_constructor()
  void method_call(
    const ASymbol & method_name,
    SkInstance *    arg_p = nullptr,
    SkInstance **   result_pp = nullptr,
    SkInvokedBase * caller_p = nullptr
    );

  //---------------------------------------------------------------------------------------
  // Evaluates the method with 0/1 arguments and returns a Boolean `true` or
  // `false` result immediately.
  // 
  // Returns: the result of the method call as `true` or `false`.
  // 
  // Params:
  //   method_name: name of method to call
  //   arg_p:
  //     Optional argument to be passed to method.  If it is nullptr, then no arguments are
  //     passed.
  //   caller_p:
  //     Object that called/invoked this expression and that may await a result.  If it is
  //     `nullptr`, then there is no object that needs to be notified when this invocation
  //     is complete.
  // 
  // Notes:
  //   This is a convenience method to use instead of `method_call(name, args_pp, --)`
  //   - if more arguments or control is desired, then use it instead
  // 
  // See: method_query(), method_invoke(), call_destructor(), call_default_constructor()
  bool method_query(
    const ASymbol & method_name,
    SkInstance *    arg_p    = nullptr,
    SkInvokedBase * caller_p = nullptr
    );

    // Coroutine Invocation

      void                 coroutine_schedule(const SkCoroutineCall & cocall, f32 update_interval = SkCall_interval_always, SkInvokedBase * caller_p = nullptr, SkMind * updater_p = nullptr);
      SkInvokedCoroutine * coroutine_call(const ASymbol & coroutine_name, SkInstance ** args_pp, uint32_t arg_count, bool immediate = true, f32 update_interval = SkCall_interval_always, SkInvokedBase * caller_p = nullptr, SkMind * updater_p = nullptr);
      SkInvokedCoroutine * coroutine_call(const ASymbol & coroutine_name, SkInstance * arg_p = nullptr, bool immediate = true, f32 update_interval = SkCall_interval_always, SkInvokedBase * caller_p = nullptr, SkMind * updater_p = nullptr);

    // Overriding from SkObjectBase

      virtual SkInstance *  as_new_instance() const                         { reference(); return const_cast<SkInstance *>(this); }
      virtual SkInstance *  find_data_by_name(const ASymbol & name) const override;
      virtual SkInstance *  get_data_by_name(const ASymbol & name) const;
      virtual uint32_t      get_obj_type() const                            { return SkObjectType_instance; } 
      virtual SkInstance *  get_topmost_scope() const;
      virtual void          set_data_by_name(const ASymbol & name, SkInstance * obj_p);

    // Overriding from ARefCountMix<>

      virtual void on_no_references();


  // Class Methods

    template<typename _UserType>
    static SkInstance * new_instance(SkClass * class_p, const _UserType & user_data);
    static SkInstance * new_instance(SkClass * class_p);
    static AObjReusePool<SkInstance> & get_pool();

  protected:

  // User data type definition

    typedef SkUserData<2> tUserData;

  // Internal Methods

    friend class AObjBlock<SkInstance>;  // So it can be accessed only by pool_new()

    SkInstance(SkClass * class_p);
    SkInstance(SkClass * class_p, const tUserData & user_data);

  // Data Members

    // Class this is an instance of - stores methods and other info shared by instances
    SkClass * m_class_p;

    // $Note - CReis Could allow a redirection pointer - to allow direct hook-ins to already
    // existing data structures.  This would make a wasted 4 bytes for each type.

    // $Note - CReis [Memory]
    // The address of m_user_data can be used with a placement new of a data structure
    // up to pointer size * 2 bytes.
    // This includes most smart pointers like AIdxPtr<> and AIdPtr<>
    tUserData  m_user_data;


  };  // SkInstance


//---------------------------------------------------------------------------------------
// Specialization - also ensures that `SkInvokedBase::get_arg_data<SkClass>(--)` etc. work properly
// 
// See: SkInstance::as<SkMetaClass>()
template<> inline SkClass * SkInstance::as_data<SkClass>() const  { return m_class_p; }


// Used to set an effectively *infinite* reference count so that the instance never gets
// "garbage collected" - in combination with overriding on_no_referenced() just to make sure.
const uint32_t SkInstanceUnreffed_infinite_ref_count = 1000000u;


//---------------------------------------------------------------------------------------
// Notes      SkookumScript instance object / class instance - that ignores reference
//            counting and has user data no data members.
//            
//            Mainly used as a hack for nil so that it does not need to be reference
//            counted - though it has the unnecessary overhead of the user data and
//            reference count variables.  [Mechanism to ignore reference counting is a bit
//            hacky though it saves splitting apart SkInstance into 2 or more classes and
//            rewriting all the code that references it.]
// Subclasses SkMetaClass
// Author(s)  Conan Reis
class SkInstanceUnreffed : public SkInstance
  {
  public:

  // Methods

    SK_NEW_OPERATORS(SkInstanceUnreffed);
    SkInstanceUnreffed();
    SkInstanceUnreffed(SkClass * class_p);

    // Overriding from SkInstance (overriding from ARefCountMix<>)

      virtual void on_no_references();

  };


//=======================================================================================
// Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Retrieves an instance object from the dynamic pool and initializes it for use.
// This method should be used instead of 'new' because it prevents unnecessary
// allocations by reusing previously allocated objects.
// Returns:    a dynamic SkInstance
// See:        pool_delete() 
// Notes:      To 'deallocate' an object that was retrieved with this method, use
//             'pool_delete()' rather than 'delete'.
// Modifiers:   static
// Author(s):   Markus Breyer
template <typename _UserType>
inline SkInstance * SkInstance::new_instance(SkClass * class_p, const _UserType & user_data)
  {
  SkInstance * instance_p = new_instance(class_p);
  
  instance_p->set_data(user_data);

  return instance_p;
  }

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkInstance.inl>
#endif

