//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Qualifier class - full qualification consisting of member name and owner class
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKQUALIFIER_HPP
#define __SKQUALIFIER_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/ANamed.hpp>
#include <SkookumScript/SkookumScript.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

class SkClass;  // Pre-declaration


//---------------------------------------------------------------------------------------
// Notes      Qualifier class - full qualification consisting of member name and owner class
// Subclasses SkInvokableBase(SkMethodBase, SkCoroutineBase),
//            SkInvokeBase(SkMethodCall, SkCoroutineCall)
// See Also   SkTypedName
// Author(s)  Conan Reis
class SkQualifier : public ANamed
  {
  friend class SkQualifierCompareName;

  public:
	  SK_NEW_OPERATORS(SkQualifier);
  // Common Methods

    explicit SkQualifier(const ASymbol & name = ASymbol::get_null(), SkClass * scope_p = nullptr);
    SkQualifier(const SkQualifier & source) : ANamed(source), m_scope_p(source.m_scope_p) {}

    SkQualifier & operator=(const SkQualifier & source) { m_name = source.m_name; m_scope_p = source.m_scope_p; return *this; }

  // Comparison Methods

    bool operator==(const SkQualifier & qual) const;
    bool operator<(const SkQualifier & qual) const;
    bool equal_ids(const SkQualifier & qual) const;
    bool equal_ids_scope_name(const SkQualifier & qual) const;
    bool less_ids_scope_name(const SkQualifier & qual) const;
    bool less_ids_name_scope(const SkQualifier & qual) const;

  // Methods

    SkClass * get_scope() const;
    void      set_scope(const SkClass * scope_p);
    void      invalidate()                           { m_name.set_null(); m_scope_p = nullptr; }

    // Data Methods - only valid if qualifier represents data member

      SkClassDescBase * get_data_type() const;

  // Class Methods

  protected:

  // Data Members

    // $Revisit - CReis Should probably be SkClassUnaryBase so it could be either
    // SkClass/SkActorClass or SkMetaClass
    SkClass * m_scope_p;

  };  // SkQualifier


//---------------------------------------------------------------------------------------
// This is passed as a second argument to various template classes such as APArray and
// APSorted to provide a mechanism for logical sorting of elements.
//
// Does a logical comparison of the name and then the scope.
class SkQualifierCompareName
  {
  public:
  // Class Methods

    // Returns true if elements are equal
    static bool equals(const SkQualifier & lhs, const SkQualifier & rhs)     { return (lhs.m_name == rhs.m_name) && (lhs.m_scope_p == rhs.m_scope_p); }

    // Returns 0 if equal, < 0 if lhs is less than rhs, and > 0 if lhs is greater than rhs
    static ptrdiff_t comparison(const SkQualifier & lhs, const SkQualifier & rhs);
  };


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkQualifier.inl>
#endif


#endif  // __SKQUALIFIER_HPP

