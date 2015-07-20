//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001-2015 Agog Labs Inc. All rights reserved.
//
// Base class for binding mechanism class providing default 
//
// Author: Markus Breyer
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkClassBindingBase.hpp>

//---------------------------------------------------------------------------------------
// Class binding for simple types that can auto-construct
template<class _BindingClass, typename _DataType>
class SkClassBindingSimple : public SkClassBindingBase<_BindingClass, _DataType>
  {
  };

//---------------------------------------------------------------------------------------
// Class binding for zero-initialized types that have no destructor
template<class _BindingClass, typename _DataType>
class SkClassBindingSimpleZero : public SkClassBindingBase<_BindingClass, _DataType>
  {
  public:
    // No destructor
    enum { Binding_has_dtor = false };
    // Constructor initializes with zero
    static void mthd_ctor(SkInvokedMethod * scope_p, SkInstance ** result_pp) { scope_p->get_this()->construct<_BindingClass>(_DataType(0)); }
  };
