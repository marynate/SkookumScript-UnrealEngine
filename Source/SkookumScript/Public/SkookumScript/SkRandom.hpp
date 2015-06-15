//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001-2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript Atomic Random Number Generator class
//
// Author: Conan Reis
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkClassBinding.hpp>
#include <AgogCore/ARandom.hpp>

//---------------------------------------------------------------------------------------
// SkookumScript Atomic Random Number Generator class.  It is high speed and
// seed driven (allowing the reproduction of generated sequences).
class SkRandom : public SkClassBindingSimple<SkRandom, ARandom>
  {
  public:

    enum { Binding_has_dtor = false }; // If to generate destructor

    static void register_bindings();

  };
