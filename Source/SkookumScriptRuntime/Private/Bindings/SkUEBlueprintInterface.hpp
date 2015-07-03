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

    enum eMethodType
      {
      MethodType_Function,
      MethodType_Event,
      };

    struct MethodEntry
      {
      SkMethodBase *            m_sk_method_p;
      TWeakObjectPtr<UFunction> m_ue_method_p;
      uint32_t                  m_num_params;
      eMethodType               m_type;

      MethodEntry(SkMethodBase * sk_method_p, UFunction * ue_method_p, uint32_t num_params, eMethodType type)
        : m_sk_method_p(sk_method_p)
        , m_ue_method_p(ue_method_p)
        , m_num_params(num_params)
        , m_type(type)
        {}
      };

    struct SkParamEntry
      {
      ASymbol         m_name;
      tK2ParamFetcher m_fetcher_p;
      };

    struct FunctionEntry : public MethodEntry
      {
      tSkValueGetter  m_result_getter;

      FunctionEntry(SkMethodBase * sk_method_p, UFunction * ue_method_p, uint32_t num_params, tSkValueGetter result_getter)
        : MethodEntry(sk_method_p, ue_method_p, num_params, MethodType_Function)
        , m_result_getter(result_getter)
        {}

      // The parameter entries are stored behind this structure in memory
      SkParamEntry *       get_param_entry_array()       { return (SkParamEntry *)(this + 1); }
      const SkParamEntry * get_param_entry_array() const { return (const SkParamEntry *)(this + 1); }
      };

    struct K2ParamEntry
      {
      tSkValueGetter  m_getter_p;
      uint32_t        m_offset;
      };

    struct EventEntry : public MethodEntry
      {
      mutable TWeakObjectPtr<UFunction> m_ue_method_to_invoke_p; // The copy of our method we actually can invoke

      EventEntry(SkMethodBase * sk_method_p, UFunction * ue_method_p, uint32_t num_params)
        : MethodEntry(sk_method_p, ue_method_p, num_params, MethodType_Event)
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

    void                prepare_method_entry(SkMethodBase * sk_method_p);
    int32_t             try_add_method_entry(UClass * ue_class_p, SkMethodBase * sk_method_p);
    int32_t             add_function_entry(UClass * ue_class_p, SkMethodBase * sk_method_p, const FString & category);
    int32_t             add_event_entry(UClass * ue_class_p, SkMethodBase * sk_method_p, const FString & category);
    void                delete_method_entry(uint32_t method_index);
    bool                make_param(ParamInfo * out_param_info_p, UFunction * ue_method_p, SkClassDescBase * sk_parameter_class_p, const FName & param_name);
    void                bind_event_method(SkMethodBase * sk_method_p);

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

    APArray<MethodEntry>  m_method_entry_array;

    UScriptStruct *       m_struct_vector3_p;
    UScriptStruct *       m_struct_rotation_angles_p;
    UScriptStruct *       m_struct_transform_p;

    static SkUEBlueprintInterface * ms_singleton_p; // Hack, make it easy to access for callbacks
        
  };