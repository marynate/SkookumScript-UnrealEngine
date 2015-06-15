//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Class wrapper for executed/called/invoked methods inline file
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AObjReusePool.hpp>
#include <SkookumScript/SkInstance.hpp>
#include <SkookumScript/SkMethod.hpp>


//=======================================================================================
// SkInvokedMethod Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Retrieves an invoked method object from the dynamic pool and initializes
//             it for use.  This should be used instead of 'new' because it prevents
//             unnecessary allocations by reusing previously allocated objects.
// Returns:    a dynamic invoked coroutine
// Arg         caller_p - the invoked method / coroutine that invoked this
//             SkookumScript/SkInvokedMethod.  nullptr if there is nothing to return to.
// Arg         scope_p - Path for data look-ups (the owner).  nullptr if this object is
//             at the topmost scope.  (Default nullptr)
// Notes:      To "deallocate" an object that was retrieved with this method, use
//             pool_delete() rather than 'delete'.
//             SK_IMETHOD_DEBUG() must be called after each call to this method to give
//             additional debugging information.
// Author(s):   Conan Reis
A_INLINE SkInvokedMethod * SkInvokedMethod::pool_new(
  SkInvokedBase * caller_p,
  SkObjectBase *  scope_p,
  SkMethodBase *  method_p
  )
  {
  SkInvokedMethod * imethod_p = get_pool().pop();

  imethod_p->m_caller_p = caller_p;
  imethod_p->m_scope_p  = scope_p;
  imethod_p->m_ptr_id   = ++ms_ptr_id_prev;
  imethod_p->m_method_p = method_p;

  return imethod_p;
  }

//---------------------------------------------------------------------------------------
// Frees up an invoked method and puts it into the dynamic pool ready for its
//             next use.  This should be used instead of 'delete' because it prevents
//             unnecessary deallocations by saving previously allocated objects.
// Arg         imethod_p - pointer to invoked method to free up and put into the dynamic
//             pool ready for its next use.
// Notes:      To "allocate" an object use 'pool_new' rather than 'new'.
// Author(s):   Conan Reis
A_INLINE void SkInvokedMethod::pool_delete(SkInvokedMethod * imethod_p)
  {
  imethod_p->data_empty_compact();  // $Revisit - CReis Compacting here to free up memory, but likely a little slower
  imethod_p->m_calls.empty();       // $Revisit - Empty call list - might be redundant
  imethod_p->AListNode<SkInvokedBase>::remove();
  imethod_p->m_ptr_id = AIdPtr_null;
  get_pool().append(imethod_p);
  }


//=======================================================================================
// SkInvokedDeferrableMethod Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Calls the wrapped method expecting immediate completion with an optional
//             result.
// Arg         result_pp - pointer to a pointer to store the instance resulting from the
//             invocation of this expression.  If it is nullptr, then the result does not
//             need to be returned and only side-effects are desired.  (Default nullptr)
// Author(s):   Conan Reis
A_INLINE void SkInvokedDeferrableMethod::invoke(
  SkInstance ** result_pp // = nullptr
  )
  {
  m_method_p->invoke(this, this, result_pp);
  }

//---------------------------------------------------------------------------------------
// Calls the method essentially as a simple coroutine - with the possibility 
//             that completion will be deferred - i.e. it will be durational.
// Returns:    deferred invoked sub-expression or nullptr if completed immediately.
// Arg         result_pp - pointer to a pointer to store the instance resulting from the
//             invocation of this expression.  The result will probably only be interesting
//             if it completes immediately.  If it is nullptr, then the result does not need
//             to be returned and only side-effects are desired.  (Default nullptr)
// Author(s):   Conan Reis
A_INLINE SkInvokedBase * SkInvokedDeferrableMethod::invoke_deferred(
  SkInstance ** result_pp // = nullptr
  )
  {
  SkInvokedBase * deferred_p = static_cast<SkMethod *>(m_method_p)->invoke_deferred(this, this, result_pp);

  if (deferred_p)
    {
    pending_deferred(deferred_p);
    }

  return deferred_p;
  }

//---------------------------------------------------------------------------------------
// Calls the wrapped method expecting immediate completion and returns the
//             result.
// Returns:    Result from invocation.
// Author(s):   Conan Reis
A_INLINE SkInstance * SkInvokedDeferrableMethod::invoke_now()
  {
  SkInstance * result_p;

  m_method_p->invoke(this, this, &result_p);

  return result_p;
  }


//=======================================================================================
// SkIExternalMethodCallWrapper Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Author(s):   Conan Reis, Richard Orth
A_INLINE SkIExternalMethodCallWrapper::~SkIExternalMethodCallWrapper()
  {
  if (m_finished_p)
    {
    *m_finished_p = true;
    }

  // want to keep the external code around
  ((SkMethod*)m_method_p)->replace_expression(nullptr);

  delete m_method_p;
  }

//---------------------------------------------------------------------------------------
// If the external caller no longer exists then we need to clear the finish pointer
//             so don't stomp memory
// Author(s):   Richard Orth
A_INLINE void SkIExternalMethodCallWrapper::clear_finished_check()
  {
  m_finished_p = nullptr;
  }
