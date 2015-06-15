//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2014 Agog Labs Inc.,
// All rights reserved.
//
// SkookumScript (Sk) Unreal Engine (UE) Bindings
// 
// #Author(s):  Markus Breyer
//=======================================================================================


#ifndef __SKOOKUMSCRIPTLISTENERMANAGER_HPP
#define __SKOOKUMSCRIPTLISTENERMANAGER_HPP
#pragma once


//=======================================================================================
// Includes
//=======================================================================================

#include "../Classes/SkookumScriptListener.h"

//=======================================================================================
// Global Defines / Macros
//=======================================================================================

//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// Keep track of USkookumScriptListener instances
class SkookumScriptListenerManager
  {
  public:

    static SkookumScriptListenerManager *   get_singleton();

    // Methods

    SkookumScriptListenerManager(uint32_t pool_init, uint32_t pool_incr);
    ~SkookumScriptListenerManager();

    USkookumScriptListener *                alloc_listener(UObject * obj_p, SkInvokedCoroutine * coro_p, USkookumScriptListener::tUnregisterCallback callback_p);
    void                                    free_listener(USkookumScriptListener * listener_p);

    USkookumScriptListener::EventInfo *     alloc_event();
    void                                    free_event(USkookumScriptListener::EventInfo * event_p, uint32_t num_arguments_to_free);

  protected:

    typedef APArray<USkookumScriptListener> tObjPool;
    typedef AObjReusePool<USkookumScriptListener::EventInfo> tEventPool;

    void              grow_inactive_list(uint32_t pool_incr);

    tObjPool          m_inactive_list;
    tObjPool          m_active_list;
    uint32_t          m_pool_incr;

    tEventPool        m_event_pool;

  }; // SkookumScriptListenerManager

//=======================================================================================
// Inline Functions
//=======================================================================================

//---------------------------------------------------------------------------------------

inline USkookumScriptListener::EventInfo * SkookumScriptListenerManager::alloc_event()
  {
  return m_event_pool.pop();
  }

//---------------------------------------------------------------------------------------
            
inline void SkookumScriptListenerManager::free_event(USkookumScriptListener::EventInfo * event_p, uint32_t num_arguments_to_free)
  {
  for (uint32_t i = 0; i < num_arguments_to_free; ++i)
    {
    event_p->m_argument_p[i]->dereference();
    }
  m_event_pool.append(event_p);
  }

#endif // __SKOOKUMSCRIPTLISTENERMANAGER_HPP

