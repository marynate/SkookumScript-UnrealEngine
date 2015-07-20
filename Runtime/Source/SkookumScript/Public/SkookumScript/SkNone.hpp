//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001-2015 Agog Labs Inc. All rights reserved.
//
// Special None class for the single nil instance.
//
// Author: Conan Reis
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkClassBindingAbstract.hpp>
#include <SkookumScript/SkSymbolDefs.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
class SkNone : public SkClassBindingAbstract<SkNone>, public SkInstanceUnreffed
  {
  public:
    
    SK_NEW_OPERATORS(SkNone);
    SkNone() : SkInstanceUnreffed(ms_class_p) { SK_ASSERTX(ms_class_p, "Class instance must be non-null here."); }

    static void initialize() { SkClassBindingAbstract::initialize(ASymbolId_None); }

  };  // SkNone
