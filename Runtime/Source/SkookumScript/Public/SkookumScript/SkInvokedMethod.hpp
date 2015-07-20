//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Class wrapper for executed/called/invoked methods declaration file
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKINVOKEDMETHOD_HPP
#define __SKINVOKEDMETHOD_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkInvokedBase.hpp>


//=======================================================================================
// Defines
//=======================================================================================

//=======================================================================================
// Global Structures
//=======================================================================================

class SkMethodBase;  // Pre-declaration

//---------------------------------------------------------------------------------------
// Notes      Wrapper for executed/called/invoked methods
// Author(s)  Conan Reis
class SkInvokedMethod : public SkInvokedContextBase
  {
  public:
  // Common Methods

    SK_NEW_OPERATORS(SkInvokedMethod);

    SkInvokedMethod(SkInvokedBase * caller_p = nullptr, SkObjectBase * scope_p = nullptr, SkMethodBase * method_p = nullptr) : SkInvokedContextBase(caller_p, scope_p), m_method_p(method_p) {}

  // Methods

	void set_method(SkMethodBase * method_p)                        { m_method_p = method_p; }
    //SkInstance * get_this();

    // Overriding from SkInvokedBase

      virtual void abort_invoke(eSkNotify notify_caller = SkNotify_fail, eSkNotifyChild notify_child = SkNotifyChild_abort);
      virtual void pending_return(bool completed = true);

      virtual eSkMember         get_invoke_type() const;
      virtual SkInvokableBase * get_invokable() const;

      #if defined(SK_AS_STRINGS)
        virtual AString  as_string_debug() const;
      #endif

    // Inherited from SkObjectBase -> SkInvokedContextBase

      virtual SkInstance * as_new_instance() const;

  // Class Methods

    static AObjReusePool<SkInvokedMethod> & get_pool();
    static void                             pool_delete(SkInvokedMethod * imethod_p);
    static SkInvokedMethod *                pool_new(SkInvokedBase * caller_p, SkObjectBase * scope_p, SkMethodBase * method_p);

    // SkookumScript Atomic Methods

      static void register_bindings();

      static void mthd_String(SkInvokedMethod * scope_p, SkInstance ** result_pp);

  protected:

  // Data Members

    // Atomic or custom code object that is being invoked - see pool_new()
    SkMethodBase * m_method_p;

    // Future:  Used with dynamic scoping first-order code blocks.
    //uint32_t m_scope_id;

  };  // SkInvokedMethod


//---------------------------------------------------------------------------------------
// Method/coroutine hybrid - essentially a coroutine that can return a result.
//             Simple modification to invoked method to allow it to handle durational code
//             to a limited extent.
//             Used primarily for quick and dirty string -> executing code in script
//             consoles.
// Also See    SkParser::invoke_script()
// Author(s):   Conan Reis
class SkInvokedDeferrableMethod : public SkInvokedMethod
  {
  public:

    SkInvokedDeferrableMethod(SkObjectBase * scope_p = nullptr, SkMethodBase * method_p = nullptr) : SkInvokedMethod(nullptr, scope_p, method_p) {}

    void            invoke(SkInstance ** result_pp = nullptr);
    SkInvokedBase * invoke_deferred(SkInstance ** result_pp = nullptr);
    SkInstance *    invoke_now();

    // Overriding from SkInvokedBase

      virtual void abort_invoke(eSkNotify notify_caller = SkNotify_fail, eSkNotifyChild notify_child = SkNotifyChild_abort);
      virtual void pending_return(bool completed = true);
  };


//---------------------------------------------------------------------------------------
// Simple modification to invoked method to allow it to handle External Code
//             that may or may not actually be a coroutine
// Author(s):   Richard Orth
class SkIExternalMethodCallWrapper : public SkInvokedDeferrableMethod
  {
  public:

  // Common Methods

    SK_NEW_OPERATORS(SkIExternalMethodCallWrapper);

    SkIExternalMethodCallWrapper(SkObjectBase * scope_p = nullptr, SkMethodBase * method_p = nullptr) : SkInvokedDeferrableMethod(scope_p, method_p), m_finished_p(nullptr) {}
	~SkIExternalMethodCallWrapper();

  // Overriding from SkInvokedBase

    virtual void abort_invoke(eSkNotify notify_caller = SkNotify_fail, eSkNotifyChild notify_child = SkNotifyChild_abort);
    virtual void pending_return(bool completed = true);

  // Methods

    void clear_finished_check();

  // Data Members

    bool * m_finished_p;

  };  // SkIExternalMethodCallWrapper


//=======================================================================================
// Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Storage specialization - SkInvokedMethod stored indirectly as pointer in SkUserData rather than whole structure
template<> inline SkInvokedMethod *  SkUserDataBase::as<SkInvokedMethod>() const          { return static_cast<SkInvokedMethod *>(as_stored<AIdPtr<SkInvokedBase>>()->get_obj()); }
template<> inline void               SkUserDataBase::set(SkInvokedMethod * const & value) { *as_stored<AIdPtr<SkInvokedBase>>() = value; }


#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkInvokedMethod.inl>
#endif


#endif  // __SKINVOKEDMETHOD_HPP

