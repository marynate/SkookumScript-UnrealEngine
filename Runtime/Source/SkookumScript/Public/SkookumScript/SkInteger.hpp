//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001-2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript atomic Integer class
//
// Author: Conan Reis
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkClassBinding.hpp>

//---------------------------------------------------------------------------------------
// SkookumScript atomic Integer class
class SkInteger : public SkClassBindingSimpleZero<SkInteger, SkIntegerType>
  {
  public:

    static void register_bindings();

  };


