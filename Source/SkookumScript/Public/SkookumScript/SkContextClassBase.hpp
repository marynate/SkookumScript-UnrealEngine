//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Object class with extra context abstract base class
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKCONTEXTCLASS_HPP
#define __SKCONTEXTCLASS_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/ARefCount.hpp>
#include <SkookumScript/SkClassDescBase.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// Object class descriptor that has an additional context/type info applied to it as a
// component or sub elements as well as a traditional primary class.
//
//   context-class = list-class | closure-class
//
// #Author(s) Conan Reis
class SkContextClassBase : public SkClassUnaryBase, public ARefCountMix<SkContextClassBase>
  {
  public:

  // Common Methods

    SK_NEW_OPERATORS(SkContextClassBase);

    explicit SkContextClassBase(SkClass * class_p)         : m_class_p(class_p) {}
    SkContextClassBase(const SkContextClassBase & tclass)  : m_class_p(tclass.m_class_p) {}

  // Methods

    // Overriding from SkClassUnaryBase, SkClassDescBase, ARefCountMix<>

      virtual void reference() const;
      virtual void dereference();
      virtual void dereference_delay() const;
      virtual void on_no_references() = 0;

      virtual SkClassDescBase * get_data_type(const ASymbol & data_name, eSkScope * scope_p = nullptr) const;
      virtual SkClass *         get_key_class() const;
      virtual const ASymbol &   get_key_class_name() const;
      virtual SkMetaClass &     get_metaclass() const;

      // Method Member Methods

        virtual void           append_method(SkMethodBase * method_p);
        virtual SkMethodBase * get_method(const ASymbol & method_name) const;
        virtual SkMethodBase * get_method_inherited(const ASymbol & method_name) const;
        virtual bool           is_method_inherited_valid(const ASymbol & method_name) const;
        virtual bool           is_method_valid(const ASymbol & method_name) const;

      // Method Member Methods

        virtual void              append_coroutine(SkCoroutineBase * coroutine_p);
        virtual SkCoroutineBase * get_coroutine_inherited(const ASymbol & coroutine_name) const;
        virtual bool              is_coroutine_valid(const ASymbol & coroutine_name) const;
        virtual bool              is_coroutine_registered(const ASymbol & coroutine_name) const;

      // Data Member Methods

        virtual SkTypedName * append_data_member(const ASymbol & name, SkClassDescBase * type_p);

  protected:

  // Internal Methods

    SkContextClassBase() {}

  // Data Members

    // This is the primary class that is being wrapped around.  For example, given the
    // typed class List{String} the primary class would be List.
    SkClass * m_class_p;

  };  // SkContextClassBase


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkContextClassBase.inl>
#endif


#endif  // __SKCONTEXTCLASS_HPP

