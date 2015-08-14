//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// Class for managing functions exposed to Blueprint graphs 
//
// Author: Markus Breyer
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================


//---------------------------------------------------------------------------------------
// Class for managing functions exposed to Blueprint graphs
class SkUEBlueprintInterface
  {
  public:
              SkUEBlueprintInterface();
              ~SkUEBlueprintInterface();

    static SkUEBlueprintInterface * get() { return ms_singleton_p; }

    void      clear();
    UClass *  reinitialize_class(SkClass * sk_class_p);
    void      reinitialize_class_recursively(SkClass * sk_class_p);
    void      reinitialize_all(); // Gather methods from SkookumScript

    bool      is_skookum_blueprint_function(UFunction * function_p) const;
    bool      is_skookum_blueprint_event(UFunction * function_p) const;

  protected:

    typedef SkInstance *  (*tK2ParamFetcher)(FFrame & stack);
    typedef uint32_t      (*tSkValueGetter)(void * const result_p, SkInstance * value_p);

    // To keep track of a parameter's name and type
    struct TypedName
      {
      ASymbol             m_name;
      SkClassDescBase *   m_type_p;

      TypedName(const ASymbol & name, SkClassDescBase * type_p) : m_name(name), m_type_p(type_p) {}
      };

    enum eBindingType
      {
      BindingType_Function,  // Call from Blueprints into Sk
      BindingType_Event,     // Call from Sk into Blueprints
      };

    // Keep track of a binding between Blueprints and Sk
    struct BindingEntry
      {
      ASymbol                   m_method_name;      // Copy of m_sk_method_p->get_name() in case m_sk_method_p goes bad
      SkClass *                 m_sk_class_p;       // Copy of m_sk_method_p->get_scope() in case m_sk_method_p goes bad
      SkMethodBase *            m_sk_method_p;
      TWeakObjectPtr<UClass>    m_ue_class_p;       // Copy of m_ue_method_p->GetOwnerClass() to detect if a deleted UFunction leaves dangling pointers
      TWeakObjectPtr<UFunction> m_ue_function_p;
      uint16_t                  m_num_params;
      bool                      m_is_class_member;  // Copy of m_sk_method_p->is_class_method() in case m_sk_method_p goes bad
      bool                      m_marked_for_delete;
      eBindingType              m_type;

      BindingEntry(SkMethodBase * sk_method_p, UFunction * ue_method_p, uint32_t num_params, eBindingType type)
        : m_method_name(sk_method_p->get_name())
        , m_sk_class_p(sk_method_p->get_scope())
        , m_sk_method_p(sk_method_p)
        , m_ue_class_p(ue_method_p->GetOwnerClass())
        , m_ue_function_p(ue_method_p)
        , m_num_params(num_params)
        , m_is_class_member(sk_method_p->is_class_member())
        , m_marked_for_delete(false)
        , m_type(type)
        {}
      };

    // Parameter being passed into Sk from Blueprints
    struct SkParamEntry : TypedName
      {
      tK2ParamFetcher m_fetcher_p;

      SkParamEntry(const ASymbol & name, SkClassDescBase * type_p, tK2ParamFetcher fetcher_p) : TypedName(name, type_p), m_fetcher_p(fetcher_p) {}
      };

    // Function binding (call from Blueprints into Sk)
    struct FunctionEntry : public BindingEntry
      {
      SkClassDescBase * m_result_type_p;
      tSkValueGetter    m_result_getter;

      FunctionEntry(SkMethodBase * sk_method_p, UFunction * ue_function_p, uint32_t num_params, SkClassDescBase * result_type_p, tSkValueGetter result_getter)
        : BindingEntry(sk_method_p, ue_function_p, num_params, BindingType_Function)
        , m_result_type_p(result_type_p)
        , m_result_getter(result_getter)
        {}

      // The parameter entries are stored behind this structure in memory
      SkParamEntry *       get_param_entry_array()       { return (SkParamEntry *)(this + 1); }
      const SkParamEntry * get_param_entry_array() const { return (const SkParamEntry *)(this + 1); }
      };

    // Parameter being passed into Blueprints into Sk
    struct K2ParamEntry : TypedName
      {
      tSkValueGetter  m_getter_p;
      uint32_t        m_offset;

      K2ParamEntry(const ASymbol & name, SkClassDescBase * type_p, tSkValueGetter getter_p, uint32_t offset) : TypedName(name, type_p), m_getter_p(getter_p), m_offset(offset) {}
      };

    // Event binding (call from Sk into Blueprints)
    struct EventEntry : public BindingEntry
      {
      mutable TWeakObjectPtr<UFunction> m_ue_function_to_invoke_p; // The copy of our method we actually can invoke

      EventEntry(SkMethodBase * sk_method_p, UFunction * ue_function_p, uint32_t num_params)
        : BindingEntry(sk_method_p, ue_function_p, num_params, BindingType_Event)
        {}

      // The parameter entries are stored behind this structure in memory
      K2ParamEntry *       get_param_entry_array()       { return (K2ParamEntry *)(this + 1); }
      const K2ParamEntry * get_param_entry_array() const { return (const K2ParamEntry *)(this + 1); }
      };

    struct ParamInfo
      {
      UProperty *       m_ue_param_p;
      tK2ParamFetcher   m_k2_param_fetcher_p;
      tSkValueGetter    m_sk_value_getter_p;
      };

    void                exec_method(FFrame & stack, void * const result_p, SkInstance * this_p);
    void                exec_class_method(FFrame & stack, void * const result_p);
    void                exec_instance_method(FFrame & stack, void * const result_p);

    static void         mthd_trigger_event(SkInvokedMethod * scope_p, SkInstance ** result_pp);

    void                reinitialize_class(SkClass * sk_class_p, UClass * ue_class_p);
    bool                try_update_binding_entry(UClass * ue_class_p, SkMethodBase * sk_method_p, int32_t * out_binding_index_p);
    void                prepare_binding_entry(SkMethodBase * sk_method_p);
    int32_t             try_add_binding_entry(UClass * ue_class_p, SkMethodBase * sk_method_p);
    int32_t             add_function_entry(UClass * ue_class_p, SkMethodBase * sk_method_p);
    int32_t             add_event_entry(UClass * ue_class_p, SkMethodBase * sk_method_p);
    int32_t             store_binding_entry(BindingEntry * binding_entry_p, int32_t binding_index_to_use);
    void                delete_binding_entry(uint32_t binding_index);
    UFunction *         build_ue_function(UClass * ue_class_p, SkMethodBase * sk_method_p, eBindingType binding_type, ParamInfo * out_param_info_array_p);
    UProperty *         build_ue_param(UFunction * ue_function_p, SkClassDescBase * sk_parameter_class_p, const FName & param_name, ParamInfo * out_param_info_p);
    void                bind_event_method(SkMethodBase * sk_method_p);
    
    template<class _TypedName>
    static bool         have_identical_signatures(const tSkParamList & param_list, const _TypedName * param_array_p);

    static SkInstance * fetch_k2_param_boolean(FFrame & stack);
    static SkInstance * fetch_k2_param_integer(FFrame & stack);
    static SkInstance * fetch_k2_param_real(FFrame & stack);
    static SkInstance * fetch_k2_param_string(FFrame & stack);
    static SkInstance * fetch_k2_param_vector3(FFrame & stack);
    static SkInstance * fetch_k2_param_rotation_angles(FFrame & stack);
    static SkInstance * fetch_k2_param_transform(FFrame & stack);
    static SkInstance * fetch_k2_param_entity(FFrame & stack);

    static uint32_t     get_sk_value_boolean(void * const result_p, SkInstance * value_p);
    static uint32_t     get_sk_value_integer(void * const result_p, SkInstance * value_p);
    static uint32_t     get_sk_value_real(void * const result_p, SkInstance * value_p);
    static uint32_t     get_sk_value_string(void * const result_p, SkInstance * value_p);
    static uint32_t     get_sk_value_vector3(void * const result_p, SkInstance * value_p);
    static uint32_t     get_sk_value_rotation_angles(void * const result_p, SkInstance * value_p);
    static uint32_t     get_sk_value_transform(void * const result_p, SkInstance * value_p);
    static uint32_t     get_sk_value_entity(void * const result_p, SkInstance * value_p);

    APArray<BindingEntry> m_binding_entry_array;

    UScriptStruct *       m_struct_vector3_p;
    UScriptStruct *       m_struct_rotation_angles_p;
    UScriptStruct *       m_struct_transform_p;

    static SkUEBlueprintInterface * ms_singleton_p; // Hack, make it easy to access for callbacks
        
  };