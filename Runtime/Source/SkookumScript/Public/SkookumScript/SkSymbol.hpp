//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001-2015 Agog Labs Inc. All rights reserved.
//
// Manages Symbols for SkookumScript
//
// Author: Conan Reis
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkClassBinding.hpp>
#include <AgogCore/ASymbolTable.hpp>

//---------------------------------------------------------------------------------------
// SkookumScript atomic Symbol class
class SkSymbol : public SkClassBindingSimple<SkSymbol, ASymbol>
  {
  public:

    static void     register_bindings();

  };
