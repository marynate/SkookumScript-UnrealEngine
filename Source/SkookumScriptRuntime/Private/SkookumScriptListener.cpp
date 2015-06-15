//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2014 Agog Labs Inc.,
// All rights reserved.
//
// SkookumScript (Sk) Unreal Engine (UE) Bindings
// 
// Based on: UObject
// 
// #Author(s):  Markus Breyer
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include "SkookumScriptRuntimePrivatePCH.h"
#include "../Classes/SkookumScriptListener.h"
#include "SkookumScriptListenerManager.hpp"

#include "Bindings/VectorMath/SkVector3.hpp"
#include "Bindings/Engine/SkUEName.hpp"
#include "SkUEActor.generated.hpp"
#include "SkUEDamageType.generated.hpp"
#include "SkUEController.generated.hpp"
#include "SkUEPrimitiveComponent.generated.hpp"

//=======================================================================================
// FSkookumScriptListenerAutoPtr
//======================================================================================

FSkookumScriptListenerAutoPtr::~FSkookumScriptListenerAutoPtr()
  {
  USkookumScriptListener * listener_p = Get();
  SK_ASSERTX(listener_p, "USkookumScriptListeners are entirely under Sk control and cannot just go away.");
  SkookumScriptListenerManager::get_singleton()->free_listener(listener_p);
  }

//=======================================================================================
// Class Data
//=======================================================================================

//=======================================================================================
// Method Definitions
//=======================================================================================

//---------------------------------------------------------------------------------------

USkookumScriptListener::USkookumScriptListener(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , m_unregister_callback_p(nullptr)
  , m_num_arguments(0)
  {
  }

//---------------------------------------------------------------------------------------

void USkookumScriptListener::initialize(UObject * obj_p, SkInvokedCoroutine * coro_p, tUnregisterCallback callback_p)
  {
  SK_ASSERTX(!coro_p->is_suspended(), "Coroutine must not be suspended yet when delegate object is initialized.");

  m_obj_p = obj_p;
  m_coro_p = coro_p;
  m_unregister_callback_p = callback_p;
  m_num_arguments = 0;
  }

//---------------------------------------------------------------------------------------

void USkookumScriptListener::deinitialize()
  {
  // Kill any events that are still around
  while (has_event())
    {
    free_event(pop_event(), true);
    }

  // Forget the coroutine we keep track of
  m_coro_p.null();

  // Unregister from delegate list if any
  if (m_unregister_callback_p && m_obj_p.IsValid())
    {
    (*m_unregister_callback_p)(m_obj_p.Get(), this);
    }
  }

//---------------------------------------------------------------------------------------

USkookumScriptListener::EventInfo * USkookumScriptListener::alloc_event()
  {
  EventInfo * event_p = SkookumScriptListenerManager::get_singleton()->alloc_event();
  #if (SKOOKUM & SK_DEBUG)
    ::memset(event_p->m_argument_p, 0, sizeof(event_p->m_argument_p));
  #endif
  return event_p;
  }

//---------------------------------------------------------------------------------------

void USkookumScriptListener::free_event(EventInfo * event_p, bool free_arguments)
  {
  SkookumScriptListenerManager::get_singleton()->free_event(event_p, free_arguments ? m_num_arguments : 0);
  }

//---------------------------------------------------------------------------------------

inline void USkookumScriptListener::push_event_and_resume(EventInfo * event_p, uint32_t num_arguments)
  {
  #if (SKOOKUM & SK_DEBUG)
    for (uint32_t i = 0; i < num_arguments; ++i) SK_ASSERTX(event_p->m_argument_p[i], "All event arguments must be set.");
    for (uint32_t i = num_arguments; i < A_COUNT_OF(event_p->m_argument_p); ++i) SK_ASSERTX(!event_p->m_argument_p[i], "Unused event arguments must be left alone.");
    SK_ASSERTX(m_num_arguments == 0 || m_num_arguments == num_arguments, "All events must have same argument count.");
  #endif
  m_num_arguments = num_arguments;
  m_event_queue.append(event_p);
  if (m_coro_p.is_valid()) m_coro_p->resume();
  }

//=======================================================================================
// Various event handlers
//======================================================================================

//---------------------------------------------------------------------------------------

void USkookumScriptListener::OnActorOverlap(AActor * other_actor_p)
  {
  EventInfo * event_p = alloc_event();
  event_p->m_argument_p[SkArg_1] = SkUEActor::new_instance(other_actor_p);
  push_event_and_resume(event_p, 1);
  }

//---------------------------------------------------------------------------------------

void USkookumScriptListener::OnTakeAnyDamage(float damage, const UDamageType * damage_type_p, AController * instigated_by_p, AActor * damage_causer_p)
  {
  EventInfo * event_p = alloc_event();
  event_p->m_argument_p[SkArg_1] = SkReal::new_instance(damage);
  event_p->m_argument_p[SkArg_2] = SkUEDamageType::new_instance(const_cast<UDamageType*>(damage_type_p));
  event_p->m_argument_p[SkArg_3] = SkUEController::new_instance(instigated_by_p);
  event_p->m_argument_p[SkArg_4] = SkUEActor::new_instance(damage_causer_p);
  push_event_and_resume(event_p, 4);
  }

//---------------------------------------------------------------------------------------

void USkookumScriptListener::OnTakePointDamage(float damage, AController * instigated_by_p, FVector hit_location, UPrimitiveComponent * hit_component_p, FName bone_name, FVector shot_from_direction, const UDamageType * damage_type_p, AActor * damage_causer_p)
  {
  EventInfo * event_p = alloc_event();
  event_p->m_argument_p[SkArg_1] = SkReal::new_instance(damage);
  event_p->m_argument_p[SkArg_2] = SkUEController::new_instance(instigated_by_p);
  event_p->m_argument_p[SkArg_3] = SkVector3::new_instance(hit_location);
  event_p->m_argument_p[SkArg_4] = SkUEPrimitiveComponent::new_instance(hit_component_p);
  event_p->m_argument_p[SkArg_5] = SkUEName::new_instance(bone_name);
  event_p->m_argument_p[SkArg_6] = SkVector3::new_instance(shot_from_direction);
  event_p->m_argument_p[SkArg_7] = SkUEDamageType::new_instance(const_cast<UDamageType*>(damage_type_p));
  event_p->m_argument_p[SkArg_8] = SkUEActor::new_instance(damage_causer_p);
  push_event_and_resume(event_p, 8);
  }

//---------------------------------------------------------------------------------------

void USkookumScriptListener::OnDestroyed()
  {
  push_event_and_resume(alloc_event(), 0);
  }
