//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2008 Agog Labs Inc.,
// All rights reserved.
//
//  ADeferFunc class definition module
// Author(s):    Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/ADeferFunc.hpp>


//=======================================================================================
// Class Data
//=======================================================================================

APArrayFree<AFunctionBase> ADeferFunc::ms_deferred_funcs;


//=======================================================================================
// Class Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Invokes/calls any previously posted/deferred function objects.
// Notes:      Generally called end of a main loop or frame update.
// Modifiers:   static
// Author(s):   Conan Reis
void ADeferFunc::invoke_deferred()
  {
  uint func_count = ms_deferred_funcs.get_length();

  if (func_count)
    {
    // The functions are called in the order that they were posted
    AFunctionBase ** funcs_pp     = ms_deferred_funcs.get_array();
    AFunctionBase ** funcs_end_pp = funcs_pp + func_count;

    for (; funcs_pp < funcs_end_pp; funcs_pp++)
      {
      (*funcs_pp)->invoke();

      delete *funcs_pp;
      }

    // Only remove the number of function objects invoked rather than just emptying the
    // array since new function objects may have been posted.
    ms_deferred_funcs.remove_all(0u, func_count);
    }
  }
