//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2001 Agog Labs Inc.,
// All rights reserved.
//
// Data structures for class descriptors and class objects
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKCLASS_HPP
#define __SKCLASS_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkClassDescBase.hpp>
#include <SkookumScript/SkDataInstance.hpp>
#include <SkookumScript/SkMethod.hpp>
#include <SkookumScript/SkCoroutine.hpp>  // Needs: tSkCoroutineFunc and tSkCoroutineMthd


//=======================================================================================
// Global Structures
//=======================================================================================

// Pre-declarations
struct SkApplyExpressionBase; 
class  SkBrain;
class  SkClassDescBase;
class  SkInvokedMethod;
class  SkObjectId;
struct SkTypedData;


// Short-hand for arrays
// $Revisit - CReis These arrays will eventually become non-sorted virtual method tables
typedef APSorted<SkClassUnaryBase, SkClassUnaryBase, SkClassUnaryBase> tSkSortedTypes;
typedef APSortedLogical<SkClass, ASymbol>         tSkClasses;
typedef APSortedLogical<SkMethodBase, ASymbol>    tSkMethodTable;
typedef APSortedLogical<SkCoroutineBase, ASymbol> tSkCoroutines;
typedef APSortedLogical<SkTypedName, ASymbol>     tSkTypedNames;
typedef APSortedLogical<SkTypedData, ASymbol>     tSkTypedDatas;


//---------------------------------------------------------------------------------------
// Used when binding a member to a class:
//   SkClass::register_coroutine_func(), SkClass::register_coroutine_mthd(),
//   SkClass::register_method_func(), SkClass::register_method_mthd()
//   
// Enum flags are preferred over bool since it is more descriptive at the point of call.
enum eSkBindFlag
  {
  // Descriptive enumerations - use when passing as a full argument:
  // [Listed before flag enumerations to ensure that they are used as descriptors in the
  // debugger when this enum type has the associated value.]
  
    // Instance member with rebinding disallowed.
    SkBindFlag_instance_no_rebind   = 0x0,  // [Placed as first enum with 0 to ensure that it appears as description in debugger when value is 0.]

    // Class member with rebinding disallowed.
    SkBindFlag_class_no_rebind      = 1 << 0,

    // Instance member with rebinding allowed
    SkBindFlag_instance_rebind      = 1 << 1,

    // Bind as a class member rather than an instance member
    SkBindFlag_class_rebind         = (1 << 0) | (1 << 1),

    // Default binding - instance member and disallow previously bound member to be rebound
    SkBindFlag_default              = SkBindFlag_instance_no_rebind,


  // Flag enumerations - use when dealing with the individual flags:

    // No binding flags set
    SkBindFlag__none        = 0x0,

    // Bind as a class member rather than an instance member
    SkBindFlag__class       = 1 << 0,

    // Bind as an instance member rather than a class member.  [Just used as hint since flag not set.]
    SkBindFlag__instance    = 0x0,

    // Allow previously bound member to be rebound
    SkBindFlag__rebind      = 1 << 1,

    // Disallow previously bound member to be rebound - assert if attempt is made to bind
    // a member that has already been bound.  [Just used as hint since flag not set.]
    SkBindFlag__no_rebind   = 0x0,
  };

