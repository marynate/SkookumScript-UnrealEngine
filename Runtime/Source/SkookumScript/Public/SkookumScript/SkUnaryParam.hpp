//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Single parameter class
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKUNARYPARAM_HPP
#define __SKUNARYPARAM_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/ARefCount.hpp>
#include <SkookumScript/SkParameterBase.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

class SkParser;          // Pre-declaration
class SkExpressionBase;  // Pre-declaration

//---------------------------------------------------------------------------------------
// Notes      Single parameter class
// Author(s)  Conan Reis
class SkUnaryParam : public SkParameterBase
  {
  friend class SkParser;  // For quick access during construction with parsing

  public:
	  SK_NEW_OPERATORS(SkUnaryParam);
  // Common Methods

    explicit SkUnaryParam(const ASymbol & name = ASymbol::get_null(), SkClassDescBase * type_p = nullptr, SkExpressionBase * default_p = nullptr);
    SkUnaryParam(SkUnaryParam * uparam_p);
    virtual ~SkUnaryParam();

  // Comparison Methods
  
    virtual bool compare_equal(const SkParameterBase & param) const;
    virtual bool compare_less(const SkParameterBase & param) const;

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkUnaryParam(eSkParameter type, uint8_t type_info, const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if defined(SK_AS_STRINGS)
      AString as_code() const;
    #endif


  // Methods

    virtual SkExpressionBase * get_default_expr() const { return m_default_p; }
    virtual eSkParameter       get_kind() const;
    virtual bool               is_defaultable() const;
    void                       set_default_expr(SkExpressionBase * expr_p);
    virtual void               track_memory(AMemoryStats * mem_stats_p) const;

    // Type-checking Methods

      virtual SkParameterBase * as_finalized_generic(const SkClassDescBase & scope_type) const;
      virtual SkClassDescBase * get_expected_type() const;
      virtual bool              is_generic() const;

  protected:

  // Data Members

    // Used as a hint to parser/compiler so that arguments passed are of the type expected.
    // $Revisit - CReis In theory this hint should not be needed during run-time if not
    // debugging or parsing - i.e. if only SK_COMPILED_IN is defined.  Currently only used
    // if SK_CODE_IN, SK_CODE_OUT or SK_COMPILED_OUT is defined.]
    ARefPtr<SkClassDescBase> m_type_p;

    // Optional default expression if argument is omitted for this parameter.  If this is
    // nullptr, then there is no default expression available and an argument is required
    // for this parameter.
    // $Revisit - CReis Could make separate versions where one has default and one does
    // not.  This could save 4 bytes of memory per unary argument on about 90% of the
    // arguments which could be several K of memory savings.
    SkExpressionBase * m_default_p;

  };  // SkUnaryParam


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkUnaryParam.inl>
#endif


#endif  // __SKUNARYPARAM_HPP

