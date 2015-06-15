//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// Additional bindings for the EntityClass (= UClass) class 
//
// Author: Markus Breyer
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <SkUEEntityClass.generated.hpp>

//=======================================================================================
// Global Functions
//=======================================================================================

//---------------------------------------------------------------------------------------
// Bindings for the EntityClass (= UClass) class 
class SkUEEntityClass_Ext : public SkUEEntityClass
  {
  public:
    static void register_bindings();
  };

