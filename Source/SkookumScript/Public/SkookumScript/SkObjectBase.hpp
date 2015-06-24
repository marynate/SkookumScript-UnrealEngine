//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// SkookumScript Base Object declaration file
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================

#ifndef __SKOBJECTBASE_HPP
#define __SKOBJECTBASE_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <AgogCore/AIdPtr.hpp>
#include <AgogCore/ANamed.hpp>
#include <SkookumScript/SkookumScript.hpp>


//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declarations
struct SkData;
class  SkInstance;
class  SkInvokedContextBase;

template<class _ObjectType> class AObjBlock;
template<class _ObjectType> class AObjReusePool;

#ifdef A_PLAT_PC
  template<class _ElementType, class _KeyType = _ElementType> class APArrayLogical;
  template<class _ElementType, class _KeyType = _ElementType> class APSortedLogical;
#else
  #include <AgogCore/APArray.hpp>
  #include <AgogCore/APSorted.hpp>
#endif

typedef APSortedLogical<SkData, ASymbol> tSkDataTable;
typedef APArrayLogical<SkData, ASymbol>  tSkDataIdxTable;


//---------------------------------------------------------------------------------------
// Used by SkObjectBase::get_obj_type() which returns uint32_t rather than eSkObjectType so
// that the types can be easily extended when custom types are derived.
enum eSkObjectType
  {
  // SkInstance, SkInstanceUnreffed(used by nil)
  SkObjectType_instance,

  // SkBoolean
  SkObjectType_boolean,

  // SkActor
  SkObjectType_actor,

  // SkMind
  SkObjectType_mind,

  // SkClosure
  SkObjectType_closure,

  // SkMetaClass
  SkObjectType_meta_class,

  // SkInvokedBase(SkInvokedExpression, SkInvokedContextBase(SkInvokedMethod,
  // SkInvokedCoroutine)) - Could split into invoked_expr, invoked_method, invoked_coroutine
  SkObjectType_invoked_obj,

  // Custom object types should be => SkObjectType__custom_start
  SkObjectType__custom_start
  };


//---------------------------------------------------------------------------------------
// SkookumScript Data member (data member, class data member, argument, temporary variable)
struct SkData : public ANamed
  {
  SK_NEW_OPERATORS(SkData);


  friend class AObjBlock<SkData>;

  // Public Data Members

    SkInstance * m_data_p;

  // Class Methods

    static SkData *                pool_new(const ASymbol & name);
    static SkData *                pool_new(const ASymbol & name, SkInstance * instance_p);
    static void                    pool_delete(SkData * data_p);
    static AObjReusePool<SkData> & get_pool();

    static void empty_table(tSkDataTable * table_p);
    static void empty_table(tSkDataIdxTable * table_p);

  protected:

  // Internal Methods

    SkData() {} // Fully initialized by pool_new

  };  // SkData


//---------------------------------------------------------------------------------------
// Root object abstract base class for all Skookum "instances".
//
// Notes:
//   AIdPtr<SkObjectBase> can be used as a smart pointer for this class and any of its
//   subclasses.
//
// Subclasses:
//   SkInstance(SkBoolean, SkClosure, SkDataInstance(SkActor),
//   SkInstanceUnreffed(SkMetaClass)), SkInvokedBase(SkInvokedExpression,
//   SkInvokedContextBase(SkInvokedMethod, SkInvokedCoroutine))
//
// #Author(s) Conan Reis
class SkObjectBase
  {
  public:

    // Public Data Members

      // Pointer id of the object that must match the id in any AIdPtr<SkObjectBase> for it
	  // to be valid.
      uint32_t m_ptr_id;

    // Methods
     
      virtual ~SkObjectBase() {}

      void renew_id()                                  { m_ptr_id = ++ms_ptr_id_prev; }
      bool is_valid_id() const                         { return m_ptr_id && (m_ptr_id <= ms_ptr_id_prev); }

      virtual uint32_t get_obj_type() const = 0;
      virtual bool     is_mind() const                 { return false; }

    // Data methods

      virtual SkInstance *           as_new_instance() const;
      virtual SkInstance *           find_data_by_name(const ASymbol & name) const = 0;
      virtual SkInstance *           get_data_by_name(const ASymbol & name) const = 0;
      virtual void                   set_data_by_name(const ASymbol & name, SkInstance * obj_p) = 0;
      virtual SkInvokedContextBase * get_scope_context() const;
      virtual SkInstance *           get_topmost_scope() const = 0;

    // Class Data

      static uint32_t ms_ptr_id_prev;

  };  // SkObjectBase


//=======================================================================================
// Inline Methods
//=======================================================================================

#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkObjectBase.inl>
#endif


#endif  // __SKOBJECTBASE_HPP

