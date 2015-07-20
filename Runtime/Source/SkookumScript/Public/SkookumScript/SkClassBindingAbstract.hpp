//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001-2015 Agog Labs Inc. All rights reserved.
//
// Binding class for objects that are the data themselves
//
// Author: Markus Breyer
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkookumScript.hpp>
#include <SkookumScript/SkBrain.hpp>
#include <SkookumScript/SkClass.hpp>
#include <SkookumScript/SkDebug.hpp>

//---------------------------------------------------------------------------------------
// Default class binding for data-less types (e.g. None and Object)
template<class _BindingClass>
class SkClassBindingAbstract
  {
  public:

    // Types

    typedef _BindingClass tDataType; // Default data type is itself

    // Class Data

    static SkClass * ms_class_p; // Pointer to the SkookumScript class belonging to this binding

    //---------------------------------------------------------------------------------------
    // Default data is the object itself
    const _BindingClass & get_data() const  { return *static_cast<const _BindingClass *>(this); }
          _BindingClass & get_data()        { return *static_cast<_BindingClass *>(this); }

  protected:

    static void initialize(ASymbol class_name);
    static void initialize(const char * class_name_p)  { initialize(ASymbol::create_existing(class_name_p)); }
    static void initialize(uint32_t class_name_id)     { initialize(ASymbol::create_existing(class_name_id)); }
  };



//=======================================================================================
// Class Data Definitions
//=======================================================================================

// Pointer to Sk class is stored here
template<class _BindingClass>
SkClass * SkClassBindingAbstract<_BindingClass>::ms_class_p;

//=======================================================================================
// Class Method Implementations
//=======================================================================================

//---------------------------------------------------------------------------------------
// Initialize class information
template<class _BindingClass>
void SkClassBindingAbstract<_BindingClass>::initialize(ASymbol class_name)
  {
  // Initialize class information
  ms_class_p = SkBrain::get_class(class_name);
  SK_ASSERTX(ms_class_p, a_str_format("Tried to initialize class bindings for '%s' but it is unknown!", class_name.as_cstr_dbg()));
  }

