//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2015 Agog Labs Inc.,
// All rights reserved.
//
// Mind object - tracks and updates coroutines.
// 
// Author(s): Conan Reis
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkClass.hpp>


//=======================================================================================
// Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Get name of mind object. Most (if not all) mind objects are singletons so just borrow
// name from class.
A_INLINE const ASymbol & SkMind::get_name() const
  {
  return m_class_p->get_name();
  }

//---------------------------------------------------------------------------------------
// Stop tracking the specified invoked coroutine
//
// Author(s): Conan Reis
A_INLINE void SkMind::coroutine_track_stop(SkInvokedCoroutine * icoro_p)
  {
  coroutine_track_updating_stop(icoro_p);
  coroutine_track_pending_stop(icoro_p);
  }

