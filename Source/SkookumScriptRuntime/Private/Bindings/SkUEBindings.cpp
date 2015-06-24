//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript Unreal Engine Bindings
// 
// Author: Markus Breyer
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include "../SkookumScriptRuntimePrivatePCH.h"
#include "SkUEBindings.hpp"

#include "VectorMath/SkVector2.hpp"
#include "VectorMath/SkVector3.hpp"
#include "VectorMath/SkVector4.hpp"
#include "VectorMath/SkRotation.hpp"
#include "VectorMath/SkRotationAngles.hpp"
#include "VectorMath/SkTransform.hpp"
#include "VectorMath/SkColor.hpp"

#include "Engine/SkUEName.hpp"
#include "Engine/SkUEActor.hpp"
#include "Engine/SkUEEntity.hpp"
#include "Engine/SkUEEntityClass.hpp"

//=======================================================================================
// Engine-Generated
//=======================================================================================

#include <SkUE.generated.inl> // Massive code file containing thousands of generated bindings

//=======================================================================================
// SkUEBindings Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Registers bindings for SkookumScript
void SkUEBindings::register_all()
  {
  // VectorMath Overlay
  SkVector2::register_bindings();
  SkVector3::register_bindings();
  SkVector4::register_bindings();
  SkRotation::register_bindings();
  SkRotationAngles::register_bindings();
  SkTransform::register_bindings();
  SkColor::register_bindings();

  // Engine-Generated Overlay
  SkUE::register_bindings();
  gather_dynamic_classes_recursively(SkUEEntity::ms_class_p);

  // Engine Overlay
  SkUEEntity_Ext::register_bindings();
  SkUEEntityClass_Ext::register_bindings();
  SkUEActor_Ext::register_bindings();
  SkUEName::register_bindings();
  }

//---------------------------------------------------------------------------------------
// Detect dynamic Blueprint classes unknown to SkookumScriptGenerator
// and add to the Sk<>UE class map
void SkUEBindings::gather_dynamic_classes_recursively(SkClass * class_p)
  {
  // Look it up, that will add it to the map if yet unknown
  SkUEClassBindingHelper::get_ue_class_from_sk_class(class_p);

  // Gather sub classes
  const tSkClasses & sub_classes = class_p->get_subclasses();
  for (uint32_t i = 0; i < sub_classes.get_length(); ++i)
    {
    gather_dynamic_classes_recursively(sub_classes(i));
    }
  }
