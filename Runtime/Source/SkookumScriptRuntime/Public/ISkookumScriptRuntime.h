//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// Main entry point for the SkookumScript runtime plugin
// 
// Author: Markus Breyer
//=======================================================================================

#pragma once

#include "ModuleManager.h"

class SkClass;
class UClass;

//---------------------------------------------------------------------------------------
// Interface class for the runtime plugin to call the editor plugin
class ISkookumScriptRuntimeEditorInterface
  {
  public:

    virtual void on_class_updated(SkClass * sk_class_p, UClass * ue_class_p) = 0;

  };


//---------------------------------------------------------------------------------------
// The public interface to this module
class ISkookumScriptRuntime : public IModuleInterface
  {
  public:

    // Methods

    virtual void  set_editor_interface(ISkookumScriptRuntimeEditorInterface * editor_interface_p) = 0;

    virtual bool  is_skookum_blueprint_function(UFunction * function_p) const = 0;
    virtual bool  is_skookum_blueprint_event(UFunction * function_p) const = 0;

  };

