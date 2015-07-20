//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// SkookumScript Object Identifier
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKOBJECTID_HPP
#define __SKOBJECTID_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkExpressionBase.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// SkookumScript Object Identifier - used to identify/look-up an object instance based on
// its class and name.
// 
// #Author(s) Conan Reis
class SkObjectId : public SkExpressionBase
  {
  public:

  // Nested structures

    // Class flags - stored in m_flags
    // Currently serialized out as
    enum eFlag
      {
      Flag_optional_find    = 1 << 0,  // May return nil or object
      Flag_optional_literal = 1 << 1,  // Class typed symbol literal
      Flag_use_cached       = 1 << 2,  // Try to use cached obj before doing look-up

      Flag__custom_bit      = 3,       // This bit and up may be used by custom look-up
      Flag__none            = 0,
      Flag__default         = Flag__none
      };


  // Public Data Members

    // Considered deriving SkObjectId from SkQualifier or ANamed though decided to keep
    // its derivation path simple and just make the class and name contained data members.

    // Name of object
    ASymbol m_name;

    // Class to retrieve object from
    SkClass * m_class_p;

    // Cached smart pointer to object
    AIdPtr<SkInstance> m_obj_p;

    // Flags - see eFlags
    uint32_t m_flags;


  // Common Methods

    SK_NEW_OPERATORS(SkObjectId);

    SkObjectId() : m_class_p(nullptr), m_flags(Flag__default) {}
    SkObjectId(const ASymbol & ident_name, SkClass * class_p, uint32_t flags);
    SkObjectId(const SkObjectId & source) : m_name(source.m_name), m_class_p(source.m_class_p), m_obj_p(source.m_obj_p), m_flags(source.m_flags) {}

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkObjectId(const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if (SKOOKUM & SK_CODE_IN)
      virtual const SkExpressionBase * find_expr_last_no_side_effect() const  { return this; }
      virtual eSkSideEffect            get_side_effect() const                { return SkSideEffect_none; }
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    const ASymbol & get_name() const     { return m_name; }

    virtual eSkExprType     get_type() const;
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

    #if (SKOOKUM & SK_CODE_IN)
      SkClass * validate(bool validate_deferred = true);
    #endif

  };  // SkObjectId


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkObjectId.inl>
#endif


#endif  // __SKOBJECTID_HPP

