//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript Actor Component
// 
// Author: Conan Reis, Markus Breyer
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include "SkookumScriptRuntimePrivatePCH.h"
#include "../Classes/SkookumScriptComponent.h"
#include "Bindings/SkUEBindings.hpp"
#include "SkUEActor.generated.hpp"


//=======================================================================================
// Class Data
//=======================================================================================


//=======================================================================================
// Method Definitions
//=======================================================================================

//---------------------------------------------------------------------------------------
USkookumScriptComponent::USkookumScriptComponent(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  {
  PrimaryComponentTick.bCanEverTick = false;
  bTickInEditor = false;
  bAutoActivate = true;
  bWantsInitializeComponent = true;

  //SkDebug::print_ide(a_str_format("Actor '%S' Ctor() 0x%p\n", *GetOwner()->GetName(), GetOwner()), SkLocale_all, SkDPrintType_trace);
  //SkDebug::print_ide(a_str_format("USkookumScriptComponent::USkookumScriptComponent() 0x%p\n", this), SkLocale_ide, SkDPrintType_trace);
  }

//---------------------------------------------------------------------------------------
void USkookumScriptComponent::OnRegister()
  {
  Super::OnRegister();

  if (!m_instance_p)
    {
    // Find the actor I belong to
    AActor * actor_p = GetOwner();
    SK_ASSERTX(actor_p, "USkookumScriptComponent must be attached to an actor.");

    // Determine SkookumScript class of my actor
    SkClass * class_p = nullptr;
    FString class_name = ScriptClassName;
    if (!class_name.IsEmpty())
      {
      AString class_name_ascii(*class_name, class_name.Len());
      class_p = SkBrain::get_class(class_name_ascii.as_cstr());
      SK_ASSERTX(class_p, a_cstr_format("Cannot find Script Class Name '%s' specified in SkookumScriptComponent of '%S'. Misspelled?", class_name_ascii.as_cstr(), *actor_p->GetName()));
      if (!class_p) goto set_default_class; // Recover from bad user input

      // Do some extra checking in non-shipping builds
      #if (SKOOKUM & SK_DEBUG)
        // Find most derived SkookumScript class known to UE4
        SkClass * known_super_class_p;
        for (known_super_class_p = class_p; known_super_class_p; known_super_class_p = known_super_class_p->get_superclass())
          {
          if (SkUEClassBindingHelper::ms_class_map_s2u.Find(known_super_class_p)) break;
          }

        // Find most derived UE4 class known to SkookumScript
        UClass * obj_uclass_p = actor_p->GetClass();
        SkClass ** class_pp = SkUEClassBindingHelper::ms_class_map_u2s.Find(actor_p->GetClass());
        for (; !class_pp && obj_uclass_p; obj_uclass_p = obj_uclass_p->GetSuperClass())
          {
          class_pp = SkUEClassBindingHelper::ms_class_map_u2s.Find(obj_uclass_p);
          }
        SK_ASSERTX(class_pp && *class_pp == known_super_class_p, a_cstr_format("Script Class Name '%s' in SkookumScriptComponent of '%S' is not properly related to Actor. Both the Script Class Name '%s' and the UE4 class of '%S' ('%S') must share the topmost ancestor class known to both SkookumScript and UE4. Right now these ancestor classes are different ('%s' for '%s' and '%s' for '%S').", class_name_ascii.as_cstr(), *actor_p->GetName(), class_name_ascii.as_cstr(), *actor_p->GetName(), *actor_p->GetClass()->GetName(), known_super_class_p ? known_super_class_p->get_name_cstr_dbg() : "<none>", class_name_ascii.as_cstr(), class_pp ? (*class_pp)->get_name_cstr_dbg() : "<none>", *actor_p->GetClass()->GetName()));
      #endif
      }
    else
      {
    set_default_class:
      // Find most derived UE4 class known to SkookumScript
      UClass * obj_uclass_p = actor_p->GetClass();
      SkClass ** class_pp = SkUEClassBindingHelper::ms_class_map_u2s.Find(actor_p->GetClass());
      for (; !class_pp && obj_uclass_p; obj_uclass_p = obj_uclass_p->GetSuperClass())
        {
        class_pp = SkUEClassBindingHelper::ms_class_map_u2s.Find(obj_uclass_p);
        }
      SK_ASSERTX(class_pp, a_cstr_format("No parent class of %S is known to SkookumScript!", *obj_uclass_p->GetName()));
      class_p = class_pp ? *class_pp : SkBrain::get_class(ASymbol_Actor); // Recover from bad user input
      }

    // Based on the desired class, create SkInstance or SkDataInstance
    SkInstance * instance_p = class_p->new_instance();
    instance_p->construct<SkUEActor>(actor_p); // Keep track of owner actor
    m_instance_p = instance_p;
    }
  }

//---------------------------------------------------------------------------------------
void USkookumScriptComponent::InitializeComponent()
  {
  Super::InitializeComponent();

  //SkDebug::print_ide(a_str_format("Actor '%S' InitializeComponent() A:%p C:%p\n", *owner_p->GetName(), owner_p, this), SkLocale_all, SkDPrintType_trace);
  //SkDebug::print_ide(a_str_format("USkookumScriptComponent::InitializeComponent() 0x%p\n", this), SkLocale_ide, SkDPrintType_trace);
  m_instance_p->call_default_constructor();
  }

//---------------------------------------------------------------------------------------
void USkookumScriptComponent::UninitializeComponent()
  {
  Super::UninitializeComponent();

  //SkDebug::print_ide(a_str_format("USkookumScriptComponent::UninitializeComponent() 0x%p\n", this), SkLocale_ide, SkDPrintType_trace);
  m_instance_p->call_destructor();
  }

//---------------------------------------------------------------------------------------
void USkookumScriptComponent::OnUnregister()
  {
  Super::OnUnregister();

  //SkDebug::print_ide(a_str_format("USkookumScriptComponent::OnUnregister() 0x%p\n", this), SkLocale_ide, SkDPrintType_trace);
  m_instance_p->dereference();
  }

//---------------------------------------------------------------------------------------
void USkookumScriptComponent::invoke_coroutine(FString name)
  {
  //SkDebug::print_ide(a_str_format("USkookumScriptComponent::invoke_coroutine(%S)\n", *name), SkLocale_ide, SkDPrintType_trace);
  m_instance_p->coroutine_call(FStringToASymbol(name), nullptr, false);
  }

//---------------------------------------------------------------------------------------
void USkookumScriptComponent::invoke_method(FString name)
  {
  //SkDebug::print_ide(a_str_format("USkookumScriptComponent::invoke_method(%S)\n", *name), SkLocale_ide, SkDPrintType_trace);
  m_instance_p->method_call(FStringToASymbol(name));
  }

//---------------------------------------------------------------------------------------
bool USkookumScriptComponent::invoke_query(FString name)
  {
  //SkDebug::print_ide(a_str_format("USkookumScriptComponent::invoke_query(%S)\n", *name), SkLocale_ide, SkDPrintType_trace);
  return m_instance_p->method_query(FStringToASymbol(name));
  }

