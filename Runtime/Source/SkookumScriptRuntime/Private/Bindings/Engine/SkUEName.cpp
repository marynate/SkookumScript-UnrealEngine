//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001-2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript Name (= FName) class
//
// Author: Markus Breyer
//=======================================================================================

//=======================================================================================
// Includes
//=======================================================================================

#include "../../SkookumScriptRuntimePrivatePCH.h"
#include "SkUEName.hpp"
#include <SkookumScript/SkString.hpp>

//=======================================================================================
// Method Definitions
//=======================================================================================

namespace SkUEName_Impl
  {

  //---------------------------------------------------------------------------------------
  // # Skookum:   Name@String() String
  // # Author(s): Markus Breyer
  static void mthd_String(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      const FName & name = scope_p->this_as<SkUEName>();
      *result_pp = SkString::new_instance(AString(name.GetPlainANSIString()));
      }
    }

  //---------------------------------------------------------------------------------------
  // # Skookum:   String@Name() Name
  // # Author(s): Markus Breyer
  static void mthd_String_to_Name(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    // Do nothing if result not desired
    if (result_pp)
      {
      const AString & str = scope_p->this_as<SkString>();
      *result_pp = SkUEName::new_instance(FName(str.as_cstr()));
      }
    }

  } // namespace

//---------------------------------------------------------------------------------------
void SkUEName::register_bindings()
  {
  tBindingBase::register_bindings("Name");
  ms_class_p->register_method_func(ASymbol_String, SkUEName_Impl::mthd_String);

  // Hook up extra String methods
  SkString::ms_class_p->register_method_func("Name", SkUEName_Impl::mthd_String_to_Name);
  }

