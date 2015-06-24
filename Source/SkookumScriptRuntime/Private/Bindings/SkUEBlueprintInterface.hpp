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

  protected:

    typedef SkInstance *  (*tParamFetcher)(FFrame & stack);
    typedef void          (*tResultSetter)(void * const result_p, SkInstance * value_p);

    struct ParamEntry
      {
      ASymbol       m_name;
      tParamFetcher m_fetcher_p;
      };

    struct MethodEntry
      {
      SkMethodBase *            m_sk_method_p;
      TWeakObjectPtr<UFunction> m_ue_method_p;
      uint32_t                  m_num_params;
      tResultSetter             m_result_setter_p;

      MethodEntry(SkMethodBase * sk_method_p, UFunction * ue_method_p, uint32_t num_params, tResultSetter result_setter_p)
        : m_sk_method_p(sk_method_p)
        , m_ue_method_p(ue_method_p)
        , m_num_params(num_params)
        , m_result_setter_p(result_setter_p)
        {}

      // The parameter entries are stored behind this structure in memory
      ParamEntry *       get_param_entry_array()       { return (ParamEntry *)(this + 1); }
      const ParamEntry * get_param_entry_array() const { return (const ParamEntry *)(this + 1); }
      };

    struct ParamInfo
      {
      UProperty *     m_ue_param_p;
      tParamFetcher   m_param_fetcher_p;
      tResultSetter   m_result_setter_p;
      };

    void                exec_method(FFrame & stack, void * const result_p, SkInstance * this_p);
    void                exec_class_method(FFrame & stack, void * const result_p);
    void                exec_instance_method(FFrame & stack, void * const result_p);

    int32_t             add_method_entry(UClass * ue_class_p, SkMethodBase * sk_method_p, const FString & category);
    void                delete_method_entry(uint32_t method_index);
    static bool         make_param(ParamInfo * out_param_info_p, UFunction * ue_method_p, SkClassDescBase * sk_parameter_class_p, const FName & param_name);

    static SkInstance * fetch_param_boolean(FFrame & stack);
    static SkInstance * fetch_param_integer(FFrame & stack);
    static SkInstance * fetch_param_real(FFrame & stack);
    static SkInstance * fetch_param_string(FFrame & stack);
    static SkInstance * fetch_param_vector3(FFrame & stack);
    static SkInstance * fetch_param_rotation_angles(FFrame & stack);
    static SkInstance * fetch_param_transform(FFrame & stack);
    static SkInstance * fetch_param_entity(FFrame & stack);

    static void         set_result_boolean(void * const result_p, SkInstance * value_p);
    static void         set_result_integer(void * const result_p, SkInstance * value_p);
    static void         set_result_real(void * const result_p, SkInstance * value_p);
    static void         set_result_string(void * const result_p, SkInstance * value_p);
    static void         set_result_vector3(void * const result_p, SkInstance * value_p);
    static void         set_result_rotation_angles(void * const result_p, SkInstance * value_p);
    static void         set_result_transform(void * const result_p, SkInstance * value_p);
    static void         set_result_entity(void * const result_p, SkInstance * value_p);

    APArray<MethodEntry>            m_method_entry_array;
    static SkUEBlueprintInterface * ms_singleton_p; // Hack, make it easy to access for callbacks
        
  };