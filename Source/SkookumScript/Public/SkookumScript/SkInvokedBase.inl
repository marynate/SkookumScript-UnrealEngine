//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Base classes for executed/called/invoked objects inline file
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AObjReusePool.hpp>
#include <SkookumScript/SkBrain.hpp>
#include <SkookumScript/SkExpressionBase.hpp>


//=======================================================================================
// SkInvokedBase Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
//  Constructor
// Returns:     itself
// Arg          caller_p - Object that invoked / called this routine and now expects to
//              be returned to upon the completion of this routine.  nullptr if there is
//              nothing to return to.  (Default nullptr)
// Arg          scope_p - Path for data look-ups (the owner).  nullptr if this object is
//              at the topmost scope.  (Default nullptr)
// Author(s):    Conan Reis
A_INLINE SkInvokedBase::SkInvokedBase(
  SkInvokedBase * caller_p, // = nullptr
  SkObjectBase *  scope_p   // = nullptr
  ) :
  m_scope_p(scope_p),
  m_caller_p(caller_p),
  m_pending_count(0u)
  {
  m_ptr_id = ++ms_ptr_id_prev;
  }

//---------------------------------------------------------------------------------------
// virtual destructor
// Author(s):   Conan Reis
A_INLINE SkInvokedBase::~SkInvokedBase()
  {
  m_ptr_id = AIdPtr_null;
  }

//---------------------------------------------------------------------------------------
// Gets the mind that is updating/owning this invoked object.
// Modifiers:   virtual - override for custom behaviour
// Author(s):   Conan Reis
A_INLINE SkMind * SkInvokedBase::get_updater() const
  {
  SkInvokedBase * caller_p = m_caller_p;

  return caller_p ? caller_p->get_updater() : nullptr;
  }

//---------------------------------------------------------------------------------------
// Called when an invoked sub-expression is deferred/did not complete
//             immediately and this invokable's completion is pending on the eventual
//             completion of the sub-expression.
// Arg         deferred_p - invoked sub-expression that was deferred
// Author(s):   Conan Reis
A_INLINE void SkInvokedBase::pending_deferred(SkInvokedBase * deferred_p)
  {
  m_calls.append(deferred_p);
  pending_increment();
  }

//---------------------------------------------------------------------------------------
// Called when an invoked sub-expression returns from a pending task and
//             notifies this invokable that it no longer needs to wait for it.
// Arg         deferred_p - invoked sub-expression that was deferred, but is now completed
// Arg         completed - true if sub-expression completed successfully, false if the
//             sub-expression was aborted/did not complete successfully - thus this
//             invokable should also probably clean up and quit.
// Author(s):   Conan Reis
A_INLINE void SkInvokedBase::pending_unregister(
  SkInvokedBase * deferred_p,
  bool            completed // = true
  )
  {
  // $Revisit - CReis Shouldn't need the in list test.
  if (deferred_p->is_in_list())
    {
    m_calls.remove(deferred_p);
    }
  //else
  //  {
  //  SK_DPRINTF("\nSkookum: Tried to remove invokeable that was already removed!\n");
  //  }

  pending_return(completed);
  }

//---------------------------------------------------------------------------------------
// Common portion of abort calls - aborts all sub-calls, abort this invoked
//             expression and optionally notify any caller that this has failed to complete.
// Arg         notify_caller - see eSkNotify
// Arg         notify_child - see eSkNotifyChild
// Modifiers:   virtual - overridden from SkInvokedBase
// Author(s):   Conan Reis
A_INLINE void SkInvokedBase::abort_common(
  eSkNotify      notify_caller,
  eSkNotifyChild notify_child
  )
  {
  // Abort/detach sub-calls

  // Ensure that pending count is not > 0 for these calls so that this invocation is
  // no longer pending on anything.
  m_pending_count = 0u;

  if (notify_child == SkNotifyChild_abort)
    {
    abort_subcalls(notify_caller);
    }
  else
    {
    detach_subcalls(notify_caller);
    }

  // Generally followed up with cleaning out any other context memory and putting this
  // invokable back in a pool or deleting it.
  // For example:
  //pool_delete(this);
  }


