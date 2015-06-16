//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// Binding classes for UE4 
//
// Author: Markus Breyer
//=======================================================================================

#include "../SkookumScriptRuntimePrivatePCH.h"
#include "SkUEClassBinding.hpp"
#include "../Classes/SkookumScriptComponent.h"
#include <SkUEWorld.generated.hpp>

//---------------------------------------------------------------------------------------

TMap<UClass*, SkClass*> SkUEClassBindingHelper::ms_class_map_u2s;
TMap<SkClass*, UClass*> SkUEClassBindingHelper::ms_class_map_s2u;

//---------------------------------------------------------------------------------------
// Get pointer to UWorld from global variable
UWorld * SkUEClassBindingHelper::get_world()
  {
  static SkTypedData * world_var_p = nullptr;
  if (!world_var_p)
    {
    SkClass * obj_class_p = SkBrain::get_class(ASymbol_Object);
    SK_ASSERTX(obj_class_p, "Couldn't find the World class!");
    uint32_t var_pos;
    if (obj_class_p->get_class_data().find(ASymbolX_c_world, AMatch_first_found, &var_pos))
      {
      world_var_p = obj_class_p->get_class_data().get_at(var_pos);
      }
    else
      {
      SK_ASSERTX(false, "Couldn't find the @@world class member variable!");
      }
    }

  SK_ASSERTX(world_var_p->m_data_p && (world_var_p->m_data_p == SkBrain::ms_nil_p || world_var_p->m_data_p->get_class() == SkBrain::get_class(ASymbol_World)), "@@world variable does not have proper type."); // nil is ok
  return world_var_p->m_data_p == SkBrain::ms_nil_p ? nullptr : world_var_p->m_data_p->as<SkUEWorld>();
  }

//---------------------------------------------------------------------------------------
// Determine SkookumScript class from UClass
SkClass * SkUEClassBindingHelper::get_object_class(UObject * obj_p, UClass * def_uclass_p /*= nullptr*/, SkClass * def_class_p /*= nullptr*/)
  {
  SkClass * class_p = def_class_p;

  if (obj_p)
    {
    UClass * obj_uclass_p = obj_p->GetClass();
    if (obj_uclass_p != def_uclass_p)
      {
      // Crawl up class hierarchy until we find a class known to Sk
      SkClass ** class_pp = nullptr;
      for (; !class_pp && obj_uclass_p; obj_uclass_p = obj_uclass_p->GetSuperClass())
        {
        class_pp = ms_class_map_u2s.Find(obj_uclass_p);
        //SK_ASSERTX(class_pp, "Unknown object class!"); // UE classes can be specific to projects and thus not known to Sk
        }
      if (class_pp)
        {
        class_p = *class_pp;
        }
      }
    }

  return class_p;
  }

//---------------------------------------------------------------------------------------
// Return instance of an actor's SkookumScriptComponent, if any
SkInstance * SkUEClassBindingHelper::get_actor_component_instance(AActor * actor_p)
  {
  // If the actor has component, return the instance contained in the component
  if (actor_p)
    {
    USkookumScriptComponent * component_p = static_cast<USkookumScriptComponent *>(actor_p->GetComponentByClass(USkookumScriptComponent::StaticClass()));
    if (component_p)
      {
      return component_p->get_instance();
      }
    }

  return nullptr;
  }

//---------------------------------------------------------------------------------------
// $Revisit MBreyer - move this somewhere else
UProperty * SkUEClassBindingHelper::find_class_property(UClass * class_p, FName property_name)
  {
  for (TFieldIterator<UProperty> property_it(class_p, EFieldIteratorFlags::ExcludeSuper); property_it; ++property_it)
    {
    UProperty * property_p = *property_it;
    if (property_p->GetFName() == property_name)
      {
      return property_p;
      }
    }
  return nullptr;
  }
