//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript color class
//
// Author: Markus Breyer
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include "../../SkookumScriptRuntimePrivatePCH.h"
#include "SkColor.hpp"

//=======================================================================================
// Method Definitions
//=======================================================================================

namespace SkColor_Impl
  {

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@!rgba(Real r, Real g, Real b, Real a) Color
  // # Author(s): Markus Breyer
  static void mthd_ctor_rgba(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Results are ignored for constructors
    scope_p->get_this()->construct<SkColor>(
      scope_p->get_arg<SkReal>(SkArg_1),
      scope_p->get_arg<SkReal>(SkArg_2),
      scope_p->get_arg<SkReal>(SkArg_3),
      scope_p->get_arg<SkReal>(SkArg_4));
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@!rgb(Real r, Real g, Real b) Color
  // # Author(s): Markus Breyer
  static void mthd_ctor_rgb(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Results are ignored for constructors
    scope_p->get_this()->construct<SkColor>(
      scope_p->get_arg<SkReal>(SkArg_1),
      scope_p->get_arg<SkReal>(SkArg_2),
      scope_p->get_arg<SkReal>(SkArg_3));
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@String() String
  // # Author(s): Markus Breyer
  static void mthd_String(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      const FLinearColor & color = scope_p->this_as<SkColor>();
      AString str(128u, "(R=%g, G=%g, B=%g, A=%g)", double(color.R), double(color.G), double(color.B), double(color.A));

      *result_pp = SkString::new_instance(str);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@=(Color col) Boolean
  // # Author(s): Markus Breyer
  static void mthd_op_equals(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkBoolean::new_instance(scope_p->this_as<SkColor>() == scope_p->get_arg<SkColor>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@~=(Color col) Boolean
  // # Author(s): Markus Breyer
  static void mthd_op_not_equal(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkBoolean::new_instance(scope_p->this_as<SkColor>() != scope_p->get_arg<SkColor>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@+(Color col) Color
  // # Author(s): Markus Breyer
  static void mthd_op_add(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkColor::new_instance(scope_p->this_as<SkColor>() + scope_p->get_arg<SkColor>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@+=(Color col) Color
  // # Author(s): Markus Breyer
  static void mthd_op_add_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkColor>() += scope_p->get_arg<SkColor>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@-(Color col) Color
  // # Author(s): Markus Breyer
  static void mthd_op_subtract(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkColor::new_instance(scope_p->this_as<SkColor>() - scope_p->get_arg<SkColor>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@-=(Color col) Color
  // # Author(s): Markus Breyer
  static void mthd_op_subtract_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkColor>() -= scope_p->get_arg<SkColor>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@*(Real num) Color
  // # Author(s): Markus Breyer
  static void mthd_op_multiply(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      FLinearColor vector(scope_p->this_as<SkColor>());

      vector *= scope_p->get_arg<SkReal>(SkArg_1);

      *result_pp = SkColor::new_instance(vector);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@*=(Real num) Color
  // # Author(s): Markus Breyer
  static void mthd_op_multiply_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkColor>() *= scope_p->get_arg<SkReal>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@/(Real num) Color
  // # Author(s): Markus Breyer
  static void mthd_op_divide(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkColor::new_instance(scope_p->this_as<SkColor>() / scope_p->get_arg<SkReal>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@/=(Real num) Color
  // # Author(s): Markus Breyer
  static void mthd_op_divide_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkColor>() /= scope_p->get_arg<SkReal>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@r() Real
  // # Author(s): Markus Breyer
  static void mthd_r(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkColor>().R);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@g() Real
  // # Author(s): Markus Breyer
  static void mthd_g(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkColor>().G);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@b() Real
  // # Author(s): Markus Breyer
  static void mthd_b(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkColor>().B);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@a() Real
  // # Author(s): Markus Breyer
  static void mthd_a(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      *result_pp = SkReal::new_instance(scope_p->this_as<SkColor>().A);
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@rgba_set(Real r, Real g, Real b, Real a) Color
  // # Author(s): Markus Breyer
  static void mthd_rgba_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();
    FLinearColor & color = this_p->as<SkColor>();

    color.R = scope_p->get_arg<SkReal>(SkArg_1);
    color.G = scope_p->get_arg<SkReal>(SkArg_2);
    color.B = scope_p->get_arg<SkReal>(SkArg_3);
    color.A = scope_p->get_arg<SkReal>(SkArg_4);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@r_set(Real r) Color
  // # Author(s): Markus Breyer
  static void mthd_r_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkColor>().R = scope_p->get_arg<SkReal>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@g_set(Real g) Color
  // # Author(s): Markus Breyer
  static void mthd_g_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkColor>().G = scope_p->get_arg<SkReal>(SkArg_1);


    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@b_set(Real b) Color
  // # Author(s): Markus Breyer
  static void mthd_b_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkColor>().B = scope_p->get_arg<SkReal>(SkArg_1);

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   Color@a_set(Real a) Color
  // # Author(s): Markus Breyer
  static void mthd_a_set(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();

    this_p->as<SkColor>().A = scope_p->get_arg<SkReal>(SkArg_1);

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
      { "!rgba",           mthd_ctor_rgba },
      { "!rgb",            mthd_ctor_rgb },

      { "String",          mthd_String },

      { "equal?",          mthd_op_equals },
      { "not_equal?",      mthd_op_not_equal },
      { "add",             mthd_op_add },
      { "add_assign",      mthd_op_add_assign },
      { "subtract",        mthd_op_subtract },
      { "subtract_assign", mthd_op_subtract_assign },
      { "multiply",        mthd_op_multiply },
      { "multiply_assign", mthd_op_multiply_assign },
      { "divide",          mthd_op_divide },
      { "divide_assign",   mthd_op_divide_assign },

      { "r",               mthd_r },
      { "g",               mthd_g },
      { "b",               mthd_b },
      { "a",               mthd_a },
      { "rgba_set",        mthd_rgba_set },
      { "r_set",           mthd_r_set },
      { "g_set",           mthd_g_set },
      { "b_set",           mthd_b_set },
      { "a_set",           mthd_a_set },
    };

  } // namespace

//---------------------------------------------------------------------------------------

void SkColor::register_bindings()
  {
  tBindingBase::register_bindings("Color");

  ms_class_p->register_method_func_bulk(SkColor_Impl::methods_i, A_COUNT_OF(SkColor_Impl::methods_i), SkBindFlag_instance_no_rebind);
  }
