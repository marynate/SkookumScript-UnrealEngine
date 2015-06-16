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
    
    static TMap<UClass*, SkClass*> ms_class_map_u2s; // Maps UClasses to their respective SkClasses
    static TMap<SkClass*, UClass*> ms_class_map_s2u; // Maps SkClasses to their respective UClasses

    static UWorld *     get_world(); // Get tha world

    static SkClass *    get_object_class(UObject * obj_p, UClass * def_uclass_p = nullptr, SkClass * def_class_p = nullptr); // Determine SkookumScript class from UClass
    static SkInstance * get_actor_component_instance(AActor * actor_p); // Return SkInstance of an actor's SkookumScriptComponent if present, nullptr otherwise

    static UProperty *  find_class_property(UClass * class_p, FName property_name);

  };


//---------------------------------------------------------------------------------------
// Customized version of the UE weak pointer

template<class _UObjectType>
class SkUEWeakObjectPtr
  {
  public:
    SkUEWeakObjectPtr() {}
    SkUEWeakObjectPtr(_UObjectType * obj_p) : m_ptr(obj_p) {}

    bool is_valid() const            { return m_ptr.IsValid(); }
    _UObjectType * get_obj() const   { return m_ptr.Get(); }
    operator _UObjectType * () const { return m_ptr.Get(); } // Cast myself to UObject pointer so it can be directly assigned to UObject pointer

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

//=======================================================================================
// Class Data Definitions
//=======================================================================================

//---------------------------------------------------------------------------------------
// Pointer to the UClass belonging to this binding
template<class _BindingClass, class _UObjectType>
UClass * SkUEClassBindingEntity<_BindingClass, _UObjectType>::ms_uclass_p = nullptr;
