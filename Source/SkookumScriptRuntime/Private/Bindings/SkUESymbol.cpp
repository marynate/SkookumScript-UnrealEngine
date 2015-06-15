//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2014 Agog Labs Inc.,
// All rights reserved.
//
// File:       Manages Symbols for SkookumScript
// Author(s):  Conan Reis
// Notes:       
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include "../SkookumScriptRuntimePrivatePCH.h"
#include "SkUESymbol.hpp"


//=======================================================================================
// Global Variables
//=======================================================================================

//---------------------------------------------------------------------------------------
// Define ASymbol constants

// Symbol Table to use in macros

#define ASYM(_id)         ASYMBOL_DEFINE(ASymbol, _id)
#define ASYMX(_id, _str)  ASYMBOL_DEFINE_STR(ASymbolX, _id, _str)
  SKUE_SYMBOLS
  SKUE_SYMBOLS_NAMED
#undef ASYMX
#undef ASYM

