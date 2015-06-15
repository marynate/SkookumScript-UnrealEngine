//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Data structures for class descriptors and class objects
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKTYPEDCLASS_HPP
#define __SKTYPEDCLASS_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/APSorted.hpp>
#include <SkookumScript/SkContextClassBase.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

//---------------------------------------------------------------------------------------
// Notes      SkookumScript Typed Class - a class that has an additional class type
//            applied to it as a component or sub elements.
//
//              list-class = List ['{' ws [class-desc ws] '}']
//
// Author(s)  Conan Reis
class SkTypedClass : public SkContextClassBase
  {
  friend class SkBrain;      // Accesses protected elements
  friend class SkClass;      // Accesses protected elements

  public:

  // Common Methods

    SK_NEW_OPERATORS(SkTypedClass);

    explicit SkTypedClass(SkClass * class_p, SkClassDescBase * item_type_p) : SkContextClassBase(class_p), m_item_type_p(item_type_p) { if (item_type_p) { item_type_p->reference(); } }
    SkTypedClass(const SkTypedClass & tclass)                               : SkContextClassBase(tclass), m_item_type_p(tclass.m_item_type_p) { if (m_item_type_p) { m_item_type_p->reference(); } }
    ~SkTypedClass()                                                         { clear(); }

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_OUT)
      void         as_binary(void ** binary_pp) const;
      uint32_t     as_binary_length() const;
      virtual void as_binary_ref(void ** binary_pp) const;
      virtual uint32_t as_binary_ref_typed_length() const;
    #endif


    #if (SKOOKUM & SK_COMPILED_IN)
      SkTypedClass(const void ** binary_pp)                                 : m_item_type_p(nullptr) { assign_binary(binary_pp); }
      void                  assign_binary(const void ** binary_pp);
      static SkTypedClass * from_binary_ref(const void ** binary_pp);
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Comparison Methods

    eAEquate compare(const SkTypedClass & tclass) const;
    bool     operator==(const SkTypedClass & tclass) const;
    bool     operator!=(const SkTypedClass & tclass) const;
    bool     operator<(const SkTypedClass & tclass) const;
    bool     operator<=(const SkTypedClass & tclass) const;
    bool     operator>(const SkTypedClass & tclass) const;
    bool     operator>=(const SkTypedClass & tclass) const;

  // Methods

    void                      clear();
    virtual SkClassDescBase * get_item_type() const             { return m_item_type_p; }

    // Overriding from SkClassUnaryBase, SkClassDescBase, ARefCountMix<>

      virtual void on_no_references();

      virtual SkClassDescBase *  as_finalized_generic(const SkClassDescBase & scope_type) const;
      virtual SkClassUnaryBase * find_common_type(const SkClassDescBase & cls) const;
      virtual eSkClassType       get_class_type() const;
      virtual bool               is_class_type(const SkClassDescBase * type_p) const;
      virtual bool               is_generic() const;

  // Class Methods

    static APSortedLogicalFree<SkTypedClass> & get_classes()    { return ms_typed_classes; }

    static SkTypedClass * get_or_create(const SkTypedClass & tclass);
    static SkTypedClass * get_or_create(SkClass * class_p, SkClassDescBase * item_type_p);
    static void           shared_pre_empty();
    static void           shared_empty()                        { ms_typed_classes.free_all(); }  
    static void           shared_ensure_references();
    static void           shared_track_memory(AMemoryStats * mem_stats_p);

  protected:

  // Internal Methods

    SkTypedClass()                                              : m_item_type_p(nullptr) {}  // Called by SkBrain

  // Data Members

    // This is the item/element/component type that is associated with the primary class.
    // For example, given the typed class List{String} the element type would be String.
    SkClassDescBase * m_item_type_p;

  // Class Data Members

    // Typed Class objects that are shared amongst various data-structures
    static APSortedLogicalFree<SkTypedClass> ms_typed_classes;

  };  // SkTypedClass


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkTypedClass.inl>
#endif


#endif  // __SKTYPEDCLASS_HPP