//---------------------------------------------------------------------------------------
// SkookumScript MetaClass - i.e. a class treated as an instance using class scope (only
// class methods/data can be used).  It is a wrapper around a `SkClass` object so that a
// class can be treated as an first-order instance object.
//
//   ```
//   meta-class = '<' class-name '>'
//   ```
// $Revisit - CReis Might be good idea to have it be derived from `SkMind` instead of
// `SkInstanceUnreffed`. This would allow instances to use their class as a default
// updater mind rather than the single "master mind".
// 
// Author(s)  Conan Reis
class SkMetaClass : public SkClassUnaryBase, public SkInstanceUnreffed
  {
  friend class SkClass;       // Accesses protected elements
  friend class SkClassUnion;  // Accesses protected elements

  public:

  // Unhide Inherited Methods

    // Methods in this class with the same name as methods in its superclasses are 'hidden'
    // (even if they do not have the same parameters), this makes them visible again.
    // These using directives must precede the new methods in this class.
    using SkInstanceUnreffed::method_call;


  // Common Methods

    SK_NEW_OPERATORS(SkMetaClass);

    explicit SkMetaClass(SkClass * class_info_p = nullptr);

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void as_binary_ref(void ** binary_pp) const;
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
      virtual AString get_scope_desc() const;
    #endif


  // Comparison Methods

    eAEquate compare(const SkMetaClass & mclass) const;

  // Methods

    SkClass & get_class_info() const  { return *m_class_info_p; }

    // Overriding from SkClassUnaryBase & SkClassDescBase

      virtual SkClassDescBase *  as_finalized_generic(const SkClassDescBase & scope_type) const;
      SkClassUnaryBase *         find_common_class(const SkClass & cls) const;
      virtual SkClassUnaryBase * find_common_type(const SkClassDescBase & cls) const;
      virtual eSkClassType       get_class_type() const;
      virtual SkClassDescBase *  get_data_type(const ASymbol & data_name, eSkScope * scope_p = nullptr) const;
      virtual SkClass *          get_key_class() const;
      virtual const ASymbol &    get_key_class_name() const;
      virtual SkMetaClass &      get_metaclass() const;
      virtual bool               is_generic() const;
      virtual bool               is_metaclass() const; 
      virtual bool               is_class_type(const SkClassDescBase * type_p) const;

      // Method Member Methods

        virtual void           append_method(SkMethodBase * method_p);
        virtual SkMethodBase * get_method(const ASymbol & method_name) const;
        virtual SkMethodBase * get_method_inherited(const ASymbol & method_name) const;
        virtual bool           is_method_inherited_valid(const ASymbol & method_name) const;
        virtual bool           is_method_valid(const ASymbol & method_name) const;

      // Coroutine Member Methods

        virtual void              append_coroutine(SkCoroutineBase * coroutine_p);
        virtual SkCoroutineBase * get_coroutine_inherited(const ASymbol & coroutine_name) const  { return nullptr; }
        virtual bool              is_coroutine_registered(const ASymbol & coroutine_name) const  { return false; }
        virtual bool              is_coroutine_valid(const ASymbol & coroutine_name) const       { return false; }

      // Data Member Methods

        virtual SkTypedName * append_data_member(const ASymbol & name, SkClassDescBase * type_p);

    // Overriding from SkInstance

      virtual void method_invoke(const SkMethodCall & mcall, SkObjectBase * scope_p, SkInvokedBase * caller_p, SkInstance ** result_pp);
      virtual void method_call(const ASymbol & method_name, SkInstance ** args_pp, uint32_t arg_count, SkInstance ** result_pp = nullptr, SkInvokedBase * caller_p = nullptr);

      #if defined(SK_AS_STRINGS)
        virtual AString as_string_debug() const;
      #endif


    // Overriding from SkObjectBase

      virtual SkInstance * get_data_by_name(const ASymbol & name) const;
      virtual void         set_data_by_name(const ASymbol & name, SkInstance * obj_p);
      virtual uint32_t     get_obj_type() const       { return SkObjectType_meta_class; } 


  protected:

  // Data Members

    // The class that this instance represents/is wrapped around
    // $Revisit - CReis [Optimization] Consider using m_class_p from SkInstance instead since it will
    // always point to "Class" and not add any useful info.
    SkClass * m_class_info_p;

    // $Revisit - CReis [Optimization] m_ref_count and m_user_data inherited from SkInstance are not used.
    // A SkInstanceBase class could be created that does not have these data members.

  };  // SkMetaClass

//---------------------------------------------------------------------------------------
// Specialization - also ensures that `SkInvokedBase::get_arg_data<SkMetaClass>(--)` etc. work properly
// 
// See: SkInstance::as<SkClass>
template<> inline SkMetaClass * SkInstance::as_data<SkMetaClass>() const { return static_cast<SkMetaClass *>(const_cast<SkInstance *>(this)); }


