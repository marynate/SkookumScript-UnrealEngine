//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2000 - 2001 Agog Labs Inc.,
// All rights reserved.
//
//  Simple string based exception class definition module
// Author(s):    Conan Reis
// Create Date:   2001-03-08
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AException.hpp>


//=======================================================================================
// Method Definitions
//=======================================================================================

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Common Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
//  AString constructor for simple error which displays a given message and
//              the file name and line where the exception is thrown.  If an exception
//              level is not given, it defaults to an informational exception.
// Returns:     itself
// Arg          err_id - Numeric id for programmatic identification of the specific
//              exception - usually from an enumerated type or the address of the
//              excepting function
// Arg          action - Suggested course of action to resolve this error.  See available
//              choices and there descriptions at eAErrAction.
// Examples:    throw AException(ERR_overflow);
// See:         AEx<>, eAErrAction, AErrorOutputBase, ADebug
// Author(s):    Conan Reis
AException::AException(
  uint32_t    err_id,
  eAErrAction action // = AErrAction_continue
  ) :
  m_err_id(err_id),
  m_action(action)
  {
  //BREAK();  // This should be more useful than annoying
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Modifying Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
//  Deals with the exception and determines what action to take - this has
//              usually already been determined by the current Error output object.
// Returns:     Type of action to take
// Modifiers:    virtual
// Author(s):    Conan Reis
eAErrAction AException::resolve()
  {
  return m_action;
  }

