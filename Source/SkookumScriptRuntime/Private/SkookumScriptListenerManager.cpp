//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2015 Agog Labs Inc.
// All rights reserved.
//
// SkookumScript (Sk) Unreal Engine (UE) Bindings
// 
// #Author(s):  Markus Breyer
//=======================================================================================

//=======================================================================================
// Includes
//=======================================================================================

#include "SkookumScriptRuntimePrivatePCH.h"
#include "SkookumScriptListenerManager.hpp"
#include "Bindings/SkUERuntime.hpp"

//---------------------------------------------------------------------------------------

SkookumScriptListenerManager * SkookumScriptListenerManager::get_singleton()
  {
  SkUERuntime * runtime_p = SkUERuntime::get_singleton();
  return runtime_p->get_listener_manager();
  }

//---------------------------------------------------------------------------------------

SkookumScriptListenerManager::SkookumScriptListenerManager(uint32_t pool_init, uint32_t pool_incr)
  : m_pool_incr(pool_incr)
  , m_event_pool(pool_init, pool_incr) // $Revisit MBreyer - use separate settings for delegate objects and events
  {
  grow_inactive_list(m_pool_incr);
  m_active_list.ensure_size(pool_init);
  }

//---------------------------------------------------------------------------------------

SkookumScriptListenerManager::~SkookumScriptListenerManager()
  {
  while (!m_active_list.is_empty())
    {
    USkookumScriptListener * listener_p = m_active_list.pop_last();
    if (listener_p->IsValidLowLevel())
      {
      listener_p->ConditionalBeginDestroy();
      }
    }
  while (!m_inactive_list.is_empty())
    {
    USkookumScriptListener * listener_p = m_inactive_list.pop_last();
    if (listener_p->IsValidLowLevel())
      {
      listener_p->ConditionalBeginDestroy();
      }
    }
  }

//---------------------------------------------------------------------------------------

USkookumScriptListener * SkookumScriptListenerManager::alloc_listener(UObject * obj_p, SkInvokedCoroutine * coro_p, USkookumScriptListener::tUnregisterCallback callback_p)
  {
  if (m_inactive_list.is_empty())
    {
    grow_inactive_list(m_pool_incr);
    }
  USkookumScriptListener * delegate_obj = m_inactive_list.pop_last();
  delegate_obj->initialize(obj_p, coro_p, callback_p);
  m_active_list.append(*delegate_obj);
  return delegate_obj;
  }

//---------------------------------------------------------------------------------------

void SkookumScriptListenerManager::free_listener(USkookumScriptListener * listener_p)
  {
  if (m_active_list.pop(*listener_p))
    {
    listener_p->deinitialize();
    m_inactive_list.append(*listener_p);
    }
  else
    {
    SK_ERRORX("SkookumScriptListener not found in active list.");
    }
  }

//---------------------------------------------------------------------------------------

void SkookumScriptListenerManager::grow_inactive_list(uint32_t pool_incr)
  {
  m_inactive_list.ensure_size(m_inactive_list.get_length() + pool_incr);
  for (uint32_t i = 0; i < pool_incr; ++i)
    {
    m_inactive_list.append(*NewObject<USkookumScriptListener>());
    }
  m_active_list.ensure_size(m_inactive_list.get_length());
  }

