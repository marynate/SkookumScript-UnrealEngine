//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Data structures for abstract class descriptors
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKCLASSDESCBASE_HPP
#define __SKCLASSDESCBASE_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AMemory.hpp>
#include <SkookumScript/SkookumScript.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declarations
class  AString;
class  ASymbol;
class  SkCoroutineBase;
class  SkMetaClass;
class  SkMethodBase;
class  SkClassUnaryBase;
struct SkTypedName;


//---------------------------------------------------------------------------------------
enum eSkClassType
  {
  SkClassType_class           = 0,  // SkClass
  SkClassType_metaclass       = 1,  // SkMetaClass
  SkClassType_typed_class     = 2,  // SkTypedClass
  SkClassType_invokable_class = 3,  // SkInvokableClass
  SkClassType_class_union     = 4   // SkClassUnion
  };


//---------------------------------------------------------------------------------------
// Notes      SkookumScript Class Abstract Base also known as a class descriptor
//            - "class-desc" from the language syntax:
//
//              class-desc     = class-unary | class-union
//              class-unary    = class-instance | meta-class
//              class-instance = class | list-class
//              class          = class-name
//              list-class     = List ['{' ws [class-desc ws] '}']
//              meta-class     = '<' class-name '>'
//              class-union    = '<' class-unary {'|' class-unary}1+ '>'
//
// Subclasses SkClassUnaryBase(SkClass(SkActorClass), SkTypedClass, SkMetaClass),
//            SkClassUnion
// Author(s)  Conan Reis
class SkClassDescBase 
  {
  friend class SkBrain;  // Accesses protected elements

  public:

  // Nested Structures

    // Enumerated constants
    enum
      {
      Binary_ref_size = 9
      };

    // AEx<SkMetaClass> exception id values
    enum eErrId
      {
      ErrId_nonexistent_data = AErrId_last,  // No such data member exists
      ErrId_nonexistent_method,              // No such method exists
      };

  // Common Methods

    SkClassDescBase() {}
    virtual ~SkClassDescBase() {}

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void as_binary_ref(void ** binary_pp) const = 0;
      void         as_binary_ref_typed(void ** binary_pp) const;
      virtual uint32_t as_binary_ref_typed_length() const  { return 5u; }
    #endif


    #if (SKOOKUM & SK_COMPILED_IN)
      static SkClassDescBase * from_binary_ref(eSkClassType type, const void ** binary_pp);
      static SkClassDescBase * from_binary_ref_typed(const void ** binary_pp);
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const = 0;
      virtual AString get_scope_desc() const;
    #endif


  // Comparison Methods

    eAEquate         compare(const SkClassDescBase & type) const;
    static bool      equals(const SkClassDescBase & lhs, const SkClassDescBase & rhs)     { return lhs.compare(rhs) == AEquate_equal; }
    static ptrdiff_t comparison(const SkClassDescBase & lhs, const SkClassDescBase & rhs) { return ptrdiff_t(lhs.compare(rhs)); }

  // Methods

      virtual void reference() const;
      virtual void dereference();
      virtual void dereference_delay() const;

      // Type Info Methods

        virtual SkClassDescBase *        as_finalized_generic(const SkClassDescBase & scope_type) const = 0;
        virtual const SkClassUnaryBase * as_unary_class() const = 0;
        virtual SkClassUnaryBase *       as_unary_class() = 0;
        virtual SkClassUnaryBase *       find_common_type(const SkClassDescBase & cls) const = 0;
        virtual bool                     is_builtin_actor_class() const;
        virtual bool                     is_actor_instance() const       { return is_builtin_actor_class() && !is_metaclass(); }
        virtual bool                     is_generic() const;
        virtual bool                     is_metaclass() const; 
        virtual bool                     is_class_type(const SkClassDescBase * type_p) const = 0;
        virtual eSkClassType             get_class_type() const = 0;
        virtual SkClassDescBase *        get_item_type() const           { return nullptr; }
        virtual SkClass *                get_key_class() const = 0;
        virtual const ASymbol &          get_key_class_name() const = 0;
        virtual SkMetaClass &            get_metaclass() const = 0;
        SkClassDescBase *                qualify(SkClass * qual_scope_p) const;

      // Data Member Methods

        virtual SkClassDescBase * get_data_type(const ASymbol & data_name, eSkScope * scope_p = nullptr) const = 0;

      // Method Member Methods

        virtual SkMethodBase * get_method(const ASymbol & method_name) const = 0;
        virtual SkMethodBase * get_method_inherited(const ASymbol & method_name) const = 0;
        virtual bool           is_method_inherited_valid(const ASymbol & method_name) const = 0;
        virtual bool           is_method_registered(const ASymbol & method_name) const;

      // Coroutine Member Methods

        virtual SkCoroutineBase * get_coroutine_inherited(const ASymbol & coroutine_name) const = 0;

      // Class Methods

        static void enable_compound_refs(bool use_refs = true)    { ms_compounds_use_ref = use_refs; }

  protected:

      // Class Data Members

      // If set then compound types - SkTypedClass and SkClassUnion - use an index reference
      // rather than a fully described binary.
      static bool ms_compounds_use_ref;

  };  // SkClassDescBase


//---------------------------------------------------------------------------------------
// Notes      This class represents class descriptors that are a single/unary class
//            rather than a union of classes.
//
//              class-unary    = class-instance | meta-class
//              class-instance = class | list-class
//              class          = class-name
//              list-class     = List ['{' ws [class-desc ws] '}']
//              meta-class     = '<' class-name '>'
//
// Subclasses SkClass(SkActorClass), SkTypedClass, SkInvokableClass, SkMetaClass
// Author(s)  Conan Reis
class SkClassUnaryBase : public SkClassDescBase
  {
  public:

  // Nested Structures

    enum ePath
      {
      Path_class      = 0,  // 00 this/current class [always "set"]
      Path_superclass = 1,  // 01
      Path_subclass   = 2,  // 10
      Path_super_sub  = 3   // 11 [CReis Sounds like a sandwich doesn't it?]
      };

  // Methods

    // Type-checking Methods

      virtual const SkClassUnaryBase * as_unary_class() const  { return this; }
      virtual SkClassUnaryBase *       as_unary_class()        { return this; }

    // Method Member Methods

      virtual void append_method(SkMethodBase * method_p) = 0;
      virtual bool is_method_valid(const ASymbol & method_name) const = 0;

    // Coroutine Member Methods

      virtual void append_coroutine(SkCoroutineBase * coroutine_p) = 0;
      virtual bool is_coroutine_valid(const ASymbol & coroutine_name) const = 0;
      virtual bool is_coroutine_registered(const ASymbol & coroutine_name) const = 0;

    // Data Member Methods

      virtual SkTypedName * append_data_member(const ASymbol & name, SkClassDescBase * type_p) = 0;

  };


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkClassDescBase.inl>
#endif


#endif  // __SKCLASSDESCBASE_HPP