//---------------------------------------------------------------------------------------
// SkookumScript Class - i.e. an instance of a class using class instance scope (both
// instance methods/data and class methods/data can be used).
//
//   class = class-name
//
// Subclasses: SkActorClass
// Author(s):  Conan Reis
class SkClass : public SkClassUnaryBase, public ANamed
  {
  // Accesses protected elements
  friend class SkBrain;
  friend class SkClassUnion;
  friend class SkCompiler;
  friend class SkContextClassBase;
  friend class SkInvokableClass;
  friend class SkMetaClass;
  friend class SkParser;
  friend class SkTypedClass;

  public:

  // Nested Structures

    // AEx<SkClass> exception id values
    enum
      {
      ErrId_nonexistent_method_regd = AErrId_last,  // No such method has been registered
      ErrId_duplicate_coroutine,                    // Coroutine with same name already registered
      ErrId_nonexistent_coroutine,                  // No such coroutine exists
      };

    enum eSubclass
      {
      Subclass_recurse,  // Apply recursively to subclasses
      Subclass_defer     // Do not apply to subclasses - defer it at a later time as a bulk operation
      };

    // Class flags - stored in m_flags
    enum eFlag
      {
      Flag_none             = 0,
      Flag_loaded           = 1 << 0,  // Class has all members loaded
      Flag_demand_load      = 1 << 1,  // Class only loads members when asked to do so
      Flag_demand_load_lock = 1 << 2,  // Once loaded do not allow it to be unloaded
      Flag_demand_unload    = 1 << 3,  // Deferred unload - unload when next possible to do so

      // Object id flags - look-up/validate for this class - i.e. Class@'name'
        Flag_object_id_lookup = 1 << 4,

        // Validation flags - use masks below
          Flag_object_id_parse_any   = 1 << 5,
          Flag_object_id_parse_list  = 1 << 6,
          Flag_object_id_parse_defer = 1 << 7,

        // Object id validation setting (masks):
          // Accept none during compile [used to temporarily disable object ids]
          Flag__id_valid_none  = Flag_none,
          // Accept any during compile
          Flag__id_valid_any   = Flag_object_id_parse_any,
          // Validate using list during compile (include list as compile dependency)
          Flag__id_valid_parse = Flag_object_id_parse_list,
          // Validate using list during compile if it exists (parse) - otherwise accept any during compile and validate using list in separate pass/run (defer)
          Flag__id_valid_exist = Flag_object_id_parse_any | Flag_object_id_parse_list,
          // Accept any during compile and validate using list in separate pass/run
          Flag__id_valid_defer = Flag_object_id_parse_any | Flag_object_id_parse_list | Flag_object_id_parse_defer,

          Flag__id_valid_mask = Flag_object_id_parse_any | Flag_object_id_parse_list | Flag_object_id_parse_defer,

        Flag__id_mask = Flag_object_id_lookup | Flag__id_valid_mask,

      // Defaults and masks
        Flag__default         = Flag_none,
        Flag__default_actor   = Flag__default | Flag_object_id_lookup | Flag_object_id_parse_any,
        Flag__demand_loaded   = Flag_loaded | Flag_demand_load,
        Flag__mask_binary     = Flag_demand_load | Flag__id_mask
      };

  // Public Types

    struct MethodInitializerFunc
      {
      const char *    m_method_name_p;
      tSkMethodFunc   m_atomic_f;
      };

    struct MethodInitializerFuncId
      {
      uint32_t        m_method_name_id;
      tSkMethodFunc   m_atomic_f;
      };
    
    struct MethodInitializerMthd
      {
      const char *    m_method_name_p;
      tSkMethodMthd   m_atomic_m;
      };

    struct MethodInitializerMthdId
      {
      uint32_t        m_method_name_id;
      tSkMethodMthd   m_atomic_m;
      };
    
    struct CoroutineInitializerFunc
      {
      const char *      m_coroutine_name_p;
      tSkCoroutineFunc  m_atomic_f;
      };

    struct CoroutineInitializerFuncId
      {
      uint32_t          m_coroutine_name_id;
      tSkCoroutineFunc  m_atomic_f;
      };

    struct CoroutineInitializerMthd
      {
      const char *      m_coroutine_name_p;
      tSkCoroutineMthd  m_atomic_m;
      };

    struct CoroutineInitializerMthdId
      {
      uint32_t          m_coroutine_name_id;
      tSkCoroutineMthd  m_atomic_m;
      };

    // Public Class Data

    #if (SKOOKUM & SK_DEBUG)
      // Reparse Member Info - placed here rather than in SkCompiler to ensure that it is
      // available for as many build configurations as possible.

      static bool ms_use_reparse_info;

      static APSorted<SkMethodBase, SkQualifier, SkQualifierCompareName>    ms_reparse_methods;
      static APSorted<SkMethodBase, SkQualifier, SkQualifierCompareName>    ms_reparse_class_methods;
      static APSorted<SkCoroutineBase, SkQualifier, SkQualifierCompareName> ms_reparse_coroutines;
      //static APSorted<SkTypedData, SkQualifier, SkQualifierCompareName>     ms_reparse_class_data;
    #endif

  // Common Methods

    SK_NEW_OPERATORS(SkClass);

    explicit SkClass(const ASymbol & name, SkClass * superclass_p = nullptr, uint32_t flags = Flag__default);
    ~SkClass();

  // Converter Methods

    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif


  // Comparison Methods

    eAEquate compare(const SkClass & ssclass) const;      // Hierarchy sort
    eAEquate compare_ids(const SkClass & ssclass) const;  // Standard sort

  // Serialization Methods

    void         enable_demand_load(bool demand_load = true)            { if (demand_load) { m_flags |= Flag_demand_load; } else { m_flags &= ~Flag_demand_load; } }
    virtual bool demand_unload();
    SkClass *    get_demand_loaded_root() const;
    bool         is_demand_loaded() const                               { return (m_flags & Flag_demand_load) || (m_superclass_p && m_superclass_p->is_demand_loaded()); }
    bool         is_demand_loaded_root() const                          { return (m_flags & Flag_demand_load) != 0; }
    bool         is_loaded() const                                      { return (m_flags & Flag_loaded) != 0u; }
    bool         is_load_locked() const                                 { return (m_flags & Flag_demand_load_lock) != 0u; }
    bool         is_unload_deferred() const                             { return (m_flags & Flag_demand_unload) != 0u; }
    void         lock_load(bool lock = true)                            { if (lock) { m_flags |= Flag_demand_load_lock; } else { m_flags &= ~Flag_demand_load_lock; } }
    void         set_flag_load(bool loaded = true);
    void         set_loaded()                                           { set_flag_load(); }

    void                 enable_object_id_lookup(bool enable = true);
    bool                 is_object_id_lookup() const                    { return (m_flags & Flag_object_id_lookup) != 0u; }
    void                 set_object_id_lookup_func(SkInstance * (* object_id_lookup_f)(SkObjectId * obj_id_p));
    virtual SkInstance * object_id_lookup(SkObjectId * obj_id_p) const;

    #if (SKOOKUM & SK_CODE_IN)
      void              clear_object_id_valid_list();
      uint32_t          get_object_id_validate() const                  { return m_flags & Flag__id_valid_mask; }
      ASymbolTable *    get_object_id_valid_list() const                { return m_object_ids_p; }
      ASymbolTable *    get_object_id_valid_list_merge();
      void              set_object_id_validate(uint32_t validate_mask = Flag__id_valid_any);
      virtual SkClass * object_id_validate(SkObjectId * obj_id_p, bool validate_deferred = true) const;

      #if (SKOOKUM & SK_DEBUG)
        uint32_t        object_id_validate_recurse(bool validate_deferred, uint32_t * obj_id_count_p = nullptr);
      #endif
    #endif

    #if (SKOOKUM & SK_COMPILED_OUT)
      virtual void     as_binary(void ** binary_pp) const;
      virtual uint32_t as_binary_length() const;
      void             as_binary_recurse(void ** binary_pp, bool skip_demand_loaded) const;
      uint32_t         as_binary_recurse_length(bool skip_demand_loaded) const;
      void             as_binary_group(void ** binary_pp, bool skip_demand_loaded) const;
      uint32_t         as_binary_group_length(bool skip_demand_loaded) const;
      void             as_binary_placeholder_recurse(void ** binary_pp) const;
      static uint32_t  as_binary_placeholder_recurse_length_all();
      virtual void     as_binary_ref(void ** binary_pp) const;
    #endif


    #if (SKOOKUM & SK_COMPILED_IN)
      virtual void     assign_binary(const void ** binary_pp, bool append_super_members = true);
      static SkClass * from_binary_ref(const void ** binary_pp);
      static void      from_binary_group(const void ** binary_pp);
    #endif

    #if (SKOOKUM & SK_DEBUG)
      void         ensure_loaded_debug();
      virtual void reparse_prep();

      uint32_t        count_expressions_recurse(eAHierarchy hierarchy = AHierarchy__all);
      eAIterateResult iterate_expressions(SkApplyExpressionBase * apply_expr_p);
      eAIterateResult iterate_expressions_recurse(SkApplyExpressionBase * apply_expr_p, eAHierarchy hierarchy = AHierarchy__all);
    #endif

  // Methods

    virtual SkMetaClass & get_metaclass() const;

    uint32_t     get_flags() const                                      { return m_flags; }

    virtual void clear_members();
    virtual void clear_members_compact();

    virtual void track_memory(AMemoryStats * mem_stats_p, bool skip_demand_loaded) const;
    virtual void track_memory(AMemoryStats * mem_stats_p) const         { track_memory(mem_stats_p, true); }
    void         track_memory_recursive(AMemoryStats * mem_stats_p, bool skip_demand_loaded) const;


    // Hierarchy Methods

      void                       append_subclass(SkClass * subclass_p);
      SkClass *                  find_common_class(const SkClass & cls) const;
      virtual SkClassUnaryBase * find_common_type(const SkClassDescBase & cls) const;
      uint32_t                   get_class_recurse_count(bool skip_demand_loaded) const;
      tSkClasses &               get_subclasses()                       { return m_subclasses; }
      const tSkClasses &         get_subclasses() const                 { return m_subclasses; }
      void                       get_subclasses_all(tSkClasses * classes_p) const;
      SkClass *                  get_superclass() const                 { return m_superclass_p; }
      void                       get_superclasses_all(tSkClasses * classes_p) const;
      uint32_t                   get_superclass_depth() const;
      SkClass *                  get_class_depth_at(uint32_t depth) const;
      bool                       is_class(const SkClass & cls) const;
      bool                       is_subclass(const SkClass & superclass) const;
      bool                       is_superclass(const SkClass & subclass) const;
      bool                       is_scope_qualifier(SkClassDescBase * recv_type_p) const;
      eAIterateResult            iterate_recurse(AFunctionArgRtnBase<SkClass *, eAIterateResult> * apply_class_p, eAHierarchy hierarchy = AHierarchy__all);
      void                       iterate_recurse(AFunctionArgBase<SkClass *> * apply_class_p, eAHierarchy hierarchy = AHierarchy__all);
      SkClass *                  next_class(SkClass * root_p) const;
      SkClass *                  next_sibling() const;


    // Method Methods

      // Instance & Class Methods

        virtual void           append_method(SkMethodBase * method_p);
        virtual SkMethodBase * get_method(const ASymbol & method_name) const;
        virtual SkMethodBase * get_method_inherited(const ASymbol & method_name) const;
        SkMethodBase *         get_method_inherited_receiver(const ASymbol & method_name, SkInstance ** receiver_pp, SkInvokedBase * caller_p) const;
        virtual bool           is_method_valid(const ASymbol & method_name) const;
        virtual bool           is_method_inherited_valid(const ASymbol & method_name) const;
        void                   register_method_func(const ASymbol & method_name, tSkMethodFunc atomic_f, eSkBindFlag flags = SkBindFlag_default);
        void                   register_method_func(const char * method_name_p, tSkMethodFunc atomic_f, eSkBindFlag flags = SkBindFlag_default);
        void                   register_method_mthd(const ASymbol & method_name, tSkMethodMthd atomic_m, eSkBindFlag flags = SkBindFlag_default);
        void                   register_method_mthd(const char * method_name_p, tSkMethodMthd atomic_m, eSkBindFlag flags = SkBindFlag_default);
        void                   register_method_func_bulk(const MethodInitializerFunc   * bindings_p, uint32_t count, eSkBindFlag flags);
        void                   register_method_func_bulk(const MethodInitializerFuncId * bindings_p, uint32_t count, eSkBindFlag flags);
        void                   register_method_mthd_bulk(const MethodInitializerMthd   * bindings_p, uint32_t count, eSkBindFlag flags);
        void                   register_method_mthd_bulk(const MethodInitializerMthdId * bindings_p, uint32_t count, eSkBindFlag flags);

      // Instance Methods

        void                   append_instance_method(SkMethodBase * method_p);
        SkMethodBase *         get_instance_destructor_inherited() const                     { return m_destructor_p; }
        SkMethodBase *         get_instance_method(const ASymbol & method_name) const        { return m_methods.get(method_name); }
        SkMethodBase *         get_instance_method_inherited(const ASymbol & method_name) const;
        SkMethodBase *         get_instance_method_overridden(const ASymbol & method_name) const;
        SkMethodBase *         get_instance_method_scoped_inherited(const SkQualifier & method_qual) const;
        const tSkMethodTable & get_instance_methods() const                                  { return m_methods; }
        bool                   is_instance_method_valid(const ASymbol & method_name) const   { return (m_methods.get(method_name) != nullptr); }
        virtual SkInstance *   new_instance();

      // Class Methods

        void                   append_class_method(SkMethodBase * method_p);
        SkMethodBase *         get_class_method(const ASymbol & method_name) const                  { return m_class_methods.get(method_name); }
        SkMethodBase *         get_class_method_inherited(const ASymbol & method_name) const;
        SkMethodBase *         get_class_method_overridden(const ASymbol & method_name) const;
        const tSkMethodTable & get_class_methods() const                                            { return m_class_methods; }
        void                   invoke_class_ctor();
        void                   invoke_class_ctor_recurse();
        void                   invoke_class_dtor_recurse();
        bool                   is_class_method_valid(const ASymbol & method_name) const             { return (m_class_methods.get(method_name) != nullptr); }
        bool                   is_class_method_inherited_valid(const ASymbol & method_name) const   { return (get_class_method_inherited(method_name) != nullptr); }

    // Coroutine Methods

      virtual void         append_coroutine(SkCoroutineBase * coroutine_p);
      SkCoroutineBase *    get_coroutine(const ASymbol & coroutine_name) const       { return m_coroutines.get(coroutine_name); }
      SkCoroutineBase *    get_coroutine_inherited(const ASymbol & coroutine_name) const;
      SkCoroutineBase *    get_coroutine_overridden(const ASymbol & coroutine_name) const;
      SkCoroutineBase *    get_coroutine_scoped_inherited(const SkQualifier & coroutine_qual) const;
      virtual bool         is_coroutine_registered(const ASymbol & coroutine_name) const;
      virtual bool         is_coroutine_valid(const ASymbol & coroutine_name) const  { return m_coroutines.find(coroutine_name); }
      void                 register_coroutine_func(const ASymbol & coroutine_name, tSkCoroutineFunc update_f, eSkBindFlag flags = SkBindFlag_default);
      void                 register_coroutine_func(const char * coroutine_name_p, tSkCoroutineFunc update_f, eSkBindFlag flags = SkBindFlag_default);
      void                 register_coroutine_mthd(const ASymbol & coroutine_name, tSkCoroutineMthd update_m, eSkBindFlag flags = SkBindFlag_default);
      void                 register_coroutine_mthd(const char * coroutine_name_p, tSkCoroutineMthd update_m, eSkBindFlag flags = SkBindFlag_default);
      void                 register_coroutine_func_bulk(const CoroutineInitializerFunc   * bindings_p, uint32_t count, eSkBindFlag flags);
      void                 register_coroutine_func_bulk(const CoroutineInitializerFuncId * bindings_p, uint32_t count, eSkBindFlag flags);
      void                 register_coroutine_mthd_bulk(const CoroutineInitializerMthd   * bindings_p, uint32_t count, eSkBindFlag flags);
      void                 register_coroutine_mthd_bulk(const CoroutineInitializerMthdId * bindings_p, uint32_t count, eSkBindFlag flags);
      bool                 remove_coroutine(const ASymbol & coroutine_name)          { return m_coroutines.free(coroutine_name); }
      const tSkCoroutines & get_coroutines() const                                   { return m_coroutines; }

    // Data Methods

      virtual SkTypedName *     append_data_member(const ASymbol & name, SkClassDescBase * type_p);
      virtual SkClassDescBase * get_data_type(const ASymbol & data_name, eSkScope * scope_p = nullptr) const;

      // Instance Methods

        SkTypedName *         append_instance_data(const ASymbol & name, SkClassDescBase * type_p, eSubclass subclasses = Subclass_recurse);
        const tSkTypedNames & get_instance_data() const        { return m_data; }
        const tSkTypedNames & get_instance_data_table() const  { return m_data_table; }
        void                  remove_instance_data_all();

        #if (SKOOKUM & SK_COMPILED_IN)
          SkClass * find_instance_data_scope(const ASymbol & name, ePath check_path = Path_super_sub);
        #endif

      // Class Methods

        SkTypedName *         append_class_data(const ASymbol & name, SkClassDescBase * type_p, eSubclass subclasses = Subclass_recurse);
        void                  fill_class_data_names(APSortedLogical<ASymbol> * target_p);
        const tSkTypedDatas & get_class_data() const        { return m_class_data; }
        const tSkTypedDatas & get_class_data_table() const  { return m_class_data_table; }
        void                  remove_class_data_all();

        #if (SKOOKUM & SK_COMPILED_IN)
          SkClass * find_class_data_scope(const ASymbol & name, ePath check_path = Path_super_sub);
        #endif

    // Overriding from SkClassDescBase

      virtual SkClassDescBase * as_finalized_generic(const SkClassDescBase & scope_type) const;
      virtual eSkClassType      get_class_type() const;
      virtual SkClassDescBase * get_item_type() const;
      virtual const ASymbol &   get_key_class_name() const;
      virtual bool              is_class_type(const SkClassDescBase * type_p) const;
      virtual bool              is_generic() const;
      virtual bool              is_method_registered(const ASymbol & method_name) const;

    // Overriding from SkClassUnaryBase

      virtual SkClass * get_key_class() const;

  protected:

  // Internal Methods

    void         recurse_instance_data(const SkTypedName * typed_p);
    void         recurse_remove_instance_data_all(const tSkTypedNames & data);
    void         recurse_remove_class_data_all(const tSkTypedDatas & data);
    void         recurse_class_data(const SkTypedData * data_p);
    virtual void recurse_class_ctor(SkInvokedMethod * imethod_p);
    void         recurse_class_dtor();
    void         demand_unload_recurse();
    void         set_destructor(SkMethodBase * destructor_p);

  // Internal Class Methods

    // SkookumScript Atomic Methods

      #if (SKOOKUM & SK_DEBUG)
        virtual void find_unregistered_atomics(APArray<SkInvokableBase> * atomics_p);
      #endif


  // Data Members

    // Class Flags - see eFlags
    uint32_t m_flags;

    // Parent of this class [Single inheritance only - at least in first pass]
    SkClass * m_superclass_p;

    // Child classes of this class
    tSkClasses m_subclasses;

    // Used to lookup named instance objects via object id expressions.  See object_id_lookup()
    SkInstance * (* m_object_id_lookup_f)(SkObjectId * obj_id_p);

    // Used to validate object ids for this class.
    // $Revisit - CReis Since there are few classes that use object ids it might be an idea
    // to store only the symbol tables for classes that actually use them rather than
    // taking up 4/8 bytes for every class whether they use object ids or not.
    ASymbolTable * m_object_ids_p;

    // $Revisit - CReis [Index Look-up] Rewrite the data and method tables so that a direct
    // index can be used rather than a binary search of the symbols.

    // Instance Member Info

      // Data Members (name and class type) for instances of this class - added via this
      // class directly.  Each data member name must be unique - no superclass nor subclass
      // may have it and there should be no class data member with the same name either
      // - this is enforced by the compiler.
      // m_data_table references this and inherited members for constructing instances of
      // this class.
      tSkTypedNames m_data;

      // Data Members (name and class type) for instances of this class including data
      // members inherited from all its superclasses for quick construction / instantiation.
      tSkTypedNames m_data_table;

      // Methods of this class - added via this class directly.
      tSkMethodTable m_methods;

      // Shortcut to instance destructor
      SkMethodBase * m_destructor_p;

      // List of available coroutines
      tSkCoroutines m_coroutines;

      // Coroutines of this class and all its superclasses for quick look-ups
      //tSkCoroutines m_coroutine_table;

    // Class Member Info

      // Class Data Members (name, class type and data) for this class - added via this
      // class directly.  Each class data member name must be unique - no superclass nor
      // subclass/ may have it and there should be no instance data member with the same
      // name either - this is enforced by the compiler.
      // m_class_data_table is used for class data look-ups.
      tSkTypedDatas m_class_data;

      // Class Data Members (name and class type) for this class including data members
      // inherited from all its superclasses for quick look-ups.
      // $Note - CReis [Optimization] Currently this does not include indirect data members from "Class".
      tSkTypedDatas m_class_data_table;

      // Class Methods of this class - added via this class directly.
      tSkMethodTable m_class_methods;

      // Class Methods of this class including data members inherited from all its
      // superclasses for quick look-ups.
      //tSkMethodTable m_class_method_table;

    // Wrappers - for quick access and no need to allocate memory

      // Metaclass wrapper for this class
      SkMetaClass m_metaclass;

  };  // SkClass


