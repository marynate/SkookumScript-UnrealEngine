//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Identifier for named code objects - i.e. temporary variables, etc.
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKIDENTIFIER_HPP
#define __SKIDENTIFIER_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkExpressionBase.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// Notes      SkookumScript Identifier - for local variables and arguments [and
//            potentially variables higher on the call stack].
//            Note that the 'reserved' identifiers "this", "this_class", "this_code",
//            "nil", and classes are handled by SkookumScript/SkLiteral.
// Subclasses SkIdentifierMember(SkIdentifierClassMember)
// See Also   SkLiteral
// Author(s)  Conan Reis
class SkIdentifier : public SkExpressionBase
  {
  public:
  // Common Methods

    SK_NEW_OPERATORS(SkIdentifier);

    SkIdentifier(const ASymbol & ident_name);

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkIdentifier(const void ** binary_pp);
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

    const ASymbol & get_name() const     { return m_ident_name; }
    virtual bool    is_local() const;

    virtual void bind_data(SkInstance * data_p, SkObjectBase * scope_p, SkInvokedBase * caller_p, bool return_result = false);
    #if (SKOOKUM & SK_DEBUG)
      virtual void bind_data(SkBind * bind_p, SkInstance * data_p, SkObjectBase * scope_p, SkInvokedBase * caller_p, bool return_result = false);
    #endif

    virtual eSkExprType     get_type() const;
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

  protected:

  // Data Members

    ASymbol m_ident_name;

  };  // SkIdentifier


//---------------------------------------------------------------------------------------
// Notes      SkookumScript Identifier for instance data members.
// See Also   SkIdentifier, SkIdentifierClassMember
// Author(s)  Conan Reis
class SkIdentifierMember : public SkIdentifier
  {
  public:
  // Common Methods

    SK_NEW_OPERATORS(SkIdentifierMember);

    SkIdentifierMember(const ASymbol & member_name, SkExpressionBase * owner_p);
    virtual ~SkIdentifierMember();

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkIdentifierMember(const void ** binary_pp);
    #endif


    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
    #endif


    #if (SKOOKUM & SK_CODE_IN)
      virtual eSkSideEffect get_side_effect() const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual bool is_local() const;

    virtual void bind_data(SkInstance * data_p, SkObjectBase * scope_p, SkInvokedBase * caller_p, bool return_result = false);
    #if (SKOOKUM & SK_DEBUG)
      virtual void bind_data(SkBind * bind_p, SkInstance * data_p, SkObjectBase * scope_p, SkInvokedBase * caller_p, bool return_result = false);
    #endif

    virtual eSkExprType     get_type() const;
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual void            null_receiver(SkExpressionBase * receiver_p);
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

  // Debugging Methods

    #if (SKOOKUM & SK_DEBUG)
      virtual SkExpressionBase * find_expr_by_pos(uint pos, eSkExprFind type = SkExprFind_all) const;
      virtual eAIterateResult    iterate_expressions(SkApplyExpressionBase * apply_expr_p, const SkInvokableBase * invokable_p = nullptr);
    #endif

  protected:

  // Data Members

    // The object that results from the evaluation of this expression is the owner of the
    // instance member identifier 'm_ident_name'.  If 'm_owner_p' is nullptr then 'this'
    // - i.e. the topmost scope - is inferred.
    SkExpressionBase * m_owner_p;

  };  // SkIdentifierMember


//---------------------------------------------------------------------------------------
// Notes      SkookumScript Identifier for class data members.
//            SkIdentifierMember could have been used instead of this class, but this
//            class skips the look-up in the instance data members.
// See Also   SkIdentifier, SkIdentifierMember
// Author(s)  Conan Reis
class SkIdentifierClassMember : public SkIdentifierMember
  {
  public:
  // Common Methods

   SK_NEW_OPERATORS(SkIdentifierClassMember);

   SkIdentifierClassMember(const ASymbol & class_member_name, SkExpressionBase * owner_p);

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_IN)
      SkIdentifierClassMember(const void ** binary_pp);
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Methods

    virtual void bind_data(SkInstance * data_p, SkObjectBase * scope_p, SkInvokedBase * caller_p, bool return_result = false);
    #if (SKOOKUM & SK_DEBUG)
      virtual void bind_data(SkBind * bind_p, SkInstance * data_p, SkObjectBase * scope_p, SkInvokedBase * caller_p, bool return_result = false);
    #endif

    virtual eSkExprType     get_type() const;
    virtual SkInvokedBase * invoke(SkObjectBase * scope_p, SkInvokedBase * caller_p = nullptr, SkInstance ** result_pp = nullptr) const;
    virtual void            track_memory(AMemoryStats * mem_stats_p) const;

  protected:

  // Data Members

    // The object that results from the evaluation of the 'm_owner' expression is the
    // owner of the class member identifier 'm_ident_name'.  If 'm_owner_p' is nullptr then
    // 'this_class' - i.e. the class of the topmost scope - is inferred.
     
    // $Revisit - CReis [Memory & Speed] Make another version of this identifier for when
    // the owner/receiver is a specific class - i.e. class literal, this or this_class.
    // This should save around 12 bytes per class member identifier.

  };  // SkIdentifierClassMember


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkIdentifier.inl>
#endif


#endif  // __SKIDENTIFIER_HPP

