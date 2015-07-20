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
// Data
//=======================================================================================

// HACK - to fix linker error - remove this if it causes trouble
#if !IS_MONOLITHIC
  namespace FNavigationSystem
    {
    const float FallbackAgentRadius = 35.f;
    const float FallbackAgentHeight = 144.f;
    }
#endif

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

  // Engine Overlay
  SkUEEntity_Ext::register_bindings();
  SkUEEntityClass_Ext::register_bindings();
  SkUEActor_Ext::register_bindings();
  SkUEName::register_bindings();
  }
