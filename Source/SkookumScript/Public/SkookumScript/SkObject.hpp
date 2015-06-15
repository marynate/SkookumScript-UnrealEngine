//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001-2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript atomic object class
//
// Author: Markus Breyer
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkClassBindingAbstract.hpp>

//---------------------------------------------------------------------------------------
// SkookumScript atomic object class
class SkObject : public SkClassBindingAbstract<SkObject>, public SkInstance
  {
  public:

    static void register_bindings();

  };


