//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// Additional bindings for the Entity (= UObject) class 
//
// Author: Markus Breyer
//=======================================================================================

//=======================================================================================
// Includes
//=======================================================================================

#include "../../SkookumScriptRuntimePrivatePCH.h"
#include "SkUEEntity.hpp"
#include "SkUEEntityClass.hpp"

//---------------------------------------------------------------------------------------

namespace SkUEEntity_Impl
  {

  //---------------------------------------------------------------------------------------
  // Copy constructor - copies class pointer as well
  void mthd_ctor_copy(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();
    SkInstance * other_p = scope_p->get_arg(SkArg_1);
    this_p->construct<SkUEEntity>(other_p->as<SkUEEntity>());
    this_p->set_class(other_p->get_class());
    }

  //---------------------------------------------------------------------------------------
  // Null constructor - constructs empty object
  void mthd_ctor_null(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();
    this_p->construct<SkUEEntity>();
    }

  //---------------------------------------------------------------------------------------
  // Assignment operator - copies class pointer as well
  void mthd_op_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    SkInstance * this_p = scope_p->get_this();
    SkInstance * other_p = scope_p->get_arg(SkArg_1);
    this_p->as<SkUEEntity>() = other_p->as<SkUEEntity>();
    this_p->set_class(other_p->get_class());

    // Return this if result desired
    if (result_pp)
      {
      this_p->reference();
      *result_pp = this_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // Tests if entity is null
  static void mthd_null_Q(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    if (result_pp) // Do nothing if result not desired
      {
      *result_pp = SkBoolean::new_instance(!scope_p->this_as<SkUEEntity>().is_valid());
      }
    }

  //---------------------------------------------------------------------------------------
  // Convert to String
  static void mthd_String(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {    
    if (result_pp) // Do nothing if result not desired
      {
      UObject * this_p = scope_p->this_as<SkUEEntity>();
      AString obj_name = this_p ? FStringToAString(this_p->GetName()) : "null";
      AString class_name = scope_p->get_this()->get_class()->as_code();
      AString uclass_name = this_p ? FStringToAString(this_p->GetClass()->GetName()) : "null";

      AString str(nullptr, 9u + obj_name.get_length() + class_name.get_length() + uclass_name.get_length(), 0u);
      str.append('"');
      str.append(obj_name);
      str.append("\" <", 3u);
      str.append(class_name);
      str.append("> (", 3u);
      str.append(uclass_name);
      str.append(')');
      *result_pp = SkString::new_instance(str);
      }
    }

  //---------------------------------------------------------------------------------------
  // Get name of this Entity
  static void mthd_name(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {    
    if (result_pp) // Do nothing if result not desired
      {
      UObject * this_p = scope_p->this_as<SkUEEntity>();
      AString obj_name = this_p ? FStringToAString(this_p->GetName()) : "null";
      *result_pp = SkString::new_instance(obj_name);
      }
    }

  //---------------------------------------------------------------------------------------
  // Get Unreal Engine class of this Entity
  static void mthd_entity_class(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    if (result_pp) // Do nothing if result not desired
      {
      UObject * this_p = scope_p->this_as<SkUEEntity>();
      *result_pp = this_p ? SkUEEntityClass::new_instance(this_p->GetClass()) : SkBrain::ms_nil_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // =(Entity operand) Boolean
  static void mthd_op_equals(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    if (result_pp) // Do nothing if result not desired
      {
      *result_pp = SkBoolean::new_instance(scope_p->this_as<SkUEEntity>() == scope_p->get_arg<SkUEEntity>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // ~=(Entity operand) Boolean
  static void mthd_op_not_equal(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    if (result_pp) // Do nothing if result not desired
      {
      *result_pp = SkBoolean::new_instance(scope_p->this_as<SkUEEntity>() != scope_p->get_arg<SkUEEntity>(SkArg_1));
      }
    }

  //---------------------------------------------------------------------------------------
  // Get Unreal Engine class of this class
  static void mthdc_static_class(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    if (result_pp) // Do nothing if result not desired
      {
      // Determine class
      SkClass * class_p = &((SkMetaClass *)scope_p->get_topmost_scope())->get_class_info();
      UClass * uclass_p = SkUEClassBindingHelper::get_ue_class_from_sk_class(class_p);
      SK_ASSERTX(uclass_p, a_cstr_format("The UE4 equivalent of class type '%s' is not known to SkookumScript.", class_p->get_name_cstr_dbg()));
      *result_pp = uclass_p ? SkUEEntityClass::new_instance(uclass_p) : SkBrain::ms_nil_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // Entity@load(String name) ThisClass_
  static void mthdc_load(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    if (result_pp) // Do nothing if result not desired
      {
      // Determine class of object to load
      SkClass * class_p = &((SkMetaClass *)scope_p->get_topmost_scope())->get_class_info();
      UClass * uclass_p = SkUEClassBindingHelper::get_ue_class_from_sk_class(class_p);
      SK_ASSERTX(uclass_p, a_cstr_format("Cannot load entity '%s' as the UE4 equivalent of class type '%s' is not known to SkookumScript.", scope_p->get_arg<SkString>(SkArg_1).as_cstr(), class_p->get_name_cstr_dbg()));

      // Load object
      UObject * obj_p = nullptr;
      if (uclass_p)
        {
        obj_p = StaticLoadObject(uclass_p, SkUEClassBindingHelper::get_world(), *AStringToFString(scope_p->get_arg<SkString>(SkArg_1)));
        }

      *result_pp = obj_p ? SkUEEntity::new_instance(obj_p, uclass_p, class_p) : SkBrain::ms_nil_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // Entity@default() ThisClass_
  static void mthdc_default(SkInvokedMethod * scope_p, SkInstance ** result_pp)
    {
    if (result_pp) // Do nothing if result not desired
      {
      // Determine class of object to get
      SkClass * class_p = &((SkMetaClass *)scope_p->get_topmost_scope())->get_class_info();
      UClass * uclass_p = SkUEClassBindingHelper::get_ue_class_from_sk_class(class_p);
      SK_ASSERTX(uclass_p, a_cstr_format("Cannot get default instance of class '%s' as the UE4 equivalent of this class is not known to SkookumScript.", class_p->get_name_cstr_dbg()));

      // Get default object
      UObject * obj_p = nullptr;
      if (uclass_p)
        {
        obj_p = GetMutableDefault<UObject>(uclass_p);
        }

      *result_pp = obj_p ? SkUEEntity::new_instance(obj_p, uclass_p, class_p) : SkBrain::ms_nil_p;
      }
    }

  //---------------------------------------------------------------------------------------
  // Entity@_wait_until_destroyed()
  static bool coro_wait_until_destroyed(SkInvokedCoroutine * scope_p)
    {
    UObject * this_p = scope_p->this_as<SkUEEntity>(); // We store the UObject as a weak pointer so it becomes null when the object is destroyed
    return !this_p || !this_p->IsValidLowLevel();
    }

  static const SkClass::MethodInitializerFunc methods_i2[] =
    {
      { "!copy",        mthd_ctor_copy },
      { "!null",        mthd_ctor_null },
      { "assign",       mthd_op_assign },
      { "null?",        mthd_null_Q },
      { "String",       mthd_String },
      { "name",         mthd_name },
      { "entity_class", mthd_entity_class },
      { "equal?",       mthd_op_equals },
      { "not_equal?",   mthd_op_not_equal },
    };

  static const SkClass::MethodInitializerFunc methods_c2[] =
    {
      { "static_class", mthdc_static_class },
      { "load",         mthdc_load },
      { "default",      mthdc_default },
    };

  } // SkUEEntity_Impl

//---------------------------------------------------------------------------------------

void SkUEEntity_Ext::register_bindings()
  {
  ms_class_p->register_method_func_bulk(SkUEEntity_Impl::methods_i2, A_COUNT_OF(SkUEEntity_Impl::methods_i2), SkBindFlag_instance_no_rebind);
  ms_class_p->register_method_func_bulk(SkUEEntity_Impl::methods_c2, A_COUNT_OF(SkUEEntity_Impl::methods_c2), SkBindFlag_class_no_rebind);
  ms_class_p->register_coroutine_func("_wait_until_destroyed", SkUEEntity_Impl::coro_wait_until_destroyed, SkBindFlag_instance_no_rebind);
  }

