//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript 4D vector class
//
// Author: Markus Breyer
//=======================================================================================

//=======================================================================================
// Includes
//=======================================================================================

#include "../../SkookumScriptRuntimePrivatePCH.h"
#include "SkVector4.hpp"

//=======================================================================================
// Method Definitions
//=======================================================================================

namespace SkVector4_Impl
  {

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@!xyzw(Real x, Real y, Real z, Real w) Vector4
  // # Author(s): Markus Breyer
  static void mthd_ctor_xyzw(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Results are ignored for constructors
    scope_p->get_this()->construct<SkVector4>(
      scope_p->get_arg<SkReal>(SkArg_1),
      scope_p->get_arg<SkReal>(SkArg_2),
      scope_p->get_arg<SkReal>(SkArg_3),
      scope_p->get_arg<SkReal>(SkArg_4));
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@!xyz(Real x, Real y, Real z) Vector4
  // # Author(s): Markus Breyer
  static void mthd_ctor_xyz(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Results are ignored for constructors
    scope_p->get_this()->construct<SkVector4>(
      scope_p->get_arg<SkReal>(SkArg_1),
      scope_p->get_arg<SkReal>(SkArg_2),
      scope_p->get_arg<SkReal>(SkArg_3),
      0.0f);
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@!xy(Real x, Real y) Vector4
  // # Author(s): Markus Breyer
  static void mthd_ctor_xy(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Results are ignored for constructors
    scope_p->get_this()->construct<SkVector4>(
      scope_p->get_arg<SkReal>(SkArg_1),
      scope_p->get_arg<SkReal>(SkArg_2),
      0.0f, 0.0f);
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@String() String
  // # Author(s): Markus Breyer
  static void mthd_String(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      const FVector4 & vector = scope_p->this_as<SkVector4>();
      AString str(128u, "(%g, %g, %g, %g)", double(vector.X), double(vector.Y), double(vector.Z), double(vector.W));

      *result_pp = SkString::new_instance(str);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@=(Vector4 vec) Boolean
  // # Author(s): Markus Breyer
  static void mthd_op_equals(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkBoolean::new_instance(scope_p->this_as<SkVector4>() == scope_p->get_arg<SkVector4>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@~=(Vector4 vec) Boolean
  // # Author(s): Markus Breyer
  static void mthd_op_not_equal(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkBoolean::new_instance(scope_p->this_as<SkVector4>() != scope_p->get_arg<SkVector4>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@+(Vector4 vec) Vector4
  // # Author(s): Markus Breyer
  static void mthd_op_add(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkVector4::new_instance(scope_p->this_as<SkVector4>() + scope_p->get_arg<SkVector4>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@+=(Vector4 vec) Vector4
  // # Author(s): Markus Breyer
  static void mthd_op_add_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector4>() += scope_p->get_arg<SkVector4>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@-(Vector4 vec) Vector4
  // # Author(s): Markus Breyer
  static void mthd_op_subtract(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkVector4::new_instance(scope_p->this_as<SkVector4>() - scope_p->get_arg<SkVector4>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@-=(Vector4 vec) Vector4
  // # Author(s): Markus Breyer
  static void mthd_op_subtract_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector4>() = this_p->as<SkVector4>() - scope_p->get_arg<SkVector4>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@*(Real num) Vector4
  // # Author(s): Markus Breyer
  static void mthd_op_multiply(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkVector4::new_instance(scope_p->this_as<SkVector4>() * scope_p->get_arg<SkReal>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@*=(Real num) Vector4
  // # Author(s): Markus Breyer
  static void mthd_op_multiply_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector4>() *= scope_p->get_arg<SkReal>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@/(Real num) Vector4
  // # Author(s): Markus Breyer
  static void mthd_op_divide(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkVector4::new_instance(scope_p->this_as<SkVector4>() / scope_p->get_arg<SkReal>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@/=(Real num) Vector4
  // # Author(s): Markus Breyer
  static void mthd_op_divide_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector4>() = this_p->as<SkVector4>() / scope_p->get_arg<SkReal>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@x() Real
  // # Author(s): Markus Breyer
  static void mthd_x(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkVector4>().X);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@y() Real
  // # Author(s): Markus Breyer
  static void mthd_y(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkVector4>().Y);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@z() Real
  // # Author(s): Markus Breyer
  static void mthd_z(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkVector4>().Z);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@w() Real
  // # Author(s): Markus Breyer
  static void mthd_w(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkVector4>().W);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@xyzw_set(Real x, Real y, Real z, Real w) Vector4
  // # Author(s): Markus Breyer
  static void mthd_xyzw_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();
    FVector4 & vec = this_p->as<SkVector4>();

    vec.X = scope_p->get_arg<SkReal>(SkArg_1);
    vec.Y = scope_p->get_arg<SkReal>(SkArg_2);
    vec.Z = scope_p->get_arg<SkReal>(SkArg_3);
    vec.W = scope_p->get_arg<SkReal>(SkArg_4);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@x_set(Real x) Vector4
  // # Author(s): Markus Breyer
  static void mthd_x_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector4>().X = scope_p->get_arg<SkReal>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@y_set(Real y) Vector4
  // # Author(s): Markus Breyer
  static void mthd_y_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector4>().Y = scope_p->get_arg<SkReal>(SkArg_1);


    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@z_set(Real z) Vector4
  // # Author(s): Markus Breyer
  static void mthd_z_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector4>().Z = scope_p->get_arg<SkReal>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@w_set(Real w) Vector4
  // # Author(s): Markus Breyer
  static void mthd_w_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkVector4>().W = scope_p->get_arg<SkReal>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@zero() Vector4
  // # Author(s): Markus Breyer
  static void mthd_zero(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();
    FVector4 & vec = this_p->as<SkVector4>();

    vec.X = 0.0f;
    vec.Y = 0.0f;
    vec.Z = 0.0f;
    vec.W = 0.0f;

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@zero?() Boolean
  // # Author(s): Markus Breyer
  static void mthd_zeroQ(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      const FVector4 & vector = scope_p->this_as<SkVector4>();
      *result_pp = SkBoolean::new_instance(vector.X == 0.0f && vector.Y == 0.0f && vector.Z == 0.0f && vector.W == 0.0f);
      }
    }

  /*
  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@distance(Vector4 vec) Real
  // # Author(s): Markus Breyer
  static void mthd_distance(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(FVector4::Dist(
        scope_p->this_as<SkVector4>(),
        scope_p->get_arg<SkVector4>(SkArg_1)));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@distance_squared(Vector4 vec) Real
  // # Author(s): Markus Breyer
  static void mthd_distance_squared(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(FVector4::DistSquared(
        scope_p->this_as<SkVector4>(),
        scope_p->get_arg<SkVector4>(SkArg_1)));
      }
    }
  */

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@dot(Vector4 vec) Real
  // # Author(s): Markus Breyer
  static void mthd_dot(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(Dot4(
        scope_p->this_as<SkVector4>(),
        scope_p->get_arg<SkVector4>(SkArg_1)));
      }
    }

  /*
  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@length() Real
  // # Author(s): Markus Breyer
  static void mthd_length(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkVector4>().Size());
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@length_squared() Real
  // # Author(s): Markus Breyer
  static void mthd_length_squared(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkVector4>().SizeSquared());
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@near?(Vector4 vector, Real epsilon_sq -> 0.0025) Boolean
  // # Author(s): Markus Breyer
  static void mthd_nearQ(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkBoolean::new_instance(
        FVector4::DistSquared(scope_p->this_as<SkVector4>(), scope_p->get_arg<SkVector4>(SkArg_1))
          <= scope_p->get_arg<SkReal>(SkArg_2));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@angle(Vector4 vec) Real
  // # Author(s): Markus Breyer
  static void mthd_angle(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(qAngleBetween(
        scope_p->this_as<SkVector4>(),
        scope_p->get_arg<SkVector4>(SkArg_1)));
      }
    }
 
  //---------------------------------------------------------------------------------------
  // # Skookum:   Vector4@normalize(Real length -> 1.0) Vector4
  // # Author(s): Markus Breyer
  static void mthd_normalize(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();
    FVector4 *    vec_p  = &this_p->as<SkVector4>();

    *vec_p = qNormalize(*vec_p, scope_p->get_arg<SkReal>(SkArg_1));

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
      { "!xyzw",            mthd_ctor_xyzw },
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

      { "x",                mthd_x },
      { "y",                mthd_y },
      { "z",                mthd_z },
      { "w",                mthd_w },
      { "xyzw_set",         mthd_xyzw_set },
      { "x_set",            mthd_x_set },
      { "y_set",            mthd_y_set },
      { "z_set",            mthd_z_set },
      { "w_set",            mthd_w_set },
      { "zero?",            mthd_zeroQ },
      { "zero",             mthd_zero },

      //{ "distance",         mthd_distance },
      //{ "distance_squared", mthd_distance_squared },
      { "dot",              mthd_dot },
      //{ "length",           mthd_length },
      //{ "length_squared",   mthd_length_squared },
      //{ "near?",            mthd_nearQ },

      //{ "angle",            mthd_angle },
      //{ "normalize",        mthd_normalize },
    };

  } // namespace

//---------------------------------------------------------------------------------------
void SkVector4::register_bindings()
  {
  tBindingBase::register_bindings("Vector4");

  ms_class_p->register_method_func_bulk(SkVector4_Impl::methods_i, A_COUNT_OF(SkVector4_Impl::methods_i), SkBindFlag_instance_no_rebind);
  }
