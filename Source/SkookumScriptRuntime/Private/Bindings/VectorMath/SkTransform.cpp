//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001-2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript transform (position + rotation + scale) class
//
// Author: Markus Breyer
//=======================================================================================

//=======================================================================================
// Includes
//=======================================================================================

#include "../../SkookumScriptRuntimePrivatePCH.h"
#include "SkTransform.hpp"
#include "SkVector3.hpp"
#include "SkRotation.hpp"

//=======================================================================================
// Method Definitions
//=======================================================================================

namespace SkTransform_Impl
  {

  //---------------------------------------------------------------------------------------
  // # Skookum:   Transform@String() String
  // # Author(s): Markus Breyer
  static void mthd_String(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      const FTransform &  transform = scope_p->this_as<SkTransform>();
      const FVector &     location = transform.GetLocation();
      const FRotator      rotator(transform.GetRotation());
      const FVector &     scale = transform.GetScale3D();
      AString             str(128u, "t=(%g, %g, %g) yaw=%g pitch=%g roll=%g s=(%g, %g, %g)", 
                            double(location.X), double(location.Y), double(location.Z),
                            double(rotator.Yaw), double(rotator.Pitch), double(rotator.Roll),
                            double(scale.X), double(scale.Y), double(scale.Z));

      *result_pp = SkString::new_instance(str);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Transform@position() Vector
  // # Author(s): Markus Breyer
  static void mthd_position(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkVector3::new_instance(scope_p->this_as<SkTransform>().GetLocation());
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Transform@position_set(Vector pos) Transform
  // # Author(s): Markus Breyer
  static void mthd_position_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkTransform>().SetLocation(scope_p->get_arg<SkVector3>(SkArg_1));

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Transform@rotation() Rotation
  // # Author(s): Markus Breyer
  static void mthd_rotation(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkRotation::new_instance(scope_p->this_as<SkTransform>().GetRotation());
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Transform@rotation_set(Rotation rot) Transform
  // # Author(s): Markus Breyer
  static void mthd_rotation_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkTransform>().SetRotation(scope_p->get_arg<SkRotation>(SkArg_1));

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Transform@scale() Vector
  // # Author(s): Markus Breyer
  static void mthd_scale(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkVector3::new_instance(scope_p->this_as<SkTransform>().GetScale3D());
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Transform@scale_set(Vector scale) Transform
  // # Author(s): Markus Breyer
  static void mthd_scale_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkTransform>().SetScale3D(scope_p->get_arg<SkVector3>(SkArg_1));

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Transform@identity() Transform
  // # Author(s): Markus Breyer
  static void mthd_identity(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkTransform>().SetIdentity();

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------

  // Instance method array
  static const SkClass::MethodInitializerFunc methods_i[] =
    {
      { "String",       mthd_String },

      { "position",     mthd_position },
      { "position_set", mthd_position_set },
      { "rotation",     mthd_rotation },
      { "rotation_set", mthd_rotation_set },
      { "scale",        mthd_scale },
      { "scale_set",    mthd_scale_set },

      { "identity",     mthd_identity },
    };

  } // namespace

//---------------------------------------------------------------------------------------

void SkTransform::register_bindings()
  {
  tBindingBase::register_bindings("Transform");

  ms_class_p->register_method_func_bulk(SkTransform_Impl::methods_i, A_COUNT_OF(SkTransform_Impl::methods_i), SkBindFlag_instance_no_rebind);
  }
