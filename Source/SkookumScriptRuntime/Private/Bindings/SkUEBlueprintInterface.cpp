//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// Binding classes for UE4 
//
// Author: Markus Breyer
//=======================================================================================

#include "../SkookumScriptRuntimePrivatePCH.h"
#include "SkUEBlueprintInterface.hpp"

#include "VectorMath/SkVector3.hpp"
#include "VectorMath/SkRotationAngles.hpp"
#include "VectorMath/SkTransform.hpp"
#include "Engine/SkUEEntity.hpp"
#include "Engine/SkUEActor.hpp"

//---------------------------------------------------------------------------------------

SkUEBlueprintInterface * SkUEBlueprintInterface::ms_singleton_p;

//---------------------------------------------------------------------------------------

SkUEBlueprintInterface::SkUEBlueprintInterface()
  {
  SK_ASSERTX(!ms_singleton_p, "There can be only one instance of this class.");
  ms_singleton_p = this;
  }

//---------------------------------------------------------------------------------------

SkUEBlueprintInterface::~SkUEBlueprintInterface()
  {
  clear();

  SK_ASSERTX(ms_singleton_p == this, "There can be only one instance of this class.");
  ms_singleton_p = nullptr;
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::clear()
  {
  // Destroy all UFunctions and UProperties we allocated
  for (uint32_t i = 0; i < m_method_entry_array.get_length(); ++i)
    {
    delete_method_entry(i);
    }

  // And forget pointers to them 
  m_method_entry_array.empty();
  }

//---------------------------------------------------------------------------------------

UClass * SkUEBlueprintInterface::reinitialize_class(SkClass * sk_class_p)
  {
  // Find our UE counterpart
  UClass * ue_class_p = SkUEClassBindingHelper::get_ue_class_from_sk_class(sk_class_p);

  // Can only reinitialize if this class has a mapping to UE
  if (ue_class_p)
    {
    // Find existing methods of this class and delete them
    for (uint32_t i = 0; i < m_method_entry_array.get_length(); ++i)
      {
      SK_ASSERTX(m_method_entry_array[i]->m_ue_method_p.IsValid(), "Encountered invalid method pointer."); // If this is a problem, store and use sk_class_p instead
      if (m_method_entry_array[i]->m_ue_method_p->GetOuter() == ue_class_p)
        {
        delete_method_entry(i);
        }
      }

    // Gather new methods
    for (auto method_p : sk_class_p->get_instance_methods())
      {
      if (method_p->get_invoke_type() == SkInvokable_method && !method_p->is_operator())
        {
        add_method_entry(ue_class_p, method_p, FString());
        }
      }
    for (auto method_p : sk_class_p->get_class_methods())
      {
      if (method_p->get_invoke_type() == SkInvokable_method && !method_p->is_operator())
        {
        add_method_entry(ue_class_p, method_p, FString());
        }
      }
    }

  return ue_class_p;
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::reinitialize_class_recursively(SkClass * sk_class_p)
  {
  if (reinitialize_class(sk_class_p))
    {
    // Gather sub classes
    const tSkClasses & sub_classes = sk_class_p->get_subclasses();
    for (uint32_t i = 0; i < sub_classes.get_length(); ++i)
      {
      reinitialize_class_recursively(sub_classes(i));
      }
    }
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::reinitialize_all()
  {
  // Clear out old mappings
  clear();

  // Traverse Sk classes and gather methods that want to be exposed
  reinitialize_class_recursively(SkUEActor::ms_class_p);
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::exec_method(FFrame & stack, void * const result_p, SkInstance * this_p)
  {
  const MethodEntry & method_entry = *ms_singleton_p->m_method_entry_array[stack.CurrentNativeFunction->RepOffset];

  SkInvokedMethod * imethod_p = SkInvokedMethod::pool_new(nullptr, this_p, method_entry.m_sk_method_p);

  SKDEBUG_ICALL_SET_INTERNAL(imethod_p);
  SKDEBUG_HOOK_SCRIPT_ENTRY(method_entry.m_sk_method_p->get_name());

  // Fill invoked method's argument list
  const ParamEntry * param_entry_array = method_entry.get_param_entry_array();
  imethod_p->data_ensure_size(method_entry.m_num_params);
  for (uint32_t i = 0; i < method_entry.m_num_params; ++i)
    {
    const ParamEntry & param_entry = param_entry_array[i];
    imethod_p->data_append_arg(param_entry.m_name, (*param_entry.m_fetcher_p)(stack));
    }

  // Done with stack - now increment the code ptr unless it is null
  stack.Code += !!stack.Code;

  // Call method
  SkInstance * result_instance_p = SkBrain::ms_nil_p;
  method_entry.m_sk_method_p->invoke(imethod_p, nullptr, &result_instance_p);
  if (method_entry.m_result_setter_p)
    {
    (*method_entry.m_result_setter_p)(result_p, result_instance_p);
    }

  SKDEBUG_HOOK_SCRIPT_EXIT();

  // Clean up arguments and temporary variables 
  // & make invoked method wrapper available for its next use
  SkInvokedMethod::pool_delete(imethod_p);
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::exec_class_method(FFrame & stack, void * const result_p)
  {
  exec_method(stack, result_p, nullptr);
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::exec_instance_method(FFrame & stack, void * const result_p)
  {
  SkInstance * this_p = SkUEEntity::new_instance(stack.Object);
  exec_method(stack, result_p, this_p);
  this_p->dereference();
  }

//---------------------------------------------------------------------------------------

int32_t SkUEBlueprintInterface::add_method_entry(UClass * ue_class_p, SkMethodBase * sk_method_p, const FString & category)
  {
  // Create new UFunction
  UFunction * ue_method_p = NewObject<UFunction>(ue_class_p, sk_method_p->get_name_cstr(), RF_Public|RF_RootSet);
  ue_method_p->FunctionFlags |= FUNC_BlueprintCallable | FUNC_Public | FUNC_Final | FUNC_Native;
  if (sk_method_p->is_class_member())
    {
    ue_method_p->SetNativeFunc((Native)&SkUEBlueprintInterface::exec_class_method);
    ue_method_p->FunctionFlags |= FUNC_Static;
    }
  else
    {
    ue_method_p->SetNativeFunc((Native)&SkUEBlueprintInterface::exec_instance_method);
    }
#if WITH_EDITOR
  ue_method_p->SetMetaData(TEXT("Category"), category.IsEmpty() ? TEXT("SkookumScript") : *(TEXT("SkookumScript|") + category));
#endif

  // Parameters of the method we are creating
  const SkParameters & params = sk_method_p->get_params();
  const tSkParamList & param_list = params.get_param_list();
  uint32_t num_params = param_list.get_length();

  ParamInfo param_info = {0}; // Set to zero as otherwise the compiler will give silly warnings

  // Allocate method entry
  MethodEntry * method_entry_p = new(AMemory::malloc(sizeof(MethodEntry) + num_params * sizeof(ParamEntry), "MethodEntry")) MethodEntry(sk_method_p, ue_method_p, num_params, param_info.m_result_setter_p);

  // Handle return value if any
  if (params.get_result_class())
    {
    if (!make_param(&param_info, ue_method_p, params.get_result_class(), "result"))
      {
      // If any parameters can not be mapped, skip exporting this entire method
    skip_method:
      delete method_entry_p;
      ue_method_p->ConditionalBeginDestroy();
      return -1;
      }

    param_info.m_ue_param_p->PropertyFlags |= CPF_ReturnParm; // Flag as return value
    ue_method_p->LinkChild(param_info.m_ue_param_p); // Link return param first so it will be last in the list
    }

  // Handle input parameters
  for (int32_t i = num_params - 1; i >= 0; --i)
    {
    const SkParameterBase * input_param = param_list(i);
    if (!make_param(&param_info, ue_method_p, input_param->get_expected_type(), input_param->get_name_cstr()))
      {
      goto skip_method;
      }
      
    ue_method_p->LinkChild(param_info.m_ue_param_p); // Link in reverse order so it will be in correct order eventually

    ParamEntry & param_entry = method_entry_p->get_param_entry_array()[i];
    param_entry.m_name = input_param->get_name();
    param_entry.m_fetcher_p = param_info.m_param_fetcher_p;
    }

  // Store method entry in array
  // Look if there is an empty slot that we can reuse
  int32_t method_index;
  for (method_index = 0; method_index < (int32_t)m_method_entry_array.get_length(); ++method_index)
    {
    if (!m_method_entry_array[method_index]) break;
    }
  ue_method_p->RepOffset = uint16(method_index); // Remember array index of method to call
  if (method_index == m_method_entry_array.get_length())
    {
    m_method_entry_array.append(*method_entry_p);
    }
  else
    {
    m_method_entry_array.set_at(method_index, method_entry_p);
    }

  // Make method known to its class
  ue_class_p->LinkChild(ue_method_p);
  ue_class_p->AddFunctionToFunctionMap(ue_method_p);

  return method_index;
  }

//---------------------------------------------------------------------------------------
// Delete method entry and set pointer to nullptr so it can be reused

void SkUEBlueprintInterface::delete_method_entry(uint32_t method_index)
  {
  MethodEntry * method_entry_p = m_method_entry_array[method_index];
  if (method_entry_p->m_ue_method_p.IsValid())
    {
    UFunction * ue_method_p = method_entry_p->m_ue_method_p.Get();
    UClass * ue_class_p = ue_method_p->GetOwnerClass();
    // Unlink from its owner class
    ue_class_p->RemoveFunctionFromFunctionMap(ue_method_p);
    // Unlink from the Children list as well
    UField ** prev_field_pp = &ue_class_p->Children;
    for (UField * field_p = *prev_field_pp; field_p; prev_field_pp = &field_p->Next, field_p = *prev_field_pp)
      {
      if (field_p == ue_method_p)
        {
        *prev_field_pp = field_p->Next;
        break;
        }
      }
    // Destroy the function along with its attached properties
    ue_method_p->ConditionalBeginDestroy();
    }
  delete method_entry_p;
  m_method_entry_array.set_at(method_index, nullptr);
  }

//---------------------------------------------------------------------------------------
// NOTE: Does not LinkChild the parameter to the given function! Caller needs to do this (in reverse order)

bool SkUEBlueprintInterface::make_param(ParamInfo * out_param_info_p, UFunction * ue_method_p, SkClassDescBase * sk_parameter_class_p, const FName & param_name)
  {
  // Based on Sk type, figure out the matching UProperty as well as fetcher and setter methods
  UProperty * property_p = nullptr;
  tParamFetcher param_fetcher_p = nullptr;
  tResultSetter result_setter_p = nullptr;
  if (sk_parameter_class_p == SkBoolean::ms_class_p)
    {
    property_p = NewObject<UBoolProperty>(ue_method_p, param_name, RF_Public);
    param_fetcher_p = &fetch_param_boolean;
    result_setter_p = &set_result_boolean;
    }
  else if (sk_parameter_class_p == SkInteger::ms_class_p)
    {
    property_p = NewObject<UIntProperty>(ue_method_p, param_name, RF_Public);
    param_fetcher_p = &fetch_param_integer;
    result_setter_p = &set_result_integer;
    }
  else if (sk_parameter_class_p == SkReal::ms_class_p)
    {
    property_p = NewObject<UFloatProperty>(ue_method_p, param_name, RF_Public);
    param_fetcher_p = &fetch_param_real;
    result_setter_p = &set_result_real;
    }
  else if (sk_parameter_class_p == SkString::ms_class_p)
    {
    property_p = NewObject<UStrProperty>(ue_method_p, param_name, RF_Public);
    param_fetcher_p = &fetch_param_string;
    result_setter_p = &set_result_string;
    }
  else if (sk_parameter_class_p == SkVector3::ms_class_p)
    {
    UStructProperty * struct_property_p = NewObject<UStructProperty>(ue_method_p, param_name);
    struct_property_p->Struct = NewObject<UScriptStruct>(property_p, "Vector", RF_Public);
    property_p = struct_property_p;
    param_fetcher_p = &fetch_param_vector3;
    result_setter_p = &set_result_vector3;
    }
  else if (sk_parameter_class_p == SkRotationAngles::ms_class_p)
    {
    UStructProperty * struct_property_p = NewObject<UStructProperty>(ue_method_p, param_name);
    struct_property_p->Struct = NewObject<UScriptStruct>(property_p, "Rotator", RF_Public);
    property_p = struct_property_p;
    param_fetcher_p = &fetch_param_rotation_angles;
    result_setter_p = &set_result_rotation_angles;
    }
  else if (sk_parameter_class_p == SkTransform::ms_class_p)
    {
    UStructProperty * struct_property_p = NewObject<UStructProperty>(ue_method_p, param_name);
    struct_property_p->Struct = NewObject<UScriptStruct>(property_p, "Transform", RF_Public);
    property_p = struct_property_p;
    param_fetcher_p = &fetch_param_transform;
    result_setter_p = &set_result_transform;
    }
  else if (sk_parameter_class_p->get_key_class()->is_class(*SkUEEntity::ms_class_p))
    {
    UClass * uclass_p = SkUEClassBindingHelper::get_ue_class_from_sk_class(sk_parameter_class_p);
    SK_ASSERTX(uclass_p, a_cstr_format("Class '%s' of parameter '%s' of method '%S.%S' being exported to Blueprints is not a known engine class.", sk_parameter_class_p->get_key_class_name().as_cstr_dbg(), param_name.GetPlainANSIString(), *ue_method_p->GetOwnerClass()->GetName(), *ue_method_p->GetName()));
    if (uclass_p)
      {
      property_p = NewObject<UObjectProperty>(ue_method_p, uclass_p, param_name, RF_Public);
      param_fetcher_p = &fetch_param_entity;
      result_setter_p = &set_result_entity;
      }
    }
  else
    {
    //SK_ASSERTX(false, a_cstr_format("Class '%s' of parameter '%s' of method '%S.%S' being exported to Blueprints can not be mapped to a Blueprint-compatible type.", sk_parameter_class_p->get_key_class_name().as_cstr_dbg(), param_name.GetPlainANSIString(), *ue_method_p->GetOwnerClass()->GetName(), *ue_method_p->GetName()));
    }

  // Add flags
  if (property_p)
    {
    property_p->PropertyFlags |= CPF_Parm;
    }

  // Set result
  out_param_info_p->m_ue_param_p = property_p;
  out_param_info_p->m_param_fetcher_p = param_fetcher_p;
  out_param_info_p->m_result_setter_p = result_setter_p;

  // Successful?
  return property_p != nullptr;
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_param_boolean(FFrame & stack)
  {
  UBoolProperty::TCppType value = UBoolProperty::GetDefaultPropertyValue();
  stack.StepCompiledIn<UBoolProperty>(&value);
  return SkBoolean::new_instance(value);
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_param_integer(FFrame & stack)
  {
  UIntProperty::TCppType value = UIntProperty::GetDefaultPropertyValue();
  stack.StepCompiledIn<UIntProperty>(&value);
  return SkInteger::new_instance(value);
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_param_real(FFrame & stack)
  {
  UFloatProperty::TCppType value = UFloatProperty::GetDefaultPropertyValue();
  stack.StepCompiledIn<UFloatProperty>(&value);
  return SkReal::new_instance(value);
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_param_string(FFrame & stack)
  {
  UStrProperty::TCppType value = UStrProperty::GetDefaultPropertyValue();
  stack.StepCompiledIn<UStrProperty>(&value);
  return SkString::new_instance(FStringToAString(value));
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_param_vector3(FFrame & stack)
  {
  FVector value(ForceInitToZero);
  stack.StepCompiledIn<UStructProperty>(&value);
  return SkVector3::new_instance(value);
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_param_rotation_angles(FFrame & stack)
  {
  FRotator value(ForceInitToZero);
  stack.StepCompiledIn<UStructProperty>(&value);
  return SkRotationAngles::new_instance(value);
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_param_transform(FFrame & stack)
  {
  FTransform value;
  stack.StepCompiledIn<UStructProperty>(&value);
  return SkTransform::new_instance(value);
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_param_entity(FFrame & stack)
  {
  UObject * obj_p = nullptr;
  stack.StepCompiledIn<UObjectPropertyBase>(&obj_p);
  return SkUEEntity::new_instance(obj_p);
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::set_result_boolean(void * const result_p, SkInstance * value_p)
  {
  *((UBoolProperty::TCppType *)result_p) = value_p->as<SkBoolean>();
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::set_result_integer(void * const result_p, SkInstance * value_p)
  {
  *((UIntProperty::TCppType *)result_p) = value_p->as<SkInteger>();
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::set_result_real(void * const result_p, SkInstance * value_p)
  {
  *((UFloatProperty::TCppType *)result_p) = value_p->as<SkReal>();
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::set_result_string(void * const result_p, SkInstance * value_p)
  {
  *((UStrProperty::TCppType *)result_p) = AStringToFString(value_p->as<SkString>());
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::set_result_vector3(void * const result_p, SkInstance * value_p)
  {
  *((FVector *)result_p) = value_p->as<SkVector3>();
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::set_result_rotation_angles(void * const result_p, SkInstance * value_p)
  {
  *((FRotator *)result_p) = value_p->as<SkRotationAngles>();
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::set_result_transform(void * const result_p, SkInstance * value_p)
  {
  *((FTransform *)result_p) = value_p->as<SkTransform>();
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::set_result_entity(void * const result_p, SkInstance * value_p)
  {
  *((UObject **)result_p) = value_p->as<SkUEEntity>();
  }
