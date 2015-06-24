//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript Unreal Engine Bindings
// 
// Author:  Conan Reis
//=======================================================================================

#pragma once

//---------------------------------------------------------------------------------------
// 
class SkUEBindings
  {
  public:

    // Class Methods

    static void register_all();

  protected:

    static void gather_dynamic_classes_recursively(SkClass * class_p);

  }; // SkUEBindings

