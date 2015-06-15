//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Class wrapper for info used to make a method call/invocation - i.e. method
//             identifier (name/index) and passed argument info.
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkSymbolDefs.hpp>


//=======================================================================================
// Inline Methods
//=======================================================================================

#if (SKOOKUM & SK_CODE_IN)


//---------------------------------------------------------------------------------------
// Determines if method to call is a constructor.
// Returns:    true if constructor, false if not
// Author(s):   Conan Reis
inline bool SkMethodCall::is_constructor() const
  {
  return ((m_scope_p == nullptr)
    && (m_name != ASymbolX_dtor)
    && (m_name.as_cstr_dbg()[0] == '!'));
  }


#endif // (SKOOKUM & SK_CODE_IN)

