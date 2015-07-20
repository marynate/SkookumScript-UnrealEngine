//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript 2D vector class
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
// SkookumScript 2D vector class
class SkVector2 : public SkClassBindingSimpleForceInit<SkVector2, FVector2D>
  {
  public:

    static void register_bindings();

  };
