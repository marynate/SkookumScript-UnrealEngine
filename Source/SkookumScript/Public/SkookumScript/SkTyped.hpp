//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Typed name and typed data classes
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKTYPED_HPP
#define __SKTYPED_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/ANamed.hpp>
#include <AgogCore/APSorted.hpp>
#include <AgogCore/ARefCount.hpp>
#include <SkookumScript/SkClassDescBase.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declarations
class SkClassUnaryBase;
class SkInstance;
class SkExpressionBase;


//---------------------------------------------------------------------------------------
// Notes      SkookumScript Typed Name
// Subclasses SkTypedData
// See Also   SkQualifier
// Author(s)  Conan Reis
struct SkTypedName : public ANamed
  {
  public:
	SK_NEW_OPERATORS(SkTypedName);
  // Public Data Members

    // Used as a hint to parser/compiler so that correct data member usage can be assured.
    // $Revisit - CReis In theory this hint should not be needed during run-time if not
    // debugging or parsing - i.e. if only SK_COMPILED_IN is defined.  Currently only used
    // if SK_CODE_IN, SK_CODE_OUT or SK_COMPILED_OUT is defined.]
    ARefPtr<SkClassDescBase> m_type_p;

  // Common Methods

    explicit SkTypedName(const ASymbol & name = ASymbol::get_null(), const SkClassDescBase * type_p = nullptr);
    SkTypedName(const SkTypedName & source);
    ~SkTypedName();

  // Comparison Methods

    bool operator==(const SkTypedName & typed) const  { return (m_name == typed.m_name) && (m_type_p->compare(*typed.m_type_p) == AEquate_equal); }
    bool operator<(const SkTypedName & typed) const   { return (m_name < typed.m_name) || ((m_name == typed.m_name) && (m_type_p->compare(*typed.m_type_p) == AEquate_less)); }

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkTypedName(const void ** binary_pp);
      void assign_binary(const void ** binary_pp);
    #endif

    #if (SKOOKUM & SK_COMPILED_OUT)
      void     as_binary(void ** binary_pp) const;
      uint32_t as_binary_length() const                 { return 4u + m_type_p->as_binary_ref_typed_length(); }
    #endif

  };  // SkTypedName

typedef APSortedLogical<SkTypedName, ASymbol> tSkTypedNames;


//---------------------------------------------------------------------------------------
// Notes      SkookumScript Typed Name Data reference
// See Also   SkData
// Author(s)  Conan Reis
struct SkTypedData : public SkTypedName
  {
  public:
	SK_NEW_OPERATORS(SkTypedData);
  // Public Data Members

    SkInstance * m_data_p;

  // Common Methods

    explicit SkTypedData(const ASymbol & name = ASymbol::get_null(), const SkClassDescBase * type_p = nullptr, SkInstance * instance_p = nullptr);
    SkTypedData(const SkTypedData & source);

    void set_data(SkInstance * data_p);

  // Class Methods

    static void empty_table(APSortedLogical<SkTypedData, ASymbol> * table_p);

  };  // SkTypedData


typedef APSortedLogical<SkTypedData, ASymbol> tSkTypedDatas;


//=======================================================================================
// Inline Functions
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkTyped.inl>
#endif


#endif  // __SKTYPED_HPP

