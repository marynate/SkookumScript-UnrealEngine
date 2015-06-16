//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript 3D vector class
//
// Author: Markus Breyer
//=======================================================================================

//=======================================================================================
// Includes
//=======================================================================================

#include "../../SkookumScriptRuntimePrivatePCH.h"
#include "SkVector3.hpp"
#include "SkRotation.hpp"
#include "SkTransform.hpp"

//=======================================================================================
// Method Definitions
//=======================================================================================

namespace SkVector3_Impl
  {

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@!xyz(Real x, Real y, Real z) Vector3
  // # Author(s): Markus Breyer
  static void mthd_ctor_xyz(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Results are ignored for constructors
    scope_p->get_this()->construct<SkVector3>(
      scope_p->get_arg<SkReal>(SkArg_1),
      scope_p->get_arg<SkReal>(SkArg_2),
      scope_p->get_arg<SkReal>(SkArg_3));
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@!xy(Real x, Real y) Vector3
  // # Author(s): Markus Breyer
  static void mthd_ctor_xy(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Results are ignored for constructors
    scope_p->get_this()->construct<SkVector3>(
      scope_p->get_arg<SkReal>(SkArg_1),
      scope_p->get_arg<SkReal>(SkArg_2),
      0.0f);
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@String() String
  // # Author(s): Markus Breyer
  static void mthd_String(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      const FVector & vector = scope_p->this_as<SkVector3>();
      AString str(128u, "(%g, %g, %g)", double(vector.X), double(vector.Y), double(vector.Z));

      *result_pp = SkString::new_instance(str);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@=(Vector3 vec) Boolean
  // # Author(s): Markus Breyer
  static void mthd_op_equals(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkBoolean::new_instance(scope_p->this_as<SkVector3>() == scope_p->get_arg<SkVector3>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@~=(Vector3 vec) Boolean
  // # Author(s): Markus Breyer
  static void mthd_op_not_equal(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkBoolean::new_instance(scope_p->this_as<SkVector3>() != scope_p->get_arg<SkVector3>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@+(Vector3 vec) Vector3
  // # Author(s): Markus Breyer
  static void mthd_op_add(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkVector3::new_instance(scope_p->this_as<SkVector3>() + scope_p->get_arg<SkVector3>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@+=(Vector3 vec) Vector3
  // # Author(s): Markus Breyer
  static void mthd_op_add_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector3>() += scope_p->get_arg<SkVector3>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@-(Vector3 vec) Vector3
  // # Author(s): Markus Breyer
  static void mthd_op_subtract(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkVector3::new_instance(scope_p->this_as<SkVector3>() - scope_p->get_arg<SkVector3>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@-=(Vector3 vec) Vector3
  // # Author(s): Markus Breyer
  static void mthd_op_subtract_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector3>() -= scope_p->get_arg<SkVector3>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@*(Real num) Vector3
  // # Author(s): Markus Breyer
  static void mthd_op_multiply(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkVector3::new_instance(scope_p->this_as<SkVector3>() * scope_p->get_arg<SkReal>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@*=(Real num) Vector3
  // # Author(s): Markus Breyer
  static void mthd_op_multiply_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector3>() *= scope_p->get_arg<SkReal>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@/(Real num) Vector3
  // # Author(s): Markus Breyer
  static void mthd_op_divide(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkVector3::new_instance(scope_p->this_as<SkVector3>() / scope_p->get_arg<SkReal>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@/=(Real num) Vector3
  // # Author(s): Markus Breyer
  static void mthd_op_divide_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector3>() /= scope_p->get_arg<SkReal>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@rotate_by(Rotation rot) Vector3
  // # Author(s): Markus Breyer
  static void mthd_rotate_by(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      const FVector & vec = scope_p->this_as<SkVector3>();
      const FQuat & rot = scope_p->get_arg<SkRotation>(SkArg_1);

      *result_pp = SkVector3::new_instance(rot.RotateVector(vec));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@unrotate_by(Rotation rot) Vector3
  // # Author(s): Markus Breyer
  static void mthd_unrotate_by(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      const FVector & vec = scope_p->this_as<SkVector3>();
      const FQuat rot = scope_p->get_arg<SkRotation>(SkArg_1);

      *result_pp = SkVector3::new_instance(rot.Inverse().RotateVector(vec));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@transform_by(Transform xform) Vector3
  // # Author(s): Markus Breyer
  static void mthd_transform_by(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      const FVector & vec = scope_p->this_as<SkVector3>();
      const FTransform & xform = scope_p->get_arg<SkTransform>(SkArg_1);

      *result_pp = SkVector3::new_instance(xform.TransformPosition(vec));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@untransform_by(Transform xform) Vector3
  // # Author(s): Markus Breyer
  static void mthd_untransform_by(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      const FVector & vec = scope_p->this_as<SkVector3>();
      const FTransform & xform = scope_p->get_arg<SkTransform>(SkArg_1);

      *result_pp = SkVector3::new_instance(xform.InverseTransformPosition(vec));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@x() Real
  // # Author(s): Markus Breyer
  static void mthd_x(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkVector3>().X);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@y() Real
  // # Author(s): Markus Breyer
  static void mthd_y(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkVector3>().Y);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@z() Real
  // # Author(s): Markus Breyer
  static void mthd_z(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkVector3>().Z);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@xyz_set(Real x, Real y, Real z) Vector3
  // # Author(s): Markus Breyer
  static void mthd_xyz_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();
    FVector & vec = this_p->as<SkVector3>();

    vec.X = scope_p->get_arg<SkReal>(SkArg_1);
    vec.Y = scope_p->get_arg<SkReal>(SkArg_2);
    vec.Z = scope_p->get_arg<SkReal>(SkArg_3);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@x_set(Real x) Vector3
  // # Author(s): Markus Breyer
  static void mthd_x_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector3>().X = scope_p->get_arg<SkReal>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@y_set(Real y) Vector3
  // # Author(s): Markus Breyer
  static void mthd_y_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector3>().Y = scope_p->get_arg<SkReal>(SkArg_1);


    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@z_set(Real z) Vector3
  // # Author(s): Markus Breyer
  static void mthd_z_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector3>().Z = scope_p->get_arg<SkReal>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@zero() Vector3
  // # Author(s): Markus Breyer
  static void mthd_zero(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector3>().Set(0.0f, 0.0f, 0.0f);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@zero?() Boolean
  // # Author(s): Markus Breyer
  static void mthd_zeroQ(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkBoolean::new_instance(scope_p->this_as<SkVector3>().IsZero());
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@cross(Vector3 vec) Vector3
  // # Author(s): Markus Breyer
  static void mthd_cross(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkVector3::new_instance(FVector::CrossProduct(
        scope_p->this_as<SkVector3>(),
        scope_p->get_arg<SkVector3>(SkArg_1)));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@distance(Vector3 vec) Real
  // # Author(s): Markus Breyer
  static void mthd_distance(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(FVector::Dist(
        scope_p->this_as<SkVector3>(),
        scope_p->get_arg<SkVector3>(SkArg_1)));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@distance_squared(Vector3 vec) Real
  // # Author(s): Markus Breyer
  static void mthd_distance_squared(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(FVector::DistSquared(
        scope_p->this_as<SkVector3>(),
        scope_p->get_arg<SkVector3>(SkArg_1)));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@dot(Vector3 vec) Real
  // # Author(s): Markus Breyer
  static void mthd_dot(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(FVector::DotProduct(
        scope_p->this_as<SkVector3>(),
        scope_p->get_arg<SkVector3>(SkArg_1)));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@length() Real
  // # Author(s): Markus Breyer
  static void mthd_length(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkVector3>().Size());
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@length_squared() Real
  // # Author(s): Markus Breyer
  static void mthd_length_squared(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkVector3>().SizeSquared());
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@near?(Vector3 vector, Real epsilon_sq -> 0.0025) Boolean
  // # Author(s): Markus Breyer
  static void mthd_nearQ(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkBoolean::new_instance(
        FVector::DistSquared(scope_p->this_as<SkVector3>(), scope_p->get_arg<SkVector3>(SkArg_1))
          <= scope_p->get_arg<SkReal>(SkArg_2));
      }
    }

  /*
  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@angle(Vector3 vec) Real
  // # Author(s): Markus Breyer
  static void mthd_angle(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(qAngleBetween(
        scope_p->this_as<SkVector3>(),
        scope_p->get_arg<SkVector3>(SkArg_1)));
      }
    }
 
  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector3@normalize(Real length -> 1.0) Vector3
  // # Author(s): Markus Breyer
  static void mthd_normalize(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();
    FVector & vec = this_p->as<SkVector3>();

    vec = qNormalize(vec, scope_p->get_arg<SkReal>(SkArg_1));

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }
  */

  //---------------------------------------------------------------------------------------

  // Instance method array
  static const SkClass::MethodInitializerFunc methods_i[] =
    {
      { "!xyz",             mthd_ctor_xyz },
      { "!xy",              mthd_ctor_xy },

      { "String",           mthd_String },

      { "equal?",           mthd_op_equals },
      { "not_equal?",       mthd_op_not_equal },
      { "add",              mthd_op_add },
      { "add_assign",       mthd_op_add_assign },
      { "subtract",         mthd_op_subtract },
      { "subtract_assign",  mthd_op_subtract_assign },
      { "multiply",         mthd_op_multiply },
      { "multiply_assign",  mthd_op_multiply_assign },
      { "divide",           mthd_op_divide },
      { "divide_assign",    mthd_op_divide_assign },

      { "rotate_by",        mthd_rotate_by },
      { "unrotate_by",      mthd_unrotate_by },
      { "transform_by",     mthd_transform_by },
      { "untransform_by",   mthd_untransform_by },

      { "x",                mthd_x },
      { "y",                mthd_y },
      { "z",                mthd_z },
      { "xyz_set",          mthd_xyz_set },
      { "x_set",            mthd_x_set },
      { "y_set",            mthd_y_set },
      { "z_set",            mthd_z_set },
      { "zero?",            mthd_zeroQ },
      { "zero",             mthd_zero },

      { "cross",            mthd_cross },
      { "distance",         mthd_distance },
      { "distance_squared", mthd_distance_squared },
      { "dot",              mthd_dot },
      { "length",           mthd_length },
      { "length_squared",   mthd_length_squared },
      { "near?",            mthd_nearQ },
      //{ "angle",            mthd_angle },
      //{ "normalize",        mthd_normalize },
    };

  } // namespace

//---------------------------------------------------------------------------------------
void SkVector3::register_bindings()
  {
  tBindingBase::register_bindings("Vector3");

  ms_class_p->register_method_func_bulk(SkVector3_Impl::methods_i, A_COUNT_OF(SkVector3_Impl::methods_i), SkBindFlag_instance_no_rebind);
  }
