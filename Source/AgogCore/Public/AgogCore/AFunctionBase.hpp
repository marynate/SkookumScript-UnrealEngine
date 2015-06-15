//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2000 Agog Labs Inc.,
// All rights reserved.
//
//  AFunctionBase class declaration header
// Author(s):    Conan Reis
// Create Date:   2000-01-11
// Notes:          
//=======================================================================================


#ifndef __AFUNCTIONBASE_HPP
#define __AFUNCTIONBASE_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AgogCore.hpp>


//=======================================================================================
// Global Macros / Defines
//=======================================================================================

// Returns a new dynamic copy of a class instance that is or is derived from the class
// specified by _BaseClass.  Used virtually when a dynamic copy is needed, but the class
// is unknown.
#define AFUNC_COPY_NEW_DEF(_BaseClass) public: virtual _BaseClass * copy_new() const;

//---------------------------------------------------------------------------------------
//  Returns a new dynamic copy of itself.  Used virtually when a dynamic copy
//              is needed, but the class is unknown.
// Returns:     new dynamic copy of itself
// Examples:     _BaseClass * new_obj_p = some_obj.copy_new();
// Notes:       The class indicated by _FuncClass must have a copy constructor.
// Author(s):    Conan Reis
#define AFUNC_COPY_NEW_DECL(_FuncClass, _BaseClass) \
  _BaseClass * _FuncClass::copy_new() const \
    { \
    _BaseClass * func_obj_p = (new _FuncClass(*this)); \
    A_VERIFY_MEMORY(func_obj_p != nullptr, _FuncClass); \
    return func_obj_p; \
    } 


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// Notes     This is a function callback object.  This class is used in the place of a
//           function argument since it can also use data included with it in any
//           evaluations that take place (if derived).  The fact that the data that the
//           function acts upon is stored with this functional object is especially
//           important for concurrent processing in that it does not rely on global data
//           and thus may operate safely in more than one thread of execution
//           simultaneously.
// Parents  
// Uses     
// InLib     AgogCore/AgogCore.lib
// Examples:    
// Author(s) Conan Reis
class AFunctionBase
  {
  public:
  // Common Methods

    virtual ~AFunctionBase();

  // Modifying Methods

    virtual void invoke() = 0;

  // Non-Modifying Methods

    virtual AFunctionBase * copy_new() const = 0;

  };  // AFunctionBase


//=======================================================================================
// Inline Functions
//=======================================================================================


#endif  // __AFUNCTIONBASE_HPP



