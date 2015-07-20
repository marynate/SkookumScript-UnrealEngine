//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript Name (= FName) class
//
// Author: Markus Breyer
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkClassBinding.hpp>
#include <NameTypes.h>

//---------------------------------------------------------------------------------------
// SkookumScript Name (= FName) class
class SkUEName : public SkClassBindingSimple<SkUEName, FName>
  {
  public:
    static void register_bindings();
  };