//---------------------------------------------------------------------------------------
// Notes      SkookumScript Union between 2 or more classes - i.e. instance classes and
//            meta classes (SkClassUnaryBase -> SkClass & SkMetaClass).  Used as a type
//            descriptor to denote an instance that could be one of several different
//            types.  The class type "None" (nil) in particular is often paired with
//            other types.
//
//              class-union    = '<' class-unary {'|' class-unary}1+ '>'
//              class-unary    = class-instance | meta-class
//              class-instance = class | list-class
//              class          = class-name
//              list-class     = List ['{' ws [class-desc ws] '}']
//              meta-class     = '<' class-name '>'
//
// ToDo
//   - merge superclasses with subclasses (and same classes)
//   - disallow "unions" that merge down to one class - use MetaClass instead
//   - sort list of unioned classes alphabetically
//   - have global sorted list of already created unions
//   - derive from ARefCount and have all references to a SkDescBase increment and decrement
//     - potentially make a SkClassDescRef that does this automatically
//
// Author(s)  Conan Reis
class SkClassUnion : public SkClassDescBase, public ARefCountMix<SkClassUnion>
  {
  // Accesses protected elements
  friend class SkBrain;       
  friend class SkClass;
  friend class SkTypedClass;
  friend class SkInvokableClass;
  friend class SkMetaClass;

  public:

  // Common Methods

    SK_NEW_OPERATORS(SkClassUnion);

    SkClassUnion()                                                : m_common_class_p(nullptr) {}
    explicit SkClassUnion(const SkClassUnaryBase & initial_class) : m_common_class_p(const_cast<SkClassUnaryBase *>(&initial_class)) { initial_class.reference(); initial_class.reference(); m_union.append(initial_class); }
    explicit SkClassUnion(const SkClassDescBase & initial_class)  : m_common_class_p(nullptr) { merge_class(initial_class); }
    SkClassUnion(const SkClassUnion & class_union);
    virtual ~SkClassUnion();
    
    SkClassUnion & operator=(const SkClassUnion & class_union);

  // Converter Methods

    #if (SKOOKUM & SK_COMPILED_OUT)
      void         as_binary(void ** binary_pp) const;
      uint32_t     as_binary_length() const;
      virtual void as_binary_ref(void ** binary_pp) const;
      virtual uint32_t as_binary_ref_typed_length() const;
    #endif


    #if (SKOOKUM & SK_COMPILED_IN)
      SkClassUnion(const void ** binary_pp)                       : m_common_class_p(nullptr) { assign_binary(binary_pp); }
      void                  assign_binary(const void ** binary_pp);
      static SkClassUnion * from_binary_ref(const void ** binary_pp);
    #endif


    #if defined(SK_AS_STRINGS)
      virtual AString as_code() const;
    #endif

  // Comparison Methods

    eAEquate compare(const SkClassUnion & class_union) const;
    bool     operator==(const SkClassUnion & class_union) const  { return compare(class_union) == AEquate_equal; }
    bool     operator<(const SkClassUnion & class_union) const   { return compare(class_union) == AEquate_less; }
    bool     operator>(const SkClassUnion & class_union) const   { return compare(class_union) == AEquate_greater; }
    bool     operator!=(const SkClassUnion & class_union) const  { return compare(class_union) != AEquate_equal; }
    bool     operator<=(const SkClassUnion & class_union) const  { return compare(class_union) != AEquate_greater; }
    bool     operator>=(const SkClassUnion & class_union) const  { return compare(class_union) != AEquate_less; }

  // Methods

    // Type-checking Methods

      virtual const SkClassUnaryBase * as_unary_class() const         { return m_common_class_p; }
      virtual SkClassUnaryBase *       as_unary_class()               { return m_common_class_p; }
      SkClassUnaryBase *               get_common_class() const       { return m_common_class_p; }
      void                             set_common_class(SkClassUnaryBase * class_p);
      virtual bool                     is_builtin_actor_class() const;
      bool                             is_class_maybe(const SkClassDescBase * type_p) const;
      bool                             is_valid_param_for(const SkClassDescBase * arg_type_p) const;
      virtual bool                     is_metaclass() const; 
      bool                             is_trivial() const             { return (m_union.get_length() <= 1u); }
      void                             merge_class(const SkClassUnaryBase & new_class);
      void                             merge_class(const SkClassDescBase & new_class);

    // Overriding from SkClassUnaryBase, SkClassDescBase, ARefCountMix<>

      virtual void reference() const;
      virtual void dereference();
      virtual void dereference_delay() const;
      void         on_no_references();

      virtual SkClassDescBase *  as_finalized_generic(const SkClassDescBase & scope_type) const;
      virtual SkClassUnaryBase * find_common_type(const SkClassDescBase & cls) const;
      virtual eSkClassType       get_class_type() const;
      virtual SkClassDescBase *  get_data_type(const ASymbol & data_name, eSkScope * scope_p = nullptr) const;
      virtual SkClassDescBase *  get_item_type() const;
      virtual SkClass *          get_key_class() const;
      virtual const ASymbol &    get_key_class_name() const;
      virtual SkMetaClass &      get_metaclass() const;
      virtual bool               is_class_type(const SkClassDescBase * type_p) const;
      virtual bool               is_generic() const;

      // Method Member Methods

        virtual SkMethodBase * get_method(const ASymbol & method_name) const;
        virtual SkMethodBase * get_method_inherited(const ASymbol & method_name) const;
        virtual bool           is_method_inherited_valid(const ASymbol & method_name) const;

      // Coroutine Member Methods

        virtual SkCoroutineBase * get_coroutine_inherited(const ASymbol & coroutine_name) const;

  // Class Methods

    static SkClassUnion *    get_or_create(const SkClassUnion & class_union);
    static SkClassDescBase * get_merge(const SkClassDescBase & class1, const SkClassDescBase & class2);
    static SkClassDescBase * get_merge(const APArrayBase<SkClassDescBase> & classes, bool object_on_empty_b = true);
    static SkClassDescBase * get_reduced(const SkClassUnion & class_union, const SkClassUnaryBase & class_to_remove);
    static void              shared_empty()               { ms_shared_unions.free_all(); }  
    static void              shared_ensure_references();
    static void              shared_track_memory(AMemoryStats * mem_stats_p);

  protected:

  // Internal Methods

    void clear();

  // Data Members

    // [Cached] Most specific superclass common to all classes in this union.
    SkClassUnaryBase * m_common_class_p;

    // Sorted list of classes in this union - there will always be more than one and they
    // will never be direct descendants of one another - i.e. none of the classes are
    // super-classes or sub-classes to any of the other classes in the union.
    tSkSortedTypes m_union;

  // Class Data Members

    // Class union objects that are shared amongst various data-structures
    static APSortedLogicalFree<SkClassUnion> ms_shared_unions;

  };  // SkClassUnion


//=======================================================================================
// Inline Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Storage specialization - SkMetaClass stored indirectly as pointer in SkUserData rather than whole structure
template<> inline SkMetaClass * SkUserDataBase::as<SkMetaClass>() const                { return *as_stored<SkMetaClass*>(); }
template<> inline void          SkUserDataBase::set(SkMetaClass const * const & value) { *as_stored<const SkMetaClass*>() = value; }
template<> inline void          SkUserDataBase::set(SkMetaClass       * const & value) { *as_stored<SkMetaClass*>() = value; }


#ifndef A_INL_IN_CPP
  #include <SkookumScript/SkClass.inl>
#endif


#endif  // __SKCLASS_HPP

