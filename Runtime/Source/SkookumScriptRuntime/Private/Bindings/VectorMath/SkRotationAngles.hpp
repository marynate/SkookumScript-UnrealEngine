//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript Euler angles class
//
// Author: Markus Breyer
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include "../SkUEClassBinding.hpp"
#include <UnrealMath.h> // Vector math functions

//---------------------------------------------------------------------------------------
// SkookumScript Euler angles class
class SkRotationAngles : public SkClassBindingSimpleForceInit<SkRotationAngles, FRotator>
  {
  public:

    static void register_bindings();

  };
