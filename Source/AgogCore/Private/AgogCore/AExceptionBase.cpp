//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2000 Agog Labs Inc.,
// All rights reserved.
//
//  AException base class definition module
// Author(s):    Conan Reis
// Create Date:   2000-01-06
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AExceptionBase.hpp>


//=======================================================================================
// Method Definitions
//=======================================================================================

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Common Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
//  Default constructor
// Returns:     a new AExceptionBase
// Notes:       So far, this method has no purpose other than to be a place to
//              put a breakpoint to stop on all exceptions.
// Author(s):    Conan Reis
AExceptionBase::AExceptionBase()
  {
  // Breakpoint me!
  //A_BREAK();  // This should be more useful than annoying
  }

//---------------------------------------------------------------------------------------
//  Destructor
// Notes:       Ensures that the proper destructor call is invoked virtually for any
//              class derived from AExceptionBase.
// Author(s):    Conan Reis
AExceptionBase::~AExceptionBase()
  {
  // Does nothing
  }

