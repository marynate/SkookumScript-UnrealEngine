//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// Class for interfacing with Blueprint graphs 
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
  for (uint32_t i = 0; i < m_binding_entry_array.get_length(); ++i)
    {
    delete_binding_entry(i);
    }

  // And forget pointers to them 
  m_binding_entry_array.empty();
  }

//---------------------------------------------------------------------------------------

UClass * SkUEBlueprintInterface::reinitialize_class(SkClass * sk_class_p)
  {
  // Find our UE counterpart
  UClass * ue_class1_p = SkUEClassBindingHelper::get_ue_class_from_sk_class(sk_class_p);

  // Can only reinitialize if this class has a mapping to UE
  if (ue_class1_p)
    {
    UClass * ue_class2_p = nullptr;

    // If this is a blueprint generated class, we want to attach ourselves to the skeleton class as well
    #if WITH_EDITORONLY_DATA
      UBlueprint * blueprint_p = UBlueprint::GetBlueprintFromClass(ue_class1_p);
      if (blueprint_p)
        {
        ue_class2_p = blueprint_p->SkeletonGeneratedClass;
      }
  #endif

    reinitialize_class(sk_class_p, ue_class1_p);
    }
  else
    {
    // UE class not known (yet), so do the best we can in this situation
    for (auto method_p : sk_class_p->get_instance_methods())
      {
      prepare_binding_entry(method_p);
      }
    for (auto method_p : sk_class_p->get_class_methods())
      {
      prepare_binding_entry(method_p);
      }
    }

  return ue_class1_p;
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::reinitialize_class(SkClass * sk_class_p, UClass * ue_class_p)
  {
  // Find existing methods of this class and mark them for delete
  for (uint32_t i = 0; i < m_binding_entry_array.get_length(); ++i)
    {
    BindingEntry * binding_entry_p = m_binding_entry_array[i];
    if (binding_entry_p && binding_entry_p->m_sk_class_p == sk_class_p)
      {
      binding_entry_p->m_marked_for_delete = true;
      }
    }

  // Gather new methods/events
  for (auto method_p : sk_class_p->get_instance_methods())
    {
    try_add_binding_entry(ue_class_p, method_p);
    }
  for (auto method_p : sk_class_p->get_class_methods())
    {
    try_add_binding_entry(ue_class_p, method_p);
    }

  // Now go and delete anything still marked for delete
  for (uint32_t i = 0; i < m_binding_entry_array.get_length(); ++i)
    {
    BindingEntry * binding_entry_p = m_binding_entry_array[i];
    if (binding_entry_p && binding_entry_p->m_marked_for_delete)
      {
      delete_binding_entry(i);
      }
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
  const FunctionEntry & function_entry = static_cast<const FunctionEntry &>(*ms_singleton_p->m_binding_entry_array[stack.CurrentNativeFunction->RepOffset]);
  SK_ASSERTX(function_entry.m_type == BindingType_Function, "BindingEntry has bad type!");

  SkInvokedMethod * imethod_p = SkInvokedMethod::pool_new(nullptr, this_p, function_entry.m_sk_method_p);

  SKDEBUG_ICALL_SET_INTERNAL(imethod_p);
  SKDEBUG_HOOK_SCRIPT_ENTRY(function_entry.m_method_name);

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

  // Find function in the binding entry array (this will be O(1) eventually when we have method user data implemented)
  for (auto binding_entry_p : this_p->m_binding_entry_array)
    {
    if (binding_entry_p && binding_entry_p->m_sk_method_p == scope_p->get_invokable())
      {
      // Found - invoke it
      SK_ASSERTX(binding_entry_p->m_ue_function_p.IsValid(), a_str_format("Tried to invoke Blueprint event function %s but the UFunction object has gone away.", binding_entry_p->m_method_name.as_cstr()));
      SK_ASSERTX(binding_entry_p->m_type == BindingType_Event, "BindingEntry has bad type!");

      // Create parameters on stack
      const EventEntry * event_entry_p = static_cast<EventEntry *>(binding_entry_p);
      const K2ParamEntry * param_entry_array = event_entry_p->get_param_entry_array();
      UFunction * ue_function_p = event_entry_p->m_ue_function_p.Get(); // Invoke the first one
      uint8_t * k2_params_storage_p = a_stack_allocate(ue_function_p->ParmsSize, uint8_t);
      for (uint32_t i = 0; i < event_entry_p->m_num_params; ++i)
        {
        const K2ParamEntry & param_entry = param_entry_array[i];
        (*param_entry.m_getter_p)(k2_params_storage_p + param_entry.m_offset, scope_p->get_arg(i));
        }

      // Invoke K2 script event with parameters
      AActor * actor_p = scope_p->this_as<SkUEActor>();
      if (!event_entry_p->m_ue_function_to_invoke_p.IsValid())
        {
        // Find Kismet copy of our method to invoke
        event_entry_p->m_ue_function_to_invoke_p = actor_p->FindFunctionChecked(*ue_function_p->GetName());
        }
      // Check if this event is actually present in any Blueprint graph
      SK_ASSERTX(event_entry_p->m_ue_function_to_invoke_p->Script.Num() > 0, a_str_format("Warning: Call to %S has no effect as no such Blueprint event node exists.", *ue_function_p->GetName()));
      actor_p->ProcessEvent(event_entry_p->m_ue_function_to_invoke_p.Get(), k2_params_storage_p);

      // No return value
      if (result_pp) *result_pp = SkBrain::ms_nil_p;
      return;
      }
    }

  SK_ASSERTX(false, a_str_format("Tried to invoke non-existing Blueprint event method %s.", scope_p->get_invokable()->get_name_cstr_dbg()));
  }

//---------------------------------------------------------------------------------------

template<class _TypedName>
bool SkUEBlueprintInterface::have_identical_signatures(const tSkParamList & param_list, const _TypedName * param_array_p)
  {
  for (uint32_t i = 0; i < param_list.get_length(); ++i)
    {
    const TypedName & typed_name = param_array_p[i];
    const SkParameterBase * param_p = param_list(i);
    if (typed_name.m_name != param_p->get_name()
     || typed_name.m_type_p != param_p->get_expected_type())
      {
      return false;
      }
    }

  return true;
  }

//---------------------------------------------------------------------------------------

bool SkUEBlueprintInterface::try_update_binding_entry(UClass * ue_class_p, SkMethodBase * sk_method_p, int32_t * out_binding_index_p)
  {
  SK_ASSERTX(out_binding_index_p, "Must be non-null");

  const tSkParamList & param_list = sk_method_p->get_params().get_param_list();

  // See if we find any compatible entry already present:  
  for (int32_t binding_index = 0; binding_index < (int32_t)m_binding_entry_array.get_length(); ++binding_index)
    {
    BindingEntry * binding_entry_p = m_binding_entry_array[binding_index];
    if (binding_entry_p
      && binding_entry_p->m_method_name == sk_method_p->get_name()
      && binding_entry_p->m_sk_class_p == sk_method_p->get_scope()
      && binding_entry_p->m_is_class_member == sk_method_p->is_class_member())
      {
      // There is no overloading in SkookumScript
      // Therefore if the above matches we found our slot
      *out_binding_index_p = binding_index;

      // Don't update if UFunction is invalid or UClass no longer valid
      if (!binding_entry_p->m_ue_function_p.IsValid() || binding_entry_p->m_ue_function_p.Get()->GetOwnerClass() != ue_class_p)
        {
        return false;
        }

      // Can't update if signatures don't match
      if (binding_entry_p->m_num_params != param_list.get_length())
        {
        return false;
        }
      if (binding_entry_p->m_type == BindingType_Function)
        {
        FunctionEntry * function_entry_p = static_cast<FunctionEntry *>(binding_entry_p);
        if (!have_identical_signatures(param_list, function_entry_p->get_param_entry_array())
         || function_entry_p->m_result_type_p != sk_method_p->get_params().get_result_class())
          {
          return false;
          }
        }
      else
        {
        if (!have_identical_signatures(param_list, static_cast<EventEntry *>(binding_entry_p)->get_param_entry_array()))
          {
          return false;
          }
        }

      // We're good to update
      binding_entry_p->m_sk_method_p = sk_method_p; // Update Sk method pointer
      binding_entry_p->m_marked_for_delete = false; // Keep around
      return true; // Successfully updated
      }
    }

  // No matching entry found at all
  *out_binding_index_p = -1;
  return false;
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::prepare_binding_entry(SkMethodBase * sk_method_p)
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

int32_t SkUEBlueprintInterface::try_add_binding_entry(UClass * ue_class_p, SkMethodBase * sk_method_p)
  {
  bool is_operator = false;
  #if (SKOOKUM & SK_COMPILED_OUT)
    is_operator = sk_method_p->is_operator(); // $Revisit - In shipping builds just bind operators as well - no harm just unnecessary
  #endif
  if (sk_method_p->get_invoke_type() == SkInvokable_method && !is_operator)
    {
    return add_function_entry(ue_class_p, sk_method_p);
    }
  else if ((sk_method_p->get_invoke_type() == SkInvokable_method_func || sk_method_p->get_invoke_type() == SkInvokable_method_mthd) 
    && (!sk_method_p->is_bound() || static_cast<SkMethodFunc *>(sk_method_p)->m_atomic_f == &mthd_trigger_event)
    && ::strncmp(sk_method_p->get_name_cstr(), "on_", 3) == 0)
    {
    // HACK - for now until we got tags working - if it's atomic, not bound and name starts with "on_" it's an event!
    return add_event_entry(ue_class_p, sk_method_p);
    }

  return -1;
  }

//---------------------------------------------------------------------------------------

int32_t SkUEBlueprintInterface::add_function_entry(UClass * ue_class_p, SkMethodBase * sk_method_p)
  {
  // Check if this binding already exists, and if so, just update it
  int32_t binding_index;
  if (try_update_binding_entry(ue_class_p, sk_method_p, &binding_index))
    {
    return binding_index;
    }
  if (binding_index >= 0)
    {
    delete_binding_entry(binding_index);
    }

  // Parameters of the method we are creating
  const SkParameters & params = sk_method_p->get_params();
  const tSkParamList & param_list = params.get_param_list();
  uint32_t num_params = param_list.get_length();

  // Create new UFunction
  ParamInfo * param_info_array_p = a_stack_allocate(num_params + 1, ParamInfo);
  UFunction * ue_function_p = build_ue_function(ue_class_p, sk_method_p, BindingType_Function, param_info_array_p);
  if (!ue_function_p) return -1;

  // Allocate binding entry
  FunctionEntry * function_entry_p = new(AMemory::malloc(sizeof(FunctionEntry) + num_params * sizeof(SkParamEntry), "FunctionEntry")) FunctionEntry(sk_method_p, ue_function_p, num_params, params.get_result_class(), param_info_array_p[num_params].m_sk_value_getter_p);

  // Initialize parameters
  for (uint32_t i = 0; i < num_params; ++i)
    {
    const SkParameterBase * input_param = param_list(i);
    new (&function_entry_p->get_param_entry_array()[i]) SkParamEntry(input_param->get_name(), input_param->get_expected_type(), param_info_array_p[i].m_k2_param_fetcher_p);
    }

  // Store binding entry in array
  binding_index = store_binding_entry(function_entry_p, binding_index);
  ue_function_p->RepOffset = uint16(binding_index); // Remember array index of method to call
  return binding_index;
  }

//---------------------------------------------------------------------------------------

int32_t SkUEBlueprintInterface::add_event_entry(UClass * ue_class_p, SkMethodBase * sk_method_p)
  {
  // Check if this binding already exists, and if so, just update it
  int32_t binding_index;
  if (try_update_binding_entry(ue_class_p, sk_method_p, &binding_index))
    {
    return binding_index;
    }
  if (binding_index >= 0)
    {
    delete_binding_entry(binding_index);
    }

  // Parameters of the method we are creating
  const SkParameters & params = sk_method_p->get_params();
  const tSkParamList & param_list = params.get_param_list();
  uint32_t num_params = param_list.get_length();

  // Create new UFunction
  ParamInfo * param_info_array_p = a_stack_allocate(num_params + 1, ParamInfo);
  UFunction * ue_function_p = build_ue_function(ue_class_p, sk_method_p, BindingType_Event, param_info_array_p);
  if (!ue_function_p) return -1;

  // Bind Sk method
  bind_event_method(sk_method_p);

  // Allocate binding entry
  EventEntry * event_entry_p = new(AMemory::malloc(sizeof(EventEntry) + num_params * sizeof(K2ParamEntry), "EventEntry")) EventEntry(sk_method_p, ue_function_p, num_params);

  // Initialize parameters
  for (uint32_t i = 0; i < num_params; ++i)
    {
    const SkParameterBase * input_param = param_list(i);
    const ParamInfo & param_info = param_info_array_p[i];
    new (&event_entry_p->get_param_entry_array()[i]) K2ParamEntry(input_param->get_name(), input_param->get_expected_type(), param_info.m_sk_value_getter_p, static_cast<UProperty *>(param_info.m_ue_param_p)->GetOffset_ForUFunction());
    }

  // Store binding entry in array
  return store_binding_entry(event_entry_p, binding_index);
  }

//---------------------------------------------------------------------------------------
// Store a given BindingEntry into the m_binding_entry_array
// If an index is given, use that, otherwise, find an empty slot to reuse, or if that fails, append a new entry
int32_t SkUEBlueprintInterface::store_binding_entry(BindingEntry * binding_entry_p, int32_t binding_index_to_use)
  {
  // If no binding index known yet, look if there is an empty slot that we can reuse
  if (binding_index_to_use < 0)
    {
    for (binding_index_to_use = 0; binding_index_to_use < (int32_t)m_binding_entry_array.get_length(); ++binding_index_to_use)
      {
      if (!m_binding_entry_array[binding_index_to_use]) break;
      }
    }
  if (binding_index_to_use == m_binding_entry_array.get_length())
    {
    m_binding_entry_array.append(*binding_entry_p);
    }
  else
    {
    m_binding_entry_array.set_at(binding_index_to_use, binding_entry_p);
    }

  return binding_index_to_use;
  }

//---------------------------------------------------------------------------------------
// Delete binding entry and set pointer to nullptr so it can be reused

void SkUEBlueprintInterface::delete_binding_entry(uint32_t binding_index)
  {
  BindingEntry * binding_entry_p = m_binding_entry_array[binding_index];
  if (binding_entry_p)
    {
    SK_ASSERTX(binding_entry_p->m_ue_function_p.IsValid() || !binding_entry_p->m_ue_class_p.IsValid(), a_str_format("UFunction %s was deleted outside of SkUEBlueprintInterface and left dangling links behind in its owner UClass (%S).", binding_entry_p->m_method_name.as_cstr(), *binding_entry_p->m_ue_class_p->GetName()));
    if (binding_entry_p->m_ue_function_p.IsValid())
      {
      UFunction * ue_function_p = binding_entry_p->m_ue_function_p.Get();
      UClass * ue_class_p = binding_entry_p->m_ue_class_p.Get();
      // Unlink from its owner class
      ue_class_p->RemoveFunctionFromFunctionMap(ue_function_p);
      // Unlink from the Children list as well
      UField ** prev_field_pp = &ue_class_p->Children;
      for (UField * field_p = *prev_field_pp; field_p; prev_field_pp = &field_p->Next, field_p = *prev_field_pp)
        {
        if (field_p == ue_function_p)
          {
          *prev_field_pp = field_p->Next;
          break;
          }
        }
      // Destroy the function along with its attached properties
      ue_function_p->ConditionalBeginDestroy();
      }
    AMemory::free(binding_entry_p);
    m_binding_entry_array.set_at(binding_index, nullptr);
    }
  }

//---------------------------------------------------------------------------------------
// Params:
//   out_param_info_array_p: Storage for info on each parameter, return value is stored behind the input parameters, and is zeroed if there is no return value
UFunction * SkUEBlueprintInterface::build_ue_function(UClass * ue_class_p, SkMethodBase * sk_method_p, eBindingType binding_type, ParamInfo * out_param_info_array_p)
  {
  UFunction * ue_function_p = NewObject<UFunction>(ue_class_p, sk_method_p->get_name_cstr(), RF_Public | RF_RootSet);

  ue_function_p->FunctionFlags |= FUNC_Public;
  if (sk_method_p->is_class_member())
    {
    ue_function_p->FunctionFlags |= FUNC_Static;
    }

  if (binding_type == BindingType_Function)
    {
    ue_function_p->FunctionFlags |= FUNC_BlueprintCallable | FUNC_Native;
    ue_function_p->SetNativeFunc(sk_method_p->is_class_member() ? (Native)&SkUEBlueprintInterface::exec_class_method : (Native)&SkUEBlueprintInterface::exec_instance_method);
    #if WITH_EDITOR
      ue_function_p->SetMetaData(TEXT("Tooltip"), *FString::Printf(TEXT("Call to SkookumScript method\n%S@%S()"), sk_method_p->get_scope()->get_name_cstr(), sk_method_p->get_name_cstr()));
    #endif
    }
  else // binding_type == BindingType_Event
    {
    ue_function_p->FunctionFlags |= FUNC_BlueprintEvent | FUNC_Event;
    ue_function_p->Bind(); // Bind to default Blueprint event mechanism
    #if WITH_EDITOR
      ue_function_p->SetMetaData(TEXT("Tooltip"), *FString::Printf(TEXT("Triggered by SkookumScript method\n%S@%S()"), sk_method_p->get_scope()->get_name_cstr(), sk_method_p->get_name_cstr()));
    #endif    
    }

  #if WITH_EDITOR
    ue_function_p->SetMetaData(TEXT("Category"), TEXT("SkookumScript"));
  #endif

  // Parameters of the method we are creating
  const SkParameters & params = sk_method_p->get_params();
  const tSkParamList & param_list = params.get_param_list();
  uint32_t num_params = param_list.get_length();

  // Handle return value if any
  if (params.get_result_class() && params.get_result_class() != SkObject::ms_class_p)
    {
    UProperty * result_param_p = build_ue_param(ue_function_p, params.get_result_class(), "result", out_param_info_array_p ? out_param_info_array_p + num_params : nullptr);
    if (!result_param_p)
      {
      // If any parameters can not be mapped, skip building this entire function
      ue_function_p->ConditionalBeginDestroy();
      return nullptr;
      }

    result_param_p->PropertyFlags |= CPF_ReturnParm; // Flag as return value
    }
  else if (out_param_info_array_p)
    {
    // If there is no return value, indicate that by zeroing that entry in the param info array
    memset(out_param_info_array_p + num_params, 0, sizeof(*out_param_info_array_p));
    }

  // Handle input parameters (in reverse order so they get linked into the list in proper order)
  for (int32_t i = num_params - 1; i >= 0; --i)
    {
    const SkParameterBase * input_param = param_list(i);
    if (!build_ue_param(ue_function_p, input_param->get_expected_type(), input_param->get_name_cstr(), out_param_info_array_p ? out_param_info_array_p + i : nullptr))
      {
      // If any parameters can not be mapped, skip building this entire function
      ue_function_p->ConditionalBeginDestroy();
      return nullptr;
      }
    }

  // Make method known to its class
  ue_class_p->LinkChild(ue_function_p);
  ue_class_p->AddFunctionToFunctionMap(ue_function_p);

  // Make sure parameter list is properly linked and offsets are set
  ue_function_p->StaticLink(true);

  return ue_function_p;
  }

//---------------------------------------------------------------------------------------

UProperty * SkUEBlueprintInterface::build_ue_param(UFunction * ue_function_p, SkClassDescBase * sk_parameter_class_p, const FName & param_name, ParamInfo * out_param_info_p)
  {
  // Based on Sk type, figure out the matching UProperty as well as fetcher and setter methods
  UProperty * property_p = nullptr;
  tK2ParamFetcher k2_param_fetcher_p = nullptr;
  tSkValueGetter sk_value_getter_p = nullptr;
  if (sk_parameter_class_p == SkBoolean::ms_class_p)
    {
    property_p = NewObject<UBoolProperty>(ue_function_p, param_name, RF_Public);
    k2_param_fetcher_p = &fetch_k2_param_boolean;
    sk_value_getter_p = &get_sk_value_boolean;
    }
  else if (sk_parameter_class_p == SkInteger::ms_class_p)
    {
    property_p = NewObject<UIntProperty>(ue_function_p, param_name, RF_Public);
    k2_param_fetcher_p = &fetch_k2_param_integer;
    sk_value_getter_p = &get_sk_value_integer;
    }
  else if (sk_parameter_class_p == SkReal::ms_class_p)
    {
    property_p = NewObject<UFloatProperty>(ue_function_p, param_name, RF_Public);
    k2_param_fetcher_p = &fetch_k2_param_real;
    sk_value_getter_p = &get_sk_value_real;
    }
  else if (sk_parameter_class_p == SkString::ms_class_p)
    {
    property_p = NewObject<UStrProperty>(ue_function_p, param_name, RF_Public);
    k2_param_fetcher_p = &fetch_k2_param_string;
    sk_value_getter_p = &get_sk_value_string;
    }
  else if (sk_parameter_class_p == SkVector3::ms_class_p)
    {
    UStructProperty * struct_property_p = NewObject<UStructProperty>(ue_function_p, param_name);
    struct_property_p->Struct = m_struct_vector3_p;
    property_p = struct_property_p;
    k2_param_fetcher_p = &fetch_k2_param_vector3;
    sk_value_getter_p = &get_sk_value_vector3;
    }
  else if (sk_parameter_class_p == SkRotationAngles::ms_class_p)
    {
    UStructProperty * struct_property_p = NewObject<UStructProperty>(ue_function_p, param_name);
    struct_property_p->Struct = m_struct_rotation_angles_p;
    property_p = struct_property_p;
    k2_param_fetcher_p = &fetch_k2_param_rotation_angles;
    sk_value_getter_p = &get_sk_value_rotation_angles;
    }
  else if (sk_parameter_class_p == SkTransform::ms_class_p)
    {
    UStructProperty * struct_property_p = NewObject<UStructProperty>(ue_function_p, param_name);
    struct_property_p->Struct = m_struct_transform_p;
    property_p = struct_property_p;
    k2_param_fetcher_p = &fetch_k2_param_transform;
    sk_value_getter_p = &get_sk_value_transform;
    }
  else if (sk_parameter_class_p->get_key_class()->is_class(*SkUEEntity::ms_class_p))
    {
    UClass * uclass_p = SkUEClassBindingHelper::get_ue_class_from_sk_class(sk_parameter_class_p);
    SK_ASSERTX(uclass_p, a_cstr_format("Class '%s' of parameter '%s' of method '%S.%S' being exported to Blueprints is not a known engine class.", sk_parameter_class_p->get_key_class_name().as_cstr_dbg(), param_name.GetPlainANSIString(), *ue_function_p->GetOwnerClass()->GetName(), *ue_function_p->GetName()));
    if (uclass_p)
      {
      property_p = NewObject<UObjectProperty>(ue_function_p, param_name, RF_Public);
      static_cast<UObjectProperty *>(property_p)->PropertyClass = uclass_p;
      k2_param_fetcher_p = &fetch_k2_param_entity;
      sk_value_getter_p = &get_sk_value_entity;
      }
    }
  else
    {
    //SK_ASSERTX(false, a_cstr_format("Class '%s' of parameter '%s' of method '%S.%S' being exported to Blueprints can not be mapped to a Blueprint-compatible type.", sk_parameter_class_p->get_key_class_name().as_cstr_dbg(), param_name.GetPlainANSIString(), *ue_function_p->GetOwnerClass()->GetName(), *ue_function_p->GetName()));
    }

  // Add flags
  if (property_p)
    {
    property_p->PropertyFlags |= CPF_Parm;
    ue_function_p->LinkChild(property_p);
    }

  // Set result
  if (out_param_info_p)
    {
    out_param_info_p->m_ue_param_p = property_p;
    out_param_info_p->m_k2_param_fetcher_p = k2_param_fetcher_p;
    out_param_info_p->m_sk_value_getter_p = sk_value_getter_p;
    }

  return property_p;
  }

//---------------------------------------------------------------------------------------

void SkUEBlueprintInterface::bind_event_method(SkMethodBase * sk_method_p)
  {
  SK_ASSERTX(!sk_method_p->is_bound() || static_cast<SkMethodFunc *>(sk_method_p)->m_atomic_f == &mthd_trigger_event, a_str_format("Trying to bind Blueprint event method '%s' but it is already bound to a different atomic implementation!", sk_method_p->get_name_cstr_dbg()));
  if (!sk_method_p->is_bound())
    {
    sk_method_p->get_scope()->register_method_func(sk_method_p->get_name(), &mthd_trigger_event, SkBindFlag_instance_no_rebind);
    }
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
