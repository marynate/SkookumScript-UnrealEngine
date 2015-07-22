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

  m_struct_vector3_p          = GetBaseStructure(TEXT("Vector"));
  m_struct_rotation_angles_p  = GetBaseStructure(TEXT("Rotator"));
  m_struct_transform_p        = GetBaseStructure(TEXT("Transform"));
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
    // If this is a blueprint generated class, we want to attach ourselves to the skeleton class instead
  #if 0 // WITH_EDITOR
    UBlueprintGeneratedClass * blueprint_class_p = Cast<UBlueprintGeneratedClass>(ue_class_p);
    if (blueprint_class_p)
      {      
      UBlueprint * blueprint_p = Cast<UBlueprint>(blueprint_class_p->ClassGeneratedBy);
      if (blueprint_p)
        {
        UClass * ue_skel_class_p = blueprint_p->SkeletonGeneratedClass;
        ue_class_p = ue_skel_class_p;
        }
      }
  #endif

    reinitialize_class(sk_class_p, ue_class_p);
    }
  else
    {
    // UE class not known (yet), so do the best we can in this situation
    for (auto method_p : sk_class_p->get_instance_methods())
      {
      prepare_method_entry(method_p);
      }
    for (auto method_p : sk_class_p->get_class_methods())
      {
      prepare_method_entry(method_p);
      }
    }

  return ue_class_p;
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::reinitialize_class(SkClass * sk_class_p, UClass * ue_class_p)
  {
  // Find existing methods of this class and delete them
  for (uint32_t i = 0; i < m_method_entry_array.get_length(); ++i)
    {
    MethodEntry * method_entry_p = m_method_entry_array[i];
    if (method_entry_p && method_entry_p->m_sk_method_p->get_scope() == sk_class_p)
      {
      delete_method_entry(i);
      }
    }

  // Gather new methods/events
  for (auto method_p : sk_class_p->get_instance_methods())
    {
    try_add_method_entry(ue_class_p, method_p);
    }
  for (auto method_p : sk_class_p->get_class_methods())
    {
    try_add_method_entry(ue_class_p, method_p);
    }
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

bool SkUEBlueprintInterface::is_skookum_blueprint_function(UFunction * function_p) const
  {
  Native native_function_p = function_p->GetNativeFunc();
  return native_function_p == (Native)&SkUEBlueprintInterface::exec_class_method
      || native_function_p == (Native)&SkUEBlueprintInterface::exec_instance_method;
  }

//---------------------------------------------------------------------------------------

bool SkUEBlueprintInterface::is_skookum_blueprint_event(UFunction * function_p) const
  {
  // HACK for now just check if name starts with lowercase "on_"
  return (function_p->GetName().Find(TEXT("on_"), ESearchCase::CaseSensitive) == 0);
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::exec_method(FFrame & stack, void * const result_p, SkInstance * this_p)
  {
  const FunctionEntry & function_entry = static_cast<const FunctionEntry &>(*ms_singleton_p->m_method_entry_array[stack.CurrentNativeFunction->RepOffset]);
  SK_ASSERTX(function_entry.m_type == MethodType_Function, "MethodEntry has bad type!");

  SkInvokedMethod * imethod_p = SkInvokedMethod::pool_new(nullptr, this_p, function_entry.m_sk_method_p);

  SKDEBUG_ICALL_SET_INTERNAL(imethod_p);
  SKDEBUG_HOOK_SCRIPT_ENTRY(function_entry.m_sk_method_p->get_name());

  // Fill invoked method's argument list
  const SkParamEntry * param_entry_array = function_entry.get_param_entry_array();
  imethod_p->data_ensure_size(function_entry.m_num_params);
  for (uint32_t i = 0; i < function_entry.m_num_params; ++i)
    {
    const SkParamEntry & param_entry = param_entry_array[i];
    imethod_p->data_append_arg(param_entry.m_name, (*param_entry.m_fetcher_p)(stack));
    }

  // Done with stack - now increment the code ptr unless it is null
  stack.Code += !!stack.Code;

  // Call method
  SkInstance * result_instance_p = SkBrain::ms_nil_p;
  function_entry.m_sk_method_p->invoke(imethod_p, nullptr, &result_instance_p);
  if (function_entry.m_result_getter)
    {
    (*function_entry.m_result_getter)(result_p, result_instance_p);
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
// Execute a blueprint event
void SkUEBlueprintInterface::mthd_trigger_event(SkInvokedMethod * scope_p, SkInstance ** result_pp)
  {
  SkUEBlueprintInterface * this_p = ms_singleton_p;

  // Find function in the method entry array (this will be O(1) eventually when we have method user data implemented)
  for (auto method_entry_p : this_p->m_method_entry_array)
    {
    if (method_entry_p && method_entry_p->m_sk_method_p == scope_p->get_invokable())
      {
      // Found - invoke it
      SK_ASSERTX(method_entry_p->m_ue_method_p.IsValid(), a_str_format("Tried to invoke Blueprint event function %s but the UFunction object has gone away.", method_entry_p->m_sk_method_p->get_name_cstr()));

      // Create parameters on stack
      const EventEntry * event_entry_p = static_cast<EventEntry *>(method_entry_p);
      const K2ParamEntry * param_entry_array = event_entry_p->get_param_entry_array();
      UFunction * ue_method_p = event_entry_p->m_ue_method_p.Get();
      uint8_t * k2_params_storage_p = a_stack_allocate(ue_method_p->ParmsSize, uint8_t);
      for (uint32_t i = 0; i < event_entry_p->m_num_params; ++i)
        {
        const K2ParamEntry & param_entry = param_entry_array[i];
        (*param_entry.m_getter_p)(k2_params_storage_p + param_entry.m_offset, scope_p->get_arg(i));
        }

      // Invoke K2 script event with parameters
      AActor * actor_p = scope_p->this_as<SkUEActor>();
      if (!event_entry_p->m_ue_method_to_invoke_p.IsValid())
        {
        // Find Kismet copy of our method to invoke
        event_entry_p->m_ue_method_to_invoke_p = actor_p->FindFunctionChecked(*ue_method_p->GetName());
        }
      // Check if this event is actually present in any Blueprint graph
      SK_ASSERTX(event_entry_p->m_ue_method_to_invoke_p->Script.Num() > 0, a_str_format("Warning: Call to %S has no effect as no such Blueprint event node exists.", *ue_method_p->GetName()));
      actor_p->ProcessEvent(event_entry_p->m_ue_method_to_invoke_p.Get(), k2_params_storage_p);

      // No return value
      if (result_pp) *result_pp = SkBrain::ms_nil_p;
      return;
      }
    }

  SK_ASSERTX(false, a_str_format("Tried to invoke non-existing Blueprint event method %s.", scope_p->get_invokable()->get_name_cstr_dbg()));
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::prepare_method_entry(SkMethodBase * sk_method_p)
  {
  // For now, all we do here is bind any yet unbound event binding
  if ((sk_method_p->get_invoke_type() == SkInvokable_method_func || sk_method_p->get_invoke_type() == SkInvokable_method_mthd) 
    && !sk_method_p->is_bound()
    && ::strncmp(sk_method_p->get_name_cstr(), "on_", 3) == 0)
    {
    bind_event_method(sk_method_p);
    }
  }

//---------------------------------------------------------------------------------------

int32_t SkUEBlueprintInterface::try_add_method_entry(UClass * ue_class_p, SkMethodBase * sk_method_p)
  {
  bool is_operator = false;
  #if (SKOOKUM & SK_COMPILED_OUT)
    is_operator = sk_method_p->is_operator(); // $Revisit - In shipping builds just bind operators as well - no harm just unnecessary
  #endif
  if (sk_method_p->get_invoke_type() == SkInvokable_method && !is_operator)
    {
    return add_function_entry(ue_class_p, sk_method_p, FString());
    }
  else if ((sk_method_p->get_invoke_type() == SkInvokable_method_func || sk_method_p->get_invoke_type() == SkInvokable_method_mthd) 
    && (!sk_method_p->is_bound() || static_cast<SkMethodFunc *>(sk_method_p)->m_atomic_f == &mthd_trigger_event)
    && ::strncmp(sk_method_p->get_name_cstr(), "on_", 3) == 0)
    {
    // HACK - for now until we got tags working - if it's atomic, not bound and name starts with "on_" it's an event!
    return add_event_entry(ue_class_p, sk_method_p, FString());
    }

  return -1;
  }

//---------------------------------------------------------------------------------------

int32_t SkUEBlueprintInterface::add_function_entry(UClass * ue_class_p, SkMethodBase * sk_method_p, const FString & category)
  {
  // Create new UFunction
  UFunction * ue_method_p = NewObject<UFunction>(ue_class_p, sk_method_p->get_name_cstr(), RF_Public);
  ue_method_p->FunctionFlags |= FUNC_BlueprintCallable | FUNC_Native | FUNC_Public;
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
  ue_method_p->SetMetaData(TEXT("Tooltip"), *FString::Printf(TEXT("Call to SkookumScript method\n%S@%S()"), sk_method_p->get_scope()->get_name_cstr(), sk_method_p->get_name_cstr()));
#endif

  // Parameters of the method we are creating
  const SkParameters & params = sk_method_p->get_params();
  const tSkParamList & param_list = params.get_param_list();
  uint32_t num_params = param_list.get_length();

  // Store parameter info here
  ParamInfo param_info = { 0 }; // Set to zero as otherwise the compiler will give silly warnings

  // Method array entry that we will allocate
  FunctionEntry * function_entry_p = nullptr;

  // Handle return value if any
  if (params.get_result_class() && params.get_result_class() != SkObject::ms_class_p)
    {
    if (!make_param(&param_info, ue_method_p, params.get_result_class(), "result"))
      {
      // If any parameters can not be mapped, skip exporting this entire method
    skip_method:
      if (function_entry_p) delete function_entry_p;
      ue_method_p->ConditionalBeginDestroy();
      return -1;
      }

    param_info.m_ue_param_p->PropertyFlags |= CPF_ReturnParm; // Flag as return value
    ue_method_p->LinkChild(param_info.m_ue_param_p); // Link return param first so it will be last in the list
    }

  // Allocate method entry
  function_entry_p = new(AMemory::malloc(sizeof(FunctionEntry) + num_params * sizeof(SkParamEntry), "FunctionEntry")) FunctionEntry(sk_method_p, ue_method_p, num_params, param_info.m_sk_value_getter_p);

  // Handle input parameters
  for (int32_t i = num_params - 1; i >= 0; --i)
    {
    const SkParameterBase * input_param = param_list(i);
    if (!make_param(&param_info, ue_method_p, input_param->get_expected_type(), input_param->get_name_cstr()))
      {
      goto skip_method;
      }
      
    ue_method_p->LinkChild(param_info.m_ue_param_p); // Link in reverse order so it will be in correct order eventually

    SkParamEntry & param_entry = function_entry_p->get_param_entry_array()[i];
    param_entry.m_name = input_param->get_name();
    param_entry.m_fetcher_p = param_info.m_k2_param_fetcher_p;
    }

  // Make method known to its class
  ue_class_p->LinkChild(ue_method_p);
  ue_class_p->AddFunctionToFunctionMap(ue_method_p);

  // Make sure parameter list is properly linked and offsets are set
  ue_method_p->StaticLink(true);

  // Store method entry in array
  // Look if there is an empty slot that we can reuse
  int32_t method_index;
  for (method_index = 0; method_index < (int32_t)m_method_entry_array.get_length(); ++method_index)
    {
    if (!m_method_entry_array[method_index]) break;
    }
  if (method_index == m_method_entry_array.get_length())
    {
    m_method_entry_array.append(*function_entry_p);
    }
  else
    {
    m_method_entry_array.set_at(method_index, function_entry_p);
    }
  ue_method_p->RepOffset = uint16(method_index); // Remember array index of method to call

  return method_index;
  }

//---------------------------------------------------------------------------------------

int32_t SkUEBlueprintInterface::add_event_entry(UClass * ue_class_p, SkMethodBase * sk_method_p, const FString & category)
  {
  // Create new UFunction
  UFunction * ue_method_p = new(EC_InternalUseOnlyConstructor, ue_class_p, sk_method_p->get_name_cstr(), RF_Public) UFunction(FObjectInitializer(), NULL, FUNC_BlueprintEvent | FUNC_Event | FUNC_Public, UINT16_MAX);
  ue_method_p->Bind(); // Bind to default Blueprint event mechanism
#if WITH_EDITOR
  ue_method_p->SetMetaData(TEXT("Category"), category.IsEmpty() ? TEXT("SkookumScript") : *(TEXT("SkookumScript|") + category));
  ue_method_p->SetMetaData(TEXT("Tooltip"), *FString::Printf(TEXT("Triggered by SkookumScript method\n%S@%S()"), sk_method_p->get_scope()->get_name_cstr(), sk_method_p->get_name_cstr()));
#endif

  // Bind Sk method
  bind_event_method(sk_method_p);

  // Parameters of the method we are creating
  const SkParameters & params = sk_method_p->get_params();
  const tSkParamList & param_list = params.get_param_list();
  uint32_t num_params = param_list.get_length();

  // Allocate method entry
  EventEntry * event_entry_p = new(AMemory::malloc(sizeof(EventEntry) + num_params * sizeof(SkParamEntry), "EventEntry")) EventEntry(sk_method_p, ue_method_p, num_params);

  // Store parameter info here
  ParamInfo param_info = { 0 }; // Set to zero as otherwise the compiler will give silly warnings

  // Handle input parameters
  for (int32_t i = num_params - 1; i >= 0; --i)
    {
    const SkParameterBase * input_param = param_list(i);
    if (!make_param(&param_info, ue_method_p, input_param->get_expected_type(), input_param->get_name_cstr()))
      {
      delete event_entry_p;
      ue_method_p->ConditionalBeginDestroy();
      return -1;
      }

    ue_method_p->LinkChild(param_info.m_ue_param_p); // Link in reverse order so it will be in correct order eventually

    K2ParamEntry & param_entry = event_entry_p->get_param_entry_array()[i];
    param_entry.m_getter_p = param_info.m_sk_value_getter_p;
    }

  // Make method known to its class
  ue_class_p->LinkChild(ue_method_p);
  ue_class_p->AddFunctionToFunctionMap(ue_method_p);

  // Make sure parameter list is properly linked and offsets are set
  ue_method_p->StaticLink(true);

  // Store shorthand to parameter offsets (only calculated inside StaticLink())
  UField * property_p = ue_method_p->Children;
  for (uint32_t i = 0; i < num_params; ++i, property_p = property_p->Next)
    {
    K2ParamEntry & param_entry = event_entry_p->get_param_entry_array()[i];
    param_entry.m_offset = static_cast<UProperty *>(property_p)->GetOffset_ForUFunction();
    }

  // Store method entry in array
  // Look if there is an empty slot that we can reuse
  int32_t method_index;
  for (method_index = 0; method_index < (int32_t)m_method_entry_array.get_length(); ++method_index)
    {
    if (!m_method_entry_array[method_index]) break;
    }
  if (method_index == m_method_entry_array.get_length())
    {
    m_method_entry_array.append(*event_entry_p);
    }
  else
    {
    m_method_entry_array.set_at(method_index, event_entry_p);
    }

  return method_index;
  }

//---------------------------------------------------------------------------------------
// Delete method entry and set pointer to nullptr so it can be reused

void SkUEBlueprintInterface::delete_method_entry(uint32_t method_index)
  {
  MethodEntry * method_entry_p = m_method_entry_array[method_index];
  if (method_entry_p)
    {
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
    AMemory::free(method_entry_p);
    m_method_entry_array.set_at(method_index, nullptr);
    }
  }

//---------------------------------------------------------------------------------------
// NOTE: Does not LinkChild the parameter to the given function! Caller needs to do this (in reverse order)

bool SkUEBlueprintInterface::make_param(ParamInfo * out_param_info_p, UFunction * ue_method_p, SkClassDescBase * sk_parameter_class_p, const FName & param_name)
  {
  // Based on Sk type, figure out the matching UProperty as well as fetcher and setter methods
  UProperty * property_p = nullptr;
  tK2ParamFetcher k2_param_fetcher_p = nullptr;
  tSkValueGetter sk_value_getter_p = nullptr;
  if (sk_parameter_class_p == SkBoolean::ms_class_p)
    {
    property_p = NewObject<UBoolProperty>(ue_method_p, param_name, RF_Public);
    k2_param_fetcher_p = &fetch_k2_param_boolean;
    sk_value_getter_p = &get_sk_value_boolean;
    }
  else if (sk_parameter_class_p == SkInteger::ms_class_p)
    {
    property_p = NewObject<UIntProperty>(ue_method_p, param_name, RF_Public);
    k2_param_fetcher_p = &fetch_k2_param_integer;
    sk_value_getter_p = &get_sk_value_integer;
    }
  else if (sk_parameter_class_p == SkReal::ms_class_p)
    {
    property_p = NewObject<UFloatProperty>(ue_method_p, param_name, RF_Public);
    k2_param_fetcher_p = &fetch_k2_param_real;
    sk_value_getter_p = &get_sk_value_real;
    }
  else if (sk_parameter_class_p == SkString::ms_class_p)
    {
    property_p = NewObject<UStrProperty>(ue_method_p, param_name, RF_Public);
    k2_param_fetcher_p = &fetch_k2_param_string;
    sk_value_getter_p = &get_sk_value_string;
    }
  else if (sk_parameter_class_p == SkVector3::ms_class_p)
    {
    UStructProperty * struct_property_p = NewObject<UStructProperty>(ue_method_p, param_name);
    struct_property_p->Struct = m_struct_vector3_p;
    property_p = struct_property_p;
    k2_param_fetcher_p = &fetch_k2_param_vector3;
    sk_value_getter_p = &get_sk_value_vector3;
    }
  else if (sk_parameter_class_p == SkRotationAngles::ms_class_p)
    {
    UStructProperty * struct_property_p = NewObject<UStructProperty>(ue_method_p, param_name);
    struct_property_p->Struct = m_struct_rotation_angles_p;
    property_p = struct_property_p;
    k2_param_fetcher_p = &fetch_k2_param_rotation_angles;
    sk_value_getter_p = &get_sk_value_rotation_angles;
    }
  else if (sk_parameter_class_p == SkTransform::ms_class_p)
    {
    UStructProperty * struct_property_p = NewObject<UStructProperty>(ue_method_p, param_name);
    struct_property_p->Struct = m_struct_transform_p;
    property_p = struct_property_p;
    k2_param_fetcher_p = &fetch_k2_param_transform;
    sk_value_getter_p = &get_sk_value_transform;
    }
  else if (sk_parameter_class_p->get_key_class()->is_class(*SkUEEntity::ms_class_p))
    {
    UClass * uclass_p = SkUEClassBindingHelper::get_ue_class_from_sk_class(sk_parameter_class_p);
    SK_ASSERTX(uclass_p, a_cstr_format("Class '%s' of parameter '%s' of method '%S.%S' being exported to Blueprints is not a known engine class.", sk_parameter_class_p->get_key_class_name().as_cstr_dbg(), param_name.GetPlainANSIString(), *ue_method_p->GetOwnerClass()->GetName(), *ue_method_p->GetName()));
    if (uclass_p)
      {
      property_p = NewObject<UObjectProperty>(ue_method_p, param_name, RF_Public);
      static_cast<UObjectProperty *>(property_p)->PropertyClass = uclass_p;
      k2_param_fetcher_p = &fetch_k2_param_entity;
      sk_value_getter_p = &get_sk_value_entity;
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
  out_param_info_p->m_k2_param_fetcher_p = k2_param_fetcher_p;
  out_param_info_p->m_sk_value_getter_p = sk_value_getter_p;

  // Successful?
  return property_p != nullptr;
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_k2_param_boolean(FFrame & stack)
  {
  UBoolProperty::TCppType value = UBoolProperty::GetDefaultPropertyValue();
  stack.StepCompiledIn<UBoolProperty>(&value);
  return SkBoolean::new_instance(value);
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_k2_param_integer(FFrame & stack)
  {
  UIntProperty::TCppType value = UIntProperty::GetDefaultPropertyValue();
  stack.StepCompiledIn<UIntProperty>(&value);
  return SkInteger::new_instance(value);
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_k2_param_real(FFrame & stack)
  {
  UFloatProperty::TCppType value = UFloatProperty::GetDefaultPropertyValue();
  stack.StepCompiledIn<UFloatProperty>(&value);
  return SkReal::new_instance(value);
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_k2_param_string(FFrame & stack)
  {
  UStrProperty::TCppType value = UStrProperty::GetDefaultPropertyValue();
  stack.StepCompiledIn<UStrProperty>(&value);
  return SkString::new_instance(FStringToAString(value));
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_k2_param_vector3(FFrame & stack)
  {
  FVector value(ForceInitToZero);
  stack.StepCompiledIn<UStructProperty>(&value);
  return SkVector3::new_instance(value);
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_k2_param_rotation_angles(FFrame & stack)
  {
  FRotator value(ForceInitToZero);
  stack.StepCompiledIn<UStructProperty>(&value);
  return SkRotationAngles::new_instance(value);
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_k2_param_transform(FFrame & stack)
  {
  FTransform value;
  stack.StepCompiledIn<UStructProperty>(&value);
  return SkTransform::new_instance(value);
  }

//---------------------------------------------------------------------------------------

SkInstance * SkUEBlueprintInterface::fetch_k2_param_entity(FFrame & stack)
  {
  UObject * obj_p = nullptr;
  stack.StepCompiledIn<UObjectPropertyBase>(&obj_p);
  return SkUEEntity::new_instance(obj_p);
  }

//---------------------------------------------------------------------------------------

uint32_t SkUEBlueprintInterface::get_sk_value_boolean(void * const result_p, SkInstance * value_p)
  {
  *((UBoolProperty::TCppType *)result_p) = value_p->as<SkBoolean>();
  return sizeof(UBoolProperty::TCppType);
  }

//---------------------------------------------------------------------------------------

uint32_t SkUEBlueprintInterface::get_sk_value_integer(void * const result_p, SkInstance * value_p)
  {
  *((UIntProperty::TCppType *)result_p) = value_p->as<SkInteger>();
  return sizeof(UIntProperty::TCppType);
  }

//---------------------------------------------------------------------------------------

uint32_t SkUEBlueprintInterface::get_sk_value_real(void * const result_p, SkInstance * value_p)
  {
  *((UFloatProperty::TCppType *)result_p) = value_p->as<SkReal>();
  return sizeof(UFloatProperty::TCppType);
  }

//---------------------------------------------------------------------------------------

uint32_t SkUEBlueprintInterface::get_sk_value_string(void * const result_p, SkInstance * value_p)
  {
  *((UStrProperty::TCppType *)result_p) = AStringToFString(value_p->as<SkString>());
  return sizeof(UStrProperty::TCppType);
  }

//---------------------------------------------------------------------------------------

uint32_t SkUEBlueprintInterface::get_sk_value_vector3(void * const result_p, SkInstance * value_p)
  {
  *((FVector *)result_p) = value_p->as<SkVector3>();
  return sizeof(FVector);
  }

//---------------------------------------------------------------------------------------

uint32_t SkUEBlueprintInterface::get_sk_value_rotation_angles(void * const result_p, SkInstance * value_p)
  {
  *((FRotator *)result_p) = value_p->as<SkRotationAngles>();
  return sizeof(FRotator);
  }

//---------------------------------------------------------------------------------------

uint32_t SkUEBlueprintInterface::get_sk_value_transform(void * const result_p, SkInstance * value_p)
  {
  *((FTransform *)result_p) = value_p->as<SkTransform>();
  return sizeof(FTransform);
  }

//---------------------------------------------------------------------------------------

uint32_t SkUEBlueprintInterface::get_sk_value_entity(void * const result_p, SkInstance * value_p)
  {
  *((UObject **)result_p) = value_p->as<SkUEEntity>();
  return sizeof(UObject *);
  }

void SkUEBlueprintInterface::bind_event_method(SkMethodBase * sk_method_p)
  {
  SK_ASSERTX(!sk_method_p->is_bound() || static_cast<SkMethodFunc *>(sk_method_p)->m_atomic_f == &mthd_trigger_event, a_str_format("Trying to bind Blueprint event method '%s' but it is already bound to a different atomic implementation!", sk_method_p->get_name_cstr_dbg()));
  if (!sk_method_p->is_bound())
    {
    sk_method_p->get_scope()->register_method_func(sk_method_p->get_name(), &mthd_trigger_event, SkBindFlag_instance_no_rebind);
    }
  }

