//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// Binding classes for UE4 
//
// Author: Markus Breyer
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkClassBindingBase.hpp>

//---------------------------------------------------------------------------------------
// Helper class providing useful global variables and static methods
class SkUEClassBindingHelper
  {
  public:
    
    static UWorld *     get_world(); // Get tha world

    static void         reset_static_class_mappings(uint32_t reserve);
    static void         add_static_class_mapping(SkClass * sk_class_p, UClass * ue_class_p);
    static SkClass *    get_sk_class_from_ue_class(UClass * ue_class_p);
    static UClass *     get_ue_class_from_sk_class(SkClassDescBase * sk_class_p);
    static SkClass *    get_object_class(UObject * obj_p, UClass * def_uclass_p = nullptr, SkClass * def_class_p = nullptr); // Determine SkookumScript class from UClass
    static SkInstance * get_actor_component_instance(AActor * actor_p); // Return SkInstance of an actor's SkookumScriptComponent if present, nullptr otherwise

    template<class _BindingClass, typename _DataType>
    static SkInstance * new_instance(const _DataType & value) { return _BindingClass::new_instance(value); }
    template<>
    static SkInstance * new_instance<SkString, FString>(const FString & value) { return SkString::new_instance(AString(*value, value.Len())); }

    template<class _BindingClass, typename _DataType>
    static void         set_from_instance(_DataType * out_value_p, SkInstance * instance_p) { *out_value_p = (_DataType)instance_p->as<_BindingClass>(); }
    template<>
    static void         set_from_instance<SkString, FString>(FString * out_value_p, SkInstance * instance_p) { *out_value_p = FString(instance_p->as<SkString>().as_cstr()); }

    template<class _BindingClass, typename _DataType>
    static void         initialize_list_from_array(SkInstanceList * out_instance_list_p, const TArray<_DataType> & array);

    template<class _BindingClass, typename _DataType>
    static SkInstance * list_from_array(const TArray<_DataType> & array);

    template<class _BindingClass, typename _DataType>
    static void         initialize_array_from_list(TArray<_DataType> * out_array_p, const SkInstanceList & list);

  protected:

  #if WITH_EDITOR
    static UClass *     add_dynamic_class_mapping(SkClassDescBase * sk_class_desc_p);
    static SkClass *    add_dynamic_class_mapping(UBlueprint * blueprint_p);
  #else
    static UClass *     add_static_class_mapping(SkClassDescBase * sk_class_desc_p);
    static SkClass *    add_static_class_mapping(UClass * ue_class_p);
  #endif

    static TMap<UClass*, SkClass*>                            ms_static_class_map_u2s; // Maps UClasses to their respective SkClasses
    static TMap<SkClassDescBase*, UClass*>                    ms_static_class_map_s2u; // Maps SkClasses to their respective UClasses

  #if WITH_EDITOR
    static TMap<UBlueprint*, SkClass*>                        ms_dynamic_class_map_u2s; // Maps Blueprints to their respective SkClasses
    static TMap<SkClassDescBase*, TWeakObjectPtr<UBlueprint>> ms_dynamic_class_map_s2u; // Maps SkClasses to their respective Blueprints
  #endif
  };

//---------------------------------------------------------------------------------------
// Customized version of the UE weak pointer

template<class _UObjectType>
class SkUEWeakObjectPtr
  {
  public:
    SkUEWeakObjectPtr() {}
    SkUEWeakObjectPtr(_UObjectType * obj_p) : m_ptr(obj_p) {}

    bool is_valid() const               { return m_ptr.IsValid(); }
    _UObjectType * get_obj() const      { return m_ptr.Get(); }
    operator _UObjectType * () const    { return m_ptr.Get(); } // Cast myself to UObject pointer so it can be directly assigned to UObject pointer
    _UObjectType * operator -> () const { return m_ptr.Get(); }

  protected:
    TWeakObjectPtr<_UObjectType>  m_ptr;
  };

