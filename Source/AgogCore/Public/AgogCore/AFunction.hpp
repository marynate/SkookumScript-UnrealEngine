//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2000 Agog Labs Inc.,
// All rights reserved.
//
//  AFunction class declaration header
// Author(s):    Conan Reis
// Create Date:   2000-01-11
// Notes:          
//=======================================================================================


#ifndef __AFUNCTION_HPP
#define __AFUNCTION_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AFunctionBase.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// Notes    This is a function callback object.  This class is used in the place of a
//          function argument since it can also use data included with it in any
//          evaluations that take place (if derived).  The fact that the data that the
//          function acts upon is stored with this functional object is especially
//          important for concurrent processing in that it does not rely on global data
//          and thus may operate safely in more than one thread of execution
//          simultaneously.
// UsesLibs    
// Inlibs   AgogCore/AgogCore.lib
// Examples:    
// Author   Conan Reis
class AFunction : public AFunctionBase
  {
  public:
  // Common Methods

    AFunction(void (*function_f)() = nullptr);
    AFunction(const AFunction & func);
    AFunction & operator=(const AFunction & func);
    
    AFUNC_COPY_NEW_DEF(AFunctionBase)  // Adds copy_new()

  // Accessor Methods

    void set_function(void (*function_f)());

  // Modifying Methods

    void invoke();

  protected:
  // Data Members

    // Address of function to call
    void (* m_function_f)();

  };  // AFunction


//=======================================================================================
// Inline Functions
//=======================================================================================


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Common Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Default constructor
// Returns:    itself
// Arg         function_f - the function to be used, if it is nullptr it is not called
// Examples:   AFunction func(comparison_func);
// Author(s):   Conan Reis
inline AFunction::AFunction(void (*function_f)()) :
  m_function_f(function_f)
  {
  }

//---------------------------------------------------------------------------------------
// Copy constructor
// Returns:    itself
// Arg         func - AFunction to copy
// Examples:   AFunction func;
//             AFunction func2(func);
// Author(s):   Conan Reis
inline AFunction::AFunction(const AFunction & func) :
  m_function_f(func.m_function_f)
  {
  }

//---------------------------------------------------------------------------------------
// Assignment operator
// Returns:    a reference to itself to allow assignment stringization
//             func1 = func2 = func3;
// Arg         func - AFunction to copy
// Examples:   func1 = func2;
// Author(s):   Conan Reis
inline AFunction & AFunction::operator=(const AFunction & func)
  {
  m_function_f = func.m_function_f;

  return *this;
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Accessor Methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//---------------------------------------------------------------------------------------
// Sets the function pointer.
// Arg         function_f - a function pointer
// Examples:   func.set_function(some_func);
// Author(s):   Conan Reis
inline void AFunction::set_function(void (*function_f)())
  {
  m_function_f = function_f;
  }


#endif  // __AFUNCTION_HPP


