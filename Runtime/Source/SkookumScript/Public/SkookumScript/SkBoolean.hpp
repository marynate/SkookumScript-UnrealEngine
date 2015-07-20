//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001-2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript atomic Boolean (true/false) class - allows short circuit evaluation.
//
// Author: Conan Reis
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkClassBinding.hpp>


//---------------------------------------------------------------------------------------
// Notes      SkookumScript Atomic Boolean (true/false) - allows short circuit evaluation.
//            Has same data as SkInstance - only differs in that it has a different
//            virtual method table.
class SkBoolean : public SkClassBindingSimpleZero<SkBoolean, SkBooleanType>
  {
  public:

    SK_NEW_OPERATORS(SkBoolean);

  // Methods

  // Class Methods

    static SkBoolean *                new_instance(SkBooleanType bool_value);
    static AObjReusePool<SkBoolean> & get_pool();
    static void                       init_pool(SkClass * bool_class_p);

    static void                       register_bindings();

  protected:

    friend class AObjBlock < SkBoolean > ;  // So it can be accessed only by pool_new()

  // Overridden from SkInstance

    virtual void method_invoke(const SkMethodCall & mcall, SkObjectBase * scope_p, SkInvokedBase * caller_p, SkInstance ** result_pp);
    virtual void on_no_references();
    virtual void delete_this();

  // Overridden from SkObjectBase

    virtual uint32_t get_obj_type() const  { return SkObjectType_boolean; }

  };  // SkBoolean


//=======================================================================================
// Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Retrieves a Boolean instance object from the dynamic pool and initializes
//             it for use.  This method should be used instead of 'new' because it
//             prevents unnecessary allocations by reusing previously allocated objects.
// Returns:    a dynamic SkBoolean
// Arg         bool_value - Should be a bool or 0 or 1
// See:        pool_delete() 
// Notes:      To 'deallocate' an object that was retrieved with this method, use
//             'pool_delete()' rather than 'delete'.
inline SkBoolean * SkBoolean::new_instance(tDataType bool_value)
  {
  SkBoolean * bool_p = get_pool().pop();

  bool_p->construct(bool_value);
  bool_p->m_ref_count = 1u;
  bool_p->m_ptr_id = ++ms_ptr_id_prev;

  return bool_p;
  }