//=======================================================================================
// SkInvokedExpression Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Retrieves an invoked expression object from the dynamic pool and
//             initializes it for use.  This should be used instead of 'new' because it
//             prevents unnecessary allocations by reusing previously allocated objects.
// Returns:    a reusable dynamic SkInvokedExpression 
// Arg         expr - the expression that is invoked/running
// Arg         caller_p - the invoked custom coroutine or method that invoked this
//             SkookumScript/SkInvokedCoroutine.  nullptr if there is nothing to return to.
// Arg         scope_p - Path for data look-ups (the owner).  nullptr if this object is
//             at the topmost scope.
// See:        pool_delete()
// Notes:      To "deallocate" an object that was retrieved with this method, use
//             pool_delete() rather than 'delete'.
// Author(s):   Conan Reis
A_INLINE SkInvokedExpression * SkInvokedExpression::pool_new(
  const SkExpressionBase & expr,
  SkInvokedBase *          caller_p,
  SkObjectBase *           scope_p
  )
  {
  SkInvokedExpression * iexpr_p = get_pool().pop();

  iexpr_p->m_expr_p        = &expr;
  iexpr_p->m_caller_p      = caller_p;
  iexpr_p->m_scope_p       = scope_p;
  iexpr_p->m_pending_count = 0u;
  iexpr_p->m_index         = 0u;
  iexpr_p->m_ptr_id        = ++ms_ptr_id_prev;

  return iexpr_p;
  }

//---------------------------------------------------------------------------------------
// Gets the mind that is updating/owning this invoked object.
// Modifiers:   virtual - override for custom behaviour
// Author(s):   Conan Reis
A_INLINE SkMind * SkInvokedExpression::get_updater() const
  {
  return m_expr_p->get_updater(*this);
  }


//=======================================================================================
// SkInvokedContextBase Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis
A_INLINE SkInstance * SkInvokedContextBase::get_this()
  {
  return static_cast<SkInstance *>(m_scope_p.get_obj());
  }

//---------------------------------------------------------------------------------------
// Adds the given temporary variable name so that it may be used to refer
//             to instances of objects making it part of the 'temporary variable stack'.
//             It is initially set to the 'nil' object.
// Arg         var_name - The identifier name of the temporary variable to append
// See:        data_remove_vars(), data_empty(), data_append_args() 
// Author(s):   Conan Reis
A_INLINE void SkInvokedContextBase::data_append_var(const ASymbol & var_name)
  {
  // nil does not need to be referenced/dereferenced
  m_data.append(*SkData::pool_new(var_name));
  }

//---------------------------------------------------------------------------------------
// Sets/binds *previously referenced* argument at specified position to the object
// specified.
//
// #See Also  set_arg_and_ref() and all the other set_*() and get_*() methods
// #Author(s) Conan Reis
A_INLINE void SkInvokedContextBase::set_arg(
  // 0-based index position of argument to set/bind - use values from eSkArgNum to make it
  // more readable.
  uint32_t pos,
  // Object to set/bind - assumes that it already has its reference count incremented
  SkInstance * obj_p
  )
  {
  SkData * data_p = m_data.get_at(pos);

  data_p->m_data_p->dereference();
  data_p->m_data_p = obj_p;
  }

//---------------------------------------------------------------------------------------
// Sets/binds argument *and auto-references it* at specified position to the object
// specified.
//
// #See Also  set_arg() and all the other set_*() and get_*() methods
// #Author(s) Conan Reis
A_INLINE void SkInvokedContextBase::set_arg_and_ref(
  // 0-based index position of argument to set/bind - use values from eSkArgNum to make it
  // more readable.
  uint32_t pos,
  // Object to set/bind - assumes that it already has its reference count incremented
  SkInstance * obj_p
  )
  {
  SkData * data_p = m_data.get_at(pos);

  // The new object is referenced before the old object is dereferenced since it could
  // be the same object
  obj_p->reference();
  data_p->m_data_p->dereference();
  data_p->m_data_p = obj_p;
  }
