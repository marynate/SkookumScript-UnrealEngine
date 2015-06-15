//=======================================================================================
// Agog Labs C++ library.
// Copyright (c) 2000 Agog Labs Inc.,
// All rights reserved.
//
// File:        External Overrides for AgogCore - used when Agog libraries hooked into
//				other code bases.
// Author(s):   Conan Reis
// Notes:       
//=======================================================================================

#ifndef __AGOGEXTHOOK_HPP
#define __AGOGEXTHOOK_HPP
#pragma once


//=======================================================================================
// Includes
//=======================================================================================



//=======================================================================================
// Global Macros / Defines
//=======================================================================================

// Indicates that custom memory allocation functions for new/delete are defined in
// external libraries.
#define A_MEMORY_FUNCS_PRESENT

// Show work-in-progress notes via  #pragma A_NOTE()
//#define A_SHOW_NOTES


#endif  // __AGOGEXTHOOK_HPP


