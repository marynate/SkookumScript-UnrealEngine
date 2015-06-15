//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001-2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript atomic Enumeration class
//
// Author: Conan Reis
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkClassBinding.hpp>

//---------------------------------------------------------------------------------------
// SkookumScript atomic Enumeration class
class SkEnum : public SkClassBindingSimpleZero<SkEnum, SkEnumType>
  {
  public:

    // Class Methods

    static SkInstance *   new_instance(SkEnumType value, SkClass * enum_class_p);
    static SkTypedData *  get_class_data_with_enum(SkClass * enum_class_p, SkEnumType enum_value);

    static void           register_bindings();

  };

//=======================================================================================
// Inline Methods
//=======================================================================================

inline SkInstance * SkEnum::new_instance(SkEnumType value, SkClass * enum_class_p)
  {
  SkInstance * instance_p = SkInstance::new_instance(enum_class_p);

  instance_p->construct<SkEnum>(value);

  return instance_p;
  }
