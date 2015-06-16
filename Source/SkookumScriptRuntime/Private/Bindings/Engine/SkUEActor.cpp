//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// Bindings for the Actor (= AActor) class 
//
// Author: Markus Breyer
//=======================================================================================

//=======================================================================================
// Includes
//=======================================================================================

#include "../../SkookumScriptRuntimePrivatePCH.h"
#include "SkUEActor.hpp"
#include "SkUEEntity.hpp"
#include "../SkUERuntime.hpp"

//=======================================================================================
// Method Definitions
//=======================================================================================

namespace SkUEActor_Impl
  {

  //---------------------------------------------------------------------------------------
  // Helper function
  static UClass * get_actor_array(TArray<UObject*> * object_array_p, SkClass * class_p)
    {
    UClass ** uclass_pp = SkUEClassBindingHelper::ms_class_map_s2u.Find(class_p);
    SK_ASSERTX(uclass_pp, a_cstr_format("Class '%s' not found in ms_class_map_s2u. All actor classes must be present.", class_p->get_name_cstr_dbg()));
    if (!uclass_pp) { return nullptr; }
    EObjectFlags exclude_flags = RF_ClassDefaultObject | RF_PendingKill;
    object_array_p->Reserve(1024);
    GetObjectsOfClass(*uclass_pp, *object_array_p, true, exclude_flags);
    return *uclass_pp;
    }

  //---------------------------------------------------------------------------------------
  // Actor@find_named(Name name) <ThisClass_|None>
  static void mthdc_find_named(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    if (result_pp) // Do nothing if result not desired
      {      
      // Get actor array
      TArray<UObject*> object_array;
      SkClass * class_p = &((SkMetaClass *)scope_p->get_topmost_scope())->get_class_info();
      UClass * uclass_p = get_actor_array(&object_array, class_p);

      // Find our actor
      FString name = AStringToFString(scope_p->get_arg<SkString>(SkArg_1));
      UWorld * world_p = SkUEClassBindingHelper::get_world();
      AActor * actor_p = nullptr;
      for (UObject ** RESTRICT obj_pp = object_array.GetData(), **RESTRICT end_pp = obj_pp + object_array.Num(); obj_pp != end_pp; ++obj_pp)
        {
        if ((*obj_pp)->GetWorld() == world_p && (*obj_pp)->GetName() == name)
          {
          actor_p = static_cast<AActor *>(*obj_pp);
          break;
          }
        }

      // Create instance from our actor
      *result_pp = actor_p ? SkUEActor::new_instance(actor_p, uclass_p, class_p) : SkBrain::ms_nil_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // Actor@named(Name name) <ThisClass_>
  static void mthdc_named(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    mthdc_find_named(scope_p, result_pp);
  
    #if (SKOOKUM & SK_DEBUG)
      if (result_pp && *result_pp == SkBrain::ms_nil_p)
        {
        A_ERRORX(a_str_format("Tried to get instance named '%s' from class '%s', but no such instance exists!\n", scope_p->get_arg<SkString>(SkArg_1).as_cstr(), ((SkMetaClass *)scope_p->get_topmost_scope())->get_class_info().get_name().as_cstr_dbg()));
        }
    #endif
    }

  //---------------------------------------------------------------------------------------
  // Actor@() List{ThisClass_}
  static void mthdc_instances(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    if (result_pp) // Do nothing if result not desired
      {
      // Get actor array
      TArray<UObject*> object_array;
      SkClass * class_p = &((SkMetaClass *)scope_p->get_topmost_scope())->get_class_info();
      UClass * uclass_p = get_actor_array(&object_array, class_p);

      // Build SkList from it
      UWorld * world_p = SkUEClassBindingHelper::get_world();
      SkInstance * instance_p = SkList::new_instance(object_array.Num());
      SkInstanceList & list = instance_p->as<SkList>();
      APArray<SkInstance> & instances = list.get_instances();
      for (UObject ** RESTRICT obj_pp = object_array.GetData(), **RESTRICT end_pp = obj_pp + object_array.Num(); obj_pp != end_pp; ++obj_pp)
        {
        if ((*obj_pp)->GetWorld() == world_p)
          {
          // This instance is already refcounted so directly append to underlying array
          instances.append(*SkUEActor::new_instance(static_cast<AActor *>(*obj_pp), uclass_p, class_p));
          }
        }
      *result_pp = instance_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // Actor@() ThisClass_
  static void mthdc_instances_first(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    if (result_pp) // Do nothing if result not desired
      {
      // Get actor array
      TArray<UObject*> object_array;
      SkClass * class_p = &((SkMetaClass *)scope_p->get_topmost_scope())->get_class_info();
      UClass * uclass_p = get_actor_array(&object_array, class_p);

      // Return first one
      UWorld * world_p = SkUEClassBindingHelper::get_world();
      for (UObject ** RESTRICT obj_pp = object_array.GetData(), **RESTRICT end_pp = obj_pp + object_array.Num(); obj_pp != end_pp; ++obj_pp)
        {
        if ((*obj_pp)->GetWorld() == world_p)
          {
          *result_pp = SkUEActor::new_instance(static_cast<AActor *>(*obj_pp), uclass_p, class_p);
          return;
          }
        }

      // None found
      *result_pp = SkBrain::ms_nil_p;
      }
    }


  //---------------------------------------------------------------------------------------
  // Event handler template parameter for handling ActorBeginOverlap events
  class EventHandler_OnActorBeginOverlap
    {
    public:
      // Add callback to object
      static void install(UObject * obj_p, USkookumScriptListener * listener_p)
        {
        Cast<AActor>(obj_p)->OnActorBeginOverlap.AddDynamic(listener_p, &USkookumScriptListener::OnActorOverlap);
        #if (SKOOKUM & SK_DEBUG)
          // Check that events will properly fire
          UPrimitiveComponent * component_p = Cast<UPrimitiveComponent>(Cast<AActor>(obj_p)->GetComponentByClass(UPrimitiveComponent::StaticClass()));
          SK_ASSERTX(component_p, a_cstr_format("Trying to receive overlap events on actor '%S' but it has no primitive (collision) component.", *obj_p->GetName()));
          SK_ASSERTX(component_p->bGenerateOverlapEvents, a_cstr_format("Trying to receive overlap events on actor '%S' but it has overlap events turned off. To fix this, check the box 'Generate Overlap Events' for the %S of '%S'. You might also simply have picked the wrong actor.", *obj_p->GetName(), *component_p->GetClass()->GetName(), *obj_p->GetName()));
        #endif
        }
      // Remove callback from object
      static void uninstall(UObject * obj_p, USkookumScriptListener * listener_p)
        {
        Cast<AActor>(obj_p)->OnActorBeginOverlap.RemoveDynamic(listener_p, &USkookumScriptListener::OnActorOverlap);
        }
    };

  //---------------------------------------------------------------------------------------
  // Event handler template parameter for handling ActorEndOverlap events
  class EventHandler_OnActorEndOverlap
    {
    public:
      // Add callback to object
      static void install(UObject * obj_p, USkookumScriptListener * listener_p)
        {
        Cast<AActor>(obj_p)->OnActorEndOverlap.AddDynamic(listener_p, &USkookumScriptListener::OnActorOverlap);
        #if (SKOOKUM & SK_DEBUG)
          // Check that events will properly fire
          UPrimitiveComponent * component_p = Cast<UPrimitiveComponent>(Cast<AActor>(obj_p)->GetComponentByClass(UPrimitiveComponent::StaticClass()));
          SK_ASSERTX(component_p, a_cstr_format("Trying to receive overlap events on actor '%S' but it has no primitive (collision) component.", *obj_p->GetName()));
          SK_ASSERTX(component_p->bGenerateOverlapEvents, a_cstr_format("Trying to receive overlap events on actor '%S' but it has overlap events turned off. To fix this, check the box 'Generate Overlap Events' for the %S of '%S'. You might also simply have picked the wrong actor.", *obj_p->GetName(), *component_p->GetClass()->GetName(), *obj_p->GetName()));
        #endif
        }
      // Remove callback from object
      static void uninstall(UObject * obj_p, USkookumScriptListener * listener_p)
        {
        Cast<AActor>(obj_p)->OnActorEndOverlap.RemoveDynamic(listener_p, &USkookumScriptListener::OnActorOverlap);
        }
    };

  //---------------------------------------------------------------------------------------
  // Event handler template parameter for handling TakeAnyDamage events
  class EventHandler_OnTakeAnyDamage
    {
    public:
      // Add callback to object
      static void install(UObject * obj_p, USkookumScriptListener * listener_p)
        {
        Cast<AActor>(obj_p)->OnTakeAnyDamage.AddDynamic(listener_p, &USkookumScriptListener::OnTakeAnyDamage);
        }
      // Remove callback from object
      static void uninstall(UObject * obj_p, USkookumScriptListener * listener_p)
        {
        Cast<AActor>(obj_p)->OnTakeAnyDamage.RemoveDynamic(listener_p, &USkookumScriptListener::OnTakeAnyDamage);
        }
    };

  //---------------------------------------------------------------------------------------
  // Event handler template parameter for handling TakePointDamage events
  class EventHandler_OnTakePointDamage
    {
    public:
      // Add callback to object
      static void install(UObject * obj_p, USkookumScriptListener * listener_p)
        {
        Cast<AActor>(obj_p)->OnTakePointDamage.AddDynamic(listener_p, &USkookumScriptListener::OnTakePointDamage);
        }
      // Remove callback from object
      static void uninstall(UObject * obj_p, USkookumScriptListener * listener_p)
        {
        Cast<AActor>(obj_p)->OnTakePointDamage.RemoveDynamic(listener_p, &USkookumScriptListener::OnTakePointDamage);
        }
    };

  //---------------------------------------------------------------------------------------
  // Event handler template parameter for handling Destroyed events
  class EventHandler_OnDestroyed
    {
    public:
      // Add callback to object
      static void install(UObject * obj_p, USkookumScriptListener * listener_p)
        {
        Cast<AActor>(obj_p)->OnDestroyed.AddDynamic(listener_p, &USkookumScriptListener::OnDestroyed);
        }
      // Remove callback from object
      static void uninstall(UObject * obj_p, USkookumScriptListener * listener_p)
        {
        Cast<AActor>(obj_p)->OnDestroyed.RemoveDynamic(listener_p, &USkookumScriptListener::OnDestroyed);
        }
    };

  #ifdef _MSC_VER
  #pragma warning(disable : 4127) // Function below contains constant conditionals on purpose
  #endif

  template<class tEventHandler, bool do_until>
  bool coro_on_event(SkInvokedCoroutine * scope_p)
    {
    UObject * this_p = scope_p->this_as<SkUEEntity>();

    // Just started?
    if (scope_p->m_update_count == 0u)
      {
      // Install and store away event listener
      USkookumScriptListener * listener_p = SkookumScriptListenerManager::get_singleton()->alloc_listener(this_p, scope_p, &tEventHandler::uninstall);
      scope_p->append_user_data<FSkookumScriptListenerAutoPtr, USkookumScriptListener *>(listener_p);
      tEventHandler::install(this_p, listener_p);

      // Suspend coroutine
      scope_p->suspend();

      // Coroutine not complete yet - call again when resumed
      return false;
      }

    // Get back stored event listener
    USkookumScriptListener * listener_p = scope_p->get_user_data<FSkookumScriptListenerAutoPtr>()->Get();
    SK_ASSERTX(listener_p->has_event(), "Must have event at this point as coroutine was resumed by delegate object.");

    // Run closure on each event accumulated in the listener
    SkClosure * closure_p = scope_p->get_arg_data<SkClosure>(SkArg_1);
    uint32_t num_arguments = listener_p->get_num_arguments();
    bool exit = false;
    SkInstance * closure_result_p = SkBrain::ms_nil_p;
    SkInstance * return_value_p = SkBrain::ms_nil_p;
    do
      {
      // Use event parameters to invoke closure, then recycle event
      USkookumScriptListener::EventInfo * event_p = listener_p->pop_event();
      if (do_until)
        {
        // Add reference to potential return values so they survive closure_method_call 
        for (uint32_t i = 0; i < num_arguments; ++i)
          {
          event_p->m_argument_p[SkArg_1 + i]->reference();
          }
        }
      closure_p->closure_method_call(&event_p->m_argument_p[0], listener_p->get_num_arguments(), &closure_result_p, scope_p);
      if (do_until)
        {
        exit = closure_result_p->as<SkBoolean>();        
        for (uint32_t i = 0; i < num_arguments; ++i)
          {
          if (exit)
            {
            scope_p->set_arg(SkArg_2 + i, event_p->m_argument_p[i]); // Store parameters as return values if exiting
            }
          else
            {
            event_p->m_argument_p[i]->dereference(); // Dereference parameters if not needed after all
            }
          }
        }
        listener_p->free_event(event_p, false);
      } while (listener_p->has_event() && !exit);

      if (!do_until || !exit)
        {
        // We're not done - wait for more events
        scope_p->suspend();
        return false;
        }

      // Ok done, return event parameters and quit
      return true;
    }

  #ifdef _MSC_VER
  #pragma warning(default : 4127)
  #endif

  //---------------------------------------------------------------------------------------
  // Actor@((Actor actor) code)
  static bool coro_on_begin_overlap_do(SkInvokedCoroutine * scope_p)
    {
    return coro_on_event<EventHandler_OnActorBeginOverlap, false>(scope_p);
    }

  //---------------------------------------------------------------------------------------
  // Actor@((Actor actor) Boolean code; Actor match)
  static bool coro_on_begin_overlap_do_until(SkInvokedCoroutine * scope_p)
    {
    return coro_on_event<EventHandler_OnActorBeginOverlap, true>(scope_p);
    }

  //---------------------------------------------------------------------------------------
  // Actor@((Actor actor) code)
  static bool coro_on_end_overlap_do(SkInvokedCoroutine * scope_p)
    {
    return coro_on_event<EventHandler_OnActorEndOverlap, false>(scope_p);
    }

  //---------------------------------------------------------------------------------------
  // Actor@((Actor actor) Boolean code; Actor match)
  static bool coro_on_end_overlap_do_until(SkInvokedCoroutine * scope_p)
    {
    return coro_on_event<EventHandler_OnActorEndOverlap, true>(scope_p);
    }

  //---------------------------------------------------------------------------------------
  // Actor@((Real damage, DamageType damage_type, Controller instigated_by, Actor damage_causer) code)
  static bool coro_on_take_any_damage_do(SkInvokedCoroutine * scope_p)
    {
    return coro_on_event<EventHandler_OnTakeAnyDamage, false>(scope_p);
    }

  //---------------------------------------------------------------------------------------
  // Actor@((Actor actor) Boolean code; Actor match)
  static bool coro_on_take_any_damage_do_until(SkInvokedCoroutine * scope_p)
    {
    return coro_on_event<EventHandler_OnTakeAnyDamage, true>(scope_p);
    }

  //---------------------------------------------------------------------------------------
  // Actor@((Actor actor) code)
  static bool coro_on_take_point_damage_do(SkInvokedCoroutine * scope_p)
    {
    return coro_on_event<EventHandler_OnTakePointDamage, false>(scope_p);
    }

  //---------------------------------------------------------------------------------------
  // Actor@((Actor actor) Boolean code; Actor match)
  static bool coro_on_take_point_damage_do_until(SkInvokedCoroutine * scope_p)
    {
    return coro_on_event<EventHandler_OnTakePointDamage, true>(scope_p);
    }

  //---------------------------------------------------------------------------------------
  // Actor@(() code)
  static bool coro_on_destroyed_do(SkInvokedCoroutine * scope_p)
    {
    return coro_on_event<EventHandler_OnDestroyed, false>(scope_p);
    }

  static const SkClass::MethodInitializerFunc methods_c2[] =
    {
      { "find_named",       mthdc_find_named },
      { "named",            mthdc_named },
      { "instances",        mthdc_instances },
      { "instances_first",  mthdc_instances_first },
    };

  static const SkClass::CoroutineInitializerFunc coroutines_i[] =
    {
      { "_on_begin_overlap_do", coro_on_begin_overlap_do },
      { "_on_begin_overlap_do_until", coro_on_begin_overlap_do_until },
      { "_on_end_overlap_do", coro_on_end_overlap_do },
      { "_on_end_overlap_do_until", coro_on_end_overlap_do_until },
      { "_on_take_any_damage_do", coro_on_take_any_damage_do },
      { "_on_take_any_damage_do_until", coro_on_take_any_damage_do_until },
      { "_on_take_point_damage_do", coro_on_take_point_damage_do },
      { "_on_take_point_damage_do_until", coro_on_take_point_damage_do_until },
      { "_on_destroyed_do", coro_on_destroyed_do },
    };

  } // SkUEActor_Impl

//---------------------------------------------------------------------------------------

void SkUEActor_Ext::register_bindings()
  {
  ms_class_p->register_method_func_bulk(SkUEActor_Impl::methods_c2, A_COUNT_OF(SkUEActor_Impl::methods_c2), SkBindFlag_class_no_rebind);
  ms_class_p->register_coroutine_func_bulk(SkUEActor_Impl::coroutines_i, A_COUNT_OF(SkUEActor_Impl::coroutines_i), SkBindFlag_instance_no_rebind);
  }