//---------------------------------------------------------------------------------------
// Binding class encapsulating a (weak pointer to a) UObject
template<class _BindingClass, class _UObjectType>
class SkUEClassBindingEntity : public SkClassBindingBase<_BindingClass, SkUEWeakObjectPtr<_UObjectType>>, public SkUEClassBindingHelper
  {
  public:

    typedef SkUEClassBindingEntity<_BindingClass, _UObjectType> tBindingEntity;

    // Don't generate these per class - inherit copy constructor and assignment operator from SkUEEntity
    enum { Binding_has_ctor      = false }; // If to generate constructor
    enum { Binding_has_ctor_copy = false }; // If to generate copy constructor
    enum { Binding_has_assign    = false }; // If to generate assignment operator
    enum { Binding_has_dtor      = false }; // If to generate destructor

    // Class Data

    static UClass * ms_uclass_p; // Pointer to the UClass belonging to this binding

    // Class Methods

    //---------------------------------------------------------------------------------------
    // Allocate and initialize a new instance of this SkookumScript type
    // We override this so we can properly determine the actual class of the SkInstance
    // which may be a sub class of tBindingAbstract::ms_class_p 
    static SkInstance * new_instance(_UObjectType * obj_p, UClass * def_uclass_p = nullptr, SkClass * def_class_p = nullptr)
      {
      SkInstance * instance_p = SkInstance::new_instance(get_object_class(obj_p, def_uclass_p ? def_uclass_p : ms_uclass_p, def_class_p ? def_class_p : tBindingAbstract::ms_class_p));
      static_cast<tBindingBase *>(instance_p)->construct(obj_p);
      return instance_p;
      }

  };

//---------------------------------------------------------------------------------------
// Binding class encapsulating a (weak pointer to an) AActor
template<class _BindingClass, class _AActorType>
class SkUEClassBindingActor : public SkUEClassBindingEntity<_BindingClass, _AActorType>
  {
  public:

    // Class Methods

    //---------------------------------------------------------------------------------------
    // Allocate and initialize a new instance of this SkookumScript type
    // We override this so we can properly determine the actual class of the SkInstance
    // which may be a sub class of tBindingAbstract::ms_class_p 
    // The actor may also contain its own SkInstance inside its SkookumScriptComponent
    static SkInstance * new_instance(_AActorType * actor_p, UClass * def_uclass_p = nullptr, SkClass * def_class_p = nullptr)
      {
      // Check if we can get an instance from a SkookumScriptComponent
      // If not, create new entity instance
      SkInstance * instance_p = get_actor_component_instance(actor_p);
      if (instance_p)
        {
        instance_p->reference();
        return instance_p;
        }
      SK_ASSERTX(!def_uclass_p || def_uclass_p == ms_uclass_p || def_uclass_p->IsChildOf(ms_uclass_p), "If you pass in def_uclass_p, it must be the same as or a super class of ms_uclass_p.");
      SK_ASSERTX(!def_class_p || def_class_p->is_class(*tBindingAbstract::ms_class_p), "If you pass in def_class_p, it must be the same as or a super class of ms_class_p.");
      return tBindingEntity::new_instance(actor_p, def_uclass_p ? def_uclass_p : ms_uclass_p, def_class_p ? def_class_p : tBindingAbstract::ms_class_p);
      }

  };

//---------------------------------------------------------------------------------------
// Class binding for UStruct
template<class _BindingClass, typename _DataType>
class SkUEClassBindingStruct : public SkClassBindingBase<_BindingClass, _DataType>
  {
  public:

    static UStruct * ms_ustruct_p; // Pointer to the UStruct belonging to this binding
  };

//---------------------------------------------------------------------------------------
// Class binding for UStruct with plain old data (assign/copy with memcpy)
template<class _BindingClass, typename _DataType>
class SkUEClassBindingStructPod : public SkUEClassBindingStruct<_BindingClass,_DataType>
  {
  public:
    // Copy constructor and assignment use memcpy
    static void mthd_ctor_copy(SkInvokedMethod * scope_p, SkInstance ** result_pp) { ::memcpy(&scope_p->this_as<_BindingClass>(), &scope_p->get_arg<_BindingClass>(SkArg_1), sizeof(_BindingClass::tDataType)); }
    static void mthd_op_assign(SkInvokedMethod * scope_p, SkInstance ** result_pp) { ::memcpy(&scope_p->this_as<_BindingClass>(), &scope_p->get_arg<_BindingClass>(SkArg_1), sizeof(_BindingClass::tDataType)); }
  };

//---------------------------------------------------------------------------------------
// Class binding for types with a constructor that takes an EForceInit argument
template<class _BindingClass, typename _DataType>
class SkClassBindingSimpleForceInit : public SkClassBindingBase<_BindingClass, _DataType>
  {
  public:
    // Constructor initializes with ForceInitToZero
    static void mthd_ctor(SkInvokedMethod * scope_p, SkInstance ** result_pp) { scope_p->get_this()->construct<_BindingClass>(ForceInitToZero); }
  };

