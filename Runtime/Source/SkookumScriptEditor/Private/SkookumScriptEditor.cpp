//=======================================================================================
// SkookumScript Unreal Engine Editor Plugin
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// Author: Markus Breyer
//=======================================================================================

#include "SkookumScriptEditorPrivatePCH.h"
#include <ISkookumScriptRuntime.h>
#include <BlueprintActionDatabase.h>
#include <BlueprintEditorUtils.h>
#include <K2Node_CallFunction.h>
#include <K2Node_Event.h>

DEFINE_LOG_CATEGORY(LogSkookumScriptEditor);

//---------------------------------------------------------------------------------------

class FSkookumScriptEditor : public ISkookumScriptEditor, public ISkookumScriptRuntimeEditorInterface
{
public:

protected:

  //---------------------------------------------------------------------------------------
  // IModuleInterface implementation

  virtual void StartupModule() override;
  virtual void ShutdownModule() override;

  //---------------------------------------------------------------------------------------
  // ISkookumScriptRuntimeEditorInterface implementation

  virtual void on_class_updated(SkClass * sk_class_p, UClass * ue_class_p) override;

  //---------------------------------------------------------------------------------------
  // Local implementation

  ISkookumScriptRuntime * get_runtime() const { return static_cast<ISkookumScriptRuntime *>(m_runtime_p.Get()); }

  // TSharedPtr holds on to the module so it can't go away while we need it
  TSharedPtr<IModuleInterface>  m_runtime_p;

};

IMPLEMENT_MODULE(FSkookumScriptEditor, SkookumScriptEditor)

//=======================================================================================
// IModuleInterface implementation
//=======================================================================================

//---------------------------------------------------------------------------------------

void FSkookumScriptEditor::StartupModule()
  {
  m_runtime_p = FModuleManager::Get().GetModule("SkookumScriptRuntime");
  get_runtime()->set_editor_interface(this);
  }

//---------------------------------------------------------------------------------------

void FSkookumScriptEditor::ShutdownModule()
  {
  get_runtime()->set_editor_interface(nullptr);
  m_runtime_p.Reset();
  }

//=======================================================================================		//=======================================================================================
// ISkookumScriptRuntimeEditorInterface implementation
//=======================================================================================

//---------------------------------------------------------------------------------------

void FSkookumScriptEditor::on_class_updated(SkClass * sk_class_p, UClass * ue_class_p)
  {
  // 1) Refresh actions (in Blueprint editor drop down menu)
  FBlueprintActionDatabase::Get().RefreshClassActions(ue_class_p);

  // Storage for gathered objects
  TArray<UObject*> obj_array;

  // 2) Refresh node display of all SkookumScript function call nodes
  obj_array.Reset();
  GetObjectsOfClass(UK2Node_CallFunction::StaticClass(), obj_array, true, RF_ClassDefaultObject | RF_PendingKill);
  for (auto obj_p : obj_array)
    {
    UK2Node_CallFunction * function_node_p = Cast<UK2Node_CallFunction>(obj_p);
    UFunction * target_function_p = function_node_p->GetTargetFunction();
    // Also refresh all nodes with no target function as it is probably a Sk function that was deleted
    //if (!target_function_p || get_runtime()->is_skookum_blueprint_function(target_function_p))
    if (target_function_p && get_runtime()->is_skookum_blueprint_function(target_function_p))
      {
      const UEdGraphSchema * schema_p = function_node_p->GetGraph()->GetSchema();
      schema_p->ReconstructNode(*function_node_p, true);
      }
    }

  // 3) Refresh node display of all SkookumScript event nodes
  obj_array.Reset();
  GetObjectsOfClass(UK2Node_Event::StaticClass(), obj_array, true, RF_ClassDefaultObject | RF_PendingKill);
  for (auto obj_p : obj_array)
    {
    UK2Node_Event * event_node_p = Cast<UK2Node_Event>(obj_p);
    UFunction * event_function_p = event_node_p->FindEventSignatureFunction();
    // Also refresh all nodes with no target function as it is probably a Sk function that was deleted
    //if (!event_function_p || get_runtime()->is_skookum_blueprint_event(event_function_p))
    if (event_function_p && get_runtime()->is_skookum_blueprint_event(event_function_p))
      {
      const UEdGraphSchema * schema_p = event_node_p->GetGraph()->GetSchema();
      schema_p->ReconstructNode(*event_node_p, true);
      }
    }
  }

//=======================================================================================
// FSkookumScriptEditor implementation
//=======================================================================================

