//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// UObject that listens to dynamic multicast delegate events
// 
// Author(s): Markus Breyer
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include "GameFramework/Actor.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Controller.h"
#include "Components/PrimitiveComponent.h"
#include "UnrealMath.h"          // Vector math functions.

#include "SkookumScriptListener.generated.h"

//=======================================================================================
// Global Defines / Macros
//=======================================================================================

//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// UObject-derived proxy class allowing callbacks from dynamic delegates
UCLASS()
class SKOOKUMSCRIPTRUNTIME_API USkookumScriptListener : public UObject
  {

    GENERATED_UCLASS_BODY()

  public:

  // Types

    struct EventInfo : AListNode<EventInfo>
      {
      SkInstance *  m_argument_p[8];
      };

    typedef void (*tUnregisterCallback)(UObject *, USkookumScriptListener *);

  // Public Data Members

  // Methods

    void                initialize(UObject * obj_p, SkInvokedCoroutine * coro_p, tUnregisterCallback callback_p);
    void                deinitialize();

    uint32_t            get_num_arguments() const { return m_num_arguments; }

    bool                has_event() const;
    EventInfo *         pop_event();
    void                free_event(EventInfo * event_p, bool free_arguments);

    // Various event callback functions

    UFUNCTION()
    void OnActorOverlap(AActor * other_actor_p);

    UFUNCTION()
    void OnTakeAnyDamage(float damage, const UDamageType * damage_type_p, AController * instigated_by_p, AActor * damage_causer_p);

    UFUNCTION()
    void OnTakePointDamage(float damage, AController * instigated_by_p, FVector hit_location, UPrimitiveComponent * hit_component_p, FName bone_name, FVector shot_from_direction, const UDamageType * damage_type_p, AActor * damage_causer_p);

    UFUNCTION()
    void OnDestroyed();

  protected:

  // Internal Methods

    static EventInfo *  alloc_event();
    void                push_event_and_resume(EventInfo * event_p, uint32_t num_arguments);

  // Internal Data Members

    FWeakObjectPtr              m_obj_p;                 // UObject we belong to
    AIdPtr<SkInvokedCoroutine>  m_coro_p;                // The coroutine that is suspended waiting for events from this object
    AList<EventInfo>            m_event_queue;           // Queued up events waiting to be processed
    uint32_t                    m_num_arguments;         // How many arguments the event has
    tUnregisterCallback         m_unregister_callback_p; // How to unregister myself from the delegate list I am hooked up to

  };  // USkookumScriptListener

//---------------------------------------------------------------------------------------
// Helper class to hold a pointer to a USkookumScriptListener
// and make sure it gets destroyed when the pointer goes away
class FSkookumScriptListenerAutoPtr : public TWeakObjectPtr<USkookumScriptListener>
  {
  public:

    FSkookumScriptListenerAutoPtr(const USkookumScriptListener * delegate_obj_p) : TWeakObjectPtr<USkookumScriptListener>(delegate_obj_p) {}
    ~FSkookumScriptListenerAutoPtr();
  };

//---------------------------------------------------------------------------------------
// Storage specialization
template<> inline FSkookumScriptListenerAutoPtr * SkUserDataBase::as<FSkookumScriptListenerAutoPtr>() const { return as_stored<FSkookumScriptListenerAutoPtr>(); }

//=======================================================================================
// Inline Functions
//=======================================================================================

//---------------------------------------------------------------------------------------

inline bool USkookumScriptListener::has_event() const
  {
  return !m_event_queue.is_empty();
  }

//---------------------------------------------------------------------------------------

inline USkookumScriptListener::EventInfo * USkookumScriptListener::pop_event()
  {
  return m_event_queue.pop_first();
  }