//=======================================================================================
// Class Data Definitions
//=======================================================================================

//---------------------------------------------------------------------------------------
// Pointer to the UClass belonging to this binding
template<class _BindingClass, class _UObjectType>
UClass * SkUEClassBindingEntity<_BindingClass, _UObjectType>::ms_uclass_p = nullptr;

//---------------------------------------------------------------------------------------
// Pointer to the UStruct belonging to this binding
template<class _BindingClass, typename _DataType>
UStruct * SkUEClassBindingStruct<_BindingClass, _DataType>::ms_ustruct_p = nullptr;

//=======================================================================================
// Inline Function Definitions
//=======================================================================================

//---------------------------------------------------------------------------------------

inline SkClass * SkUEClassBindingHelper::get_sk_class_from_ue_class(UClass * ue_class_p)
  {
  // First see if it's a known static (Engine) class
  SkClass ** sk_class_pp = ms_static_class_map_u2s.Find(ue_class_p);
  if (sk_class_pp) return *sk_class_pp;
  #if WITH_EDITOR
    // If not, it might be a dynamic (Blueprint) class
    UBlueprintGeneratedClass * blueprint_class_p = Cast<UBlueprintGeneratedClass>(ue_class_p);
    if (!blueprint_class_p) return nullptr;
    UBlueprint * blueprint_p = Cast<UBlueprint>(blueprint_class_p->ClassGeneratedBy);
    if (!blueprint_p) return nullptr;
    // It's a blueprint class, see if we know it already
    sk_class_pp = ms_dynamic_class_map_u2s.Find(blueprint_p);
    if (sk_class_pp) return *sk_class_pp;
    // (Yet) unknown, try to look it up by name and add to map
    return add_dynamic_class_mapping(blueprint_p);
  #else
    return add_static_class_mapping(ue_class_p);
  #endif
  }

//---------------------------------------------------------------------------------------

inline UClass * SkUEClassBindingHelper::get_ue_class_from_sk_class(SkClassDescBase * sk_class_p)
  {
  // First see if it's a known static (Engine) class
  UClass ** ue_class_pp = ms_static_class_map_s2u.Find(sk_class_p);
  if (ue_class_pp) return *ue_class_pp;
  #if WITH_EDITOR
    // If not, see if it's a known dynamic (Blueprint) class
    TWeakObjectPtr<UBlueprint> * blueprint_pp = ms_dynamic_class_map_s2u.Find(sk_class_p);
    if (blueprint_pp)
      {
      UBlueprint * blueprint_p = blueprint_pp->Get();
      if (blueprint_p) return blueprint_p->GeneratedClass;
      }
    // (Yet) unknown (or blueprint was rebuilt/reloaded), try to look it up by name and add to map
    return add_dynamic_class_mapping(sk_class_p);
  #else
    return add_static_class_mapping(sk_class_p);
  #endif
  }

//---------------------------------------------------------------------------------------

template<class _BindingClass, typename _DataType>
void SkUEClassBindingHelper::initialize_list_from_array(SkInstanceList * out_instance_list_p, const TArray<_DataType> & array)
  {
  APArray<SkInstance> & list_instances = out_instance_list_p->get_instances();
  list_instances.ensure_size_empty(array.Num());
  for (auto & item : array)
    {
    list_instances.append(*new_instance<_BindingClass, _DataType>(item));
    }
  }

//---------------------------------------------------------------------------------------

template<class _BindingClass, typename _DataType>
SkInstance * SkUEClassBindingHelper::list_from_array(const TArray<_DataType> & array)
  {
  SkInstance * instance_p = SkList::new_instance(array.Num());
  initialize_list_from_array<_BindingClass, _DataType>(&instance_p->as<SkList>(), array);
  return instance_p;
  }

//---------------------------------------------------------------------------------------

template<class _BindingClass, typename _DataType>
void SkUEClassBindingHelper::initialize_array_from_list(TArray<_DataType> * out_array_p, const SkInstanceList & list)
  {
  APArray<SkInstance> & instances = list.get_instances();
  out_array_p->Reserve(instances.get_length());
  for (auto & instance_p : instances)
    {
    uint32 index = out_array_p->AddUninitialized();
    set_from_instance<_BindingClass, _DataType>(&(*out_array_p)[index], instance_p);
    }
  }
