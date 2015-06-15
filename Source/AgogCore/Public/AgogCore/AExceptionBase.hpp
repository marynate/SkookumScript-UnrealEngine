//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2000 Agog Labs Inc.,
// All rights reserved.
//
//  AException base class declaration header.
// Author(s):    Conan Reis
// Create Date:   2000-01-06
// Notes:          
//=======================================================================================


#ifndef __AEXCEPTIONBASE_HPP
#define __AEXCEPTIONBASE_HPP


//=======================================================================================
// Includes
//=======================================================================================


//=======================================================================================
// Global Structures
//=======================================================================================

// Some common error/exception id values
// $Note - CReis In the future these could represent general types of exceptions and be bitwise
// or-ed with more specific exception values, so that both general and specific types of
// exceptions would be programmatically discernable.
enum eAErrId
  {
  AErrId_ok,
  AErrId_generic,  // Used on occasions where there is only one exception for a class and making extra exception ids is overkill
  AErrId_unknown,
  AErrId_low_memory,
  AErrId_invalid_request,
  AErrId_invalid_index,
  AErrId_invalid_index_range,
  AErrId_invalid_index_span,
  AErrId_invalid_datum_id,
  AErrId_not_supported,
  AErrId_os_error,
  AErrId_last = 64 // Marker for starting more specific exceptions
  };

// What measure to take after an error
enum eAErrAction
  {
  // Go to recovery area (throw / catch) and:

  AErrAction_quit,        // Quit application
  AErrAction_retry,       // Retry last command
  AErrAction_continue,    // Skip currnent command and continue with next command

  // Go to next line of code - usually unstable

  AErrAction_ignore,      // Ignore and go to next line of code (and hope that app is still stable)
  AErrAction_ignore_all,  // Ignore and go to next line of code and do not perform this test in the future

  // Modifier Flags

  AErrAction__debug_break  = 0x100,

  AErrAction__action_mask  = 0x0FF
  };

///---------------------------------------------------------------------------------------
// Notes      All thrown exceptions should be derived from this base.
// Subclasses AException
// See Also   AEx<>, ADebug, AErrorOutputBase
// UsesLibs   
// InLibs     AgogCore/AgogCore.lib
// Examples:    
// Author(s)  Conan Reis
class AExceptionBase
  {
  public:
  // Common Methods

    AExceptionBase();
    virtual ~AExceptionBase();

  // Modifying Methods

    // This method should supply appropriate values to, call, and then return the value
    // resulting from AErrorOutputBase::get_default().determine_choice(-);
    virtual eAErrAction resolve() = 0;

  };  // AExceptionBase


//=======================================================================================
// AExceptionBase Inline Functions
//=======================================================================================


#endif // __AEXCEPTIONBASE_HPP


