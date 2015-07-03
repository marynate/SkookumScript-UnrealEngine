//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// Main entry point for the SkookumScript runtime plugin
// 
// Author: Conan Reis
//=======================================================================================

#include "SkookumScriptRuntimePrivatePCH.h"
#include "Bindings/SkUEBindings.hpp"
#include "Bindings/SkUERuntime.hpp"
#include "Bindings/SkUERemote.hpp"
#include "Bindings/SkUEBlueprintInterface.hpp"

#include "Runtime/Launch/Resources/Version.h"
#include "Runtime/Engine/Public/Tickable.h"
#include "Engine/World.h"

#include <SkUEWorld.generated.hpp>

#ifdef A_PLAT_PC
  #include <windows.h>  // Uses: IsDebuggerPresent(), OutputDebugStringA()
#endif


//---------------------------------------------------------------------------------------
class FSkookumScriptRuntime : public ISkookumScriptRuntime, public FTickableGameObject
  {
  public:

    FSkookumScriptRuntime();

  protected:

  // Methods

    // Overridden from IModuleInterface

    virtual void    StartupModule() override;
    virtual void    PostLoadCallback() override;
    virtual void    PreUnloadCallback() override;
    virtual void    ShutdownModule() override;

    // Overridden from FTickableGameObject

    virtual bool    IsTickable() const override;
    virtual bool    IsTickableWhenPaused() const override;
    virtual bool    IsTickableInEditor() const override;
    virtual void    Tick(float deltaTime) override;
    virtual TStatId GetStatId() const override;

    // Local methods

    void          on_world_init_pre(UWorld * world_p, const UWorld::InitializationValues init_vals);
    void          on_world_init_post(UWorld * world_p, const UWorld::InitializationValues init_vals);
    void          on_world_cleanup(UWorld * world_p, bool session_ended_b, bool cleanup_resources_b);
    void          on_asset_loaded(UObject * new_object_p);

    void          set_game_world(UWorld * world_p);

    virtual void  set_editor_interface(ISkookumScriptRuntimeEditorInterface * editor_interface_p);
    virtual bool  is_skookum_blueprint_function(UFunction * function_p) const override;
    virtual bool  is_skookum_blueprint_event(UFunction * function_p) const override;

  // Data Members

    SkUERuntime                             m_runtime;
    ISkookumScriptRuntimeEditorInterface *  m_editor_interface_p;

    #ifdef SKOOKUM_REMOTE_UNREAL
      SkUERemote m_remote_client;
    #endif

    UWorld *          m_game_world_p;

    FDelegateHandle   m_on_world_init_pre_handle;
    FDelegateHandle   m_on_world_init_post_handle;
    FDelegateHandle   m_on_world_cleanup_handle;
    FDelegateHandle   m_on_asset_loaded_handle;
  };


//---------------------------------------------------------------------------------------
// Simple error dialog until more sophisticated one in place.
// Could communicate remotely with Skookum IDE and have it bring up message window.
class ASimpleErrorOutput : public AErrorOutputBase
  {
    public:

    virtual bool determine_choice(const AErrMsg & info, eAErrAction * action_p) override;

  };

//---------------------------------------------------------------------------------------
// Determines which error choice to take by prompting the user.  It also writes out
// information to the default output window(s).
// 
// # Returns:  true if a user break should be made in the debugger, false if not
// 
// # Params:
//   msg:      See the definition of `AErrMsg` in ADebug.hpp for more information.
//   action_p: address to store chosen course of action to take to resolve error
//   
// # Author(s): Conan Reis
bool ASimpleErrorOutput::determine_choice(
  const AErrMsg & msg,
  eAErrAction *   action_p
  )
  {
  const char * title_p;
  const char * choice_p    = NULL;
  bool         dbg_present = FPlatformMisc::IsDebuggerPresent();

  // Set pop-up attributes and default values
  switch (msg.m_err_level)
    {
    case AErrLevel_internal:
      title_p = (msg.m_title_p ? msg.m_title_p : "Internal recoverable exception");
      break;

    default:
      title_p  = (msg.m_title_p ? msg.m_title_p : "Error");
      choice_p =
        "\nChoose:\n"
        "  'Abort'  - break into C++ & get callstack [then ignore on continue]\n"
        "  'Retry'  - attempt recovery/ignore [still tests this assert in future]\n"
        "  'Ignore' - recover/ignore always [auto-ignores this assert in future]";
    }

  // Format description
  char         desc_p[2048];
  AString      desc(desc_p, 2048, 0u, false);
  const char * high_desc_p = msg.m_desc_high_p ? msg.m_desc_high_p : "An error has occurred.";
  const char * low_desc_p  = msg.m_desc_low_p  ? msg.m_desc_low_p  : "";
  const char * func_desc_p = msg.m_func_name_p ? msg.m_func_name_p : "";

  desc.insert(high_desc_p);
  desc.append(ADebug::context_string());

  // Ensure there is some space
  desc.ensure_size_extra(512u);

  if (msg.m_source_path_p)
    {
    desc.append_format("\n\n  C++ Internal Info:\n    %s\n    %s(%u) :\n    %s\n", func_desc_p, msg.m_source_path_p, msg.m_source_line, low_desc_p, msg.m_err_id);
    }
  else
    {
    desc.append_format("\n\n  C++ Internal Info:\n    %s\n    %s\n", func_desc_p, low_desc_p, msg.m_err_id);
    }

  // Print out to debug system first
  ADebug::print_format("\n###%s : ", title_p);
  ADebug::print(desc);

  desc.append(choice_p);

  // Prompt user (if necessary)
  eAErrAction action     = AErrAction_ignore;
  bool        user_break = false;

  if (choice_p)
    {
    #if defined(A_PLAT_PC)

      int result = ::MessageBoxA(
        NULL, desc, title_p, MB_ICONEXCLAMATION | MB_ABORTRETRYIGNORE | MB_DEFBUTTON1 | MB_SETFOREGROUND | MB_APPLMODAL);

      switch (result)
        {
        case IDABORT:    // Abort button was selected.
          user_break = true;
          action     = AErrAction_ignore;
          break;

        case IDRETRY:    // Retry button was selected.
          action = AErrAction_ignore;
          break;

        case IDIGNORE:   // Ignore button was selected.
          action = AErrAction_ignore_all;
          break;
        }
    #else
      user_break = dbg_present;
    #endif
    }

  *action_p = action;

  return user_break;
  }


//=======================================================================================
// Global Function Definitions
//=======================================================================================

IMPLEMENT_MODULE(FSkookumScriptRuntime, SkookumScriptRuntime)
DEFINE_LOG_CATEGORY(LogSkookum);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Memory Allocation with Descriptions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// These must be defined because A_MEMORY_FUNCS_PRESENT is set in AgogCore/AgogExtHook.hpp

// Note that the new/delete allocation functions without description arguments are defined
// in the IMPLEMENT_MODULE macro above:
//   ```
//   void * operator new(size_t Size);
//   void * operator new[](size_t Size);
//   void   operator delete(void * Ptr);
//   void   operator delete[](void * Ptr);
//   ```

//---------------------------------------------------------------------------------------
void * operator new(size_t size, const char * desc_cstr_p)
  {
  return FMemory::Malloc(size, 16); // $Revisit - MBreyer Make alignment controllable by caller
  }

//---------------------------------------------------------------------------------------
void * operator new[](size_t size, const char * desc_cstr_p)
  {
  return FMemory::Malloc(size, 16); // $Revisit - MBreyer Make alignment controllable by caller
  }

// $Note - *** delete operators with additional arguments cannot be called explicitly
// - they are only called on class destruction on a paired new with similar arguments.
// This means that a simple delete call will often call the single argument operator
// delete(void * mem_p) so take care to ensure that the proper delete is paired with a
// corresponding new.

//---------------------------------------------------------------------------------------
void operator delete(void * buffer_p, const char * desc_cstr_p)
  {
  FMemory::Free(buffer_p);
  }

//---------------------------------------------------------------------------------------
void operator delete[](void * buffer_p, const char * desc_cstr_p)
  {
  FMemory::Free(buffer_p);
  }


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Agog Namespace Functions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace Agog
  {

  //---------------------------------------------------------------------------------------
  // Description Returns constants / values used by the AgogCore library.
  // Returns     constants / values used by the AgogCore library.
  // Examples    Called internally
  // Author(s)   Conan Reis
  AgogCoreVals & get_agog_core_vals()
    {
    static AgogCoreVals s_values;

    //if (s_values.m_using_defaults)
    //  {
    //  // Set custom initial values
    //  s_values.m_using_defaults = false;
    //  }

    return s_values;
    }

  //---------------------------------------------------------------------------------------
  // Prints supplied C-string to debug console - which can be a debugger window, standard
  // out, something custom in the app, etc.
  // 
  // #Notes
  //   Called by ADebug print functions & A_DPRINT() which call the ADebug print functions.
  //   
  // #See Also
  //   ADebug print functions which all call this function.
  //   Also the functions ADebug::register_print_*() can register additional print
  //   functions.
  void dprint(const char * cstr_p)
    {
    #ifdef A_PLAT_PC
      if (FPlatformMisc::IsDebuggerPresent())
        {
        // Note that Unicode version OutputDebugStringW() actually calls OutputDebugStringA()
        // so calling it directly is faster.
        ::OutputDebugStringA(cstr_p);
        }
      //else
    #endif
      //  {
      //  ADebug::print_std(cstr_p);
      //  }
    }

  //---------------------------------------------------------------------------------------
  // Description Called whenever an error occurs but *before* a choice has been made as to
  //             how it should be resolved.  It optionally creates an error output object
  //             that will have its determine_choice() called if 'nested' is false.
  // Returns     an AErrorOutputBase object to call determine_choice() on or NULL if a
  //             default resolve error choice is to be made without prompting the user with
  //             output to the debug output window.
  // Arg         nested - Indicates whether the error is nested inside another error - i.e.
  //             an additional error happened before a prior error was fully resolved
  //             (while unwinding the stack on a 'continue' exception throw for example).
  //             determine_choice() will *not* be called if 'nested' is true.
  // Examples    Called by ADebug class - before on_error_post() and on_error_quit()
  // See Also    ADebug, AErrPopUp, AErrorDialog
  // Author(s)   Conan Reis
  AErrorOutputBase * on_error_pre(bool nested)
    {
    static ASimpleErrorOutput s_simple_err_out;

    return &s_simple_err_out;
    }

  //---------------------------------------------------------------------------------------
  // Description Called whenever an error occurs and *after* a choice has been made as to
  //             how it should be resolved.
  // Arg         action - the action that will be taken to attempt resolve the error.
  // Examples    Called by ADebug class - after on_error_pre() and before on_error_quit()
  // Author(s)   Conan Reis
  void on_error_post(eAErrAction action)
    {
    // Depending on action could switch back to fullscreen
    }

  //---------------------------------------------------------------------------------------
  // Description Called if 'Quit' is chosen during error.
  // Examples    Called by ADebug class - after on_error_pre() and on_error_post()
  // Author(s)   Conan Reis
  void on_error_quit()
    {
    //AApplication::shut_down();
    
    exit(EXIT_FAILURE);
    }

  //---------------------------------------------------------------------------------------
  void * malloc_func(size_t size, const char * name_p)
    {
    return size ? FMemory::Malloc(size, 16) : nullptr; // $Revisit - MBreyer Make alignment controllable by caller
    }

  //---------------------------------------------------------------------------------------
  void free_func(void * mem_p)
    {
    if (mem_p) FMemory::Free(mem_p); // $Revisit - MBreyer Make alignment controllable by caller
    }

  //---------------------------------------------------------------------------------------
  // Converts the size requested to allocate to the actual amount allocated.
  uint32_t req_byte_size_func(uint32_t bytes_requested)
    {
    // Assume 1:1 for default
    return bytes_requested;
    }

  }  // End Agog namespace


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Skookum Namespace Functions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace Skookum
  {

  //---------------------------------------------------------------------------------------
  // Description Returns constants / values used by the SkookumScript library.
  // Returns     constants / values used by the SkookumScript library.
  // Examples    Called internally
  // Author(s)   Conan Reis
  SkookumVals & get_lib_vals()
    {
    static SkookumVals s_values;

    if (s_values.m_using_defaults)
      {
      // Set custom initial values
      s_values.m_using_defaults = false;

      // Unreal uses its own actor class
      s_values.m_use_builtin_actor = false; 
      s_values.m_custom_actor_class_name = "Actor";

      // Get platform name
      s_values.m_platform_id_string = FStringToAString(UGameplayStatics::GetPlatformName());

      // Get engine name
      s_values.m_engine_id_string.ensure_size(20);
      s_values.m_engine_id_string.format("UE%d.%d.%d-%s", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ENGINE_PATCH_VERSION, BUILT_FROM_CHANGELIST ? "Installed" : "Compiled");
      }

    return s_values;
    }

  }


//---------------------------------------------------------------------------------------
FSkookumScriptRuntime::FSkookumScriptRuntime()
  : m_editor_interface_p(nullptr)
  , m_game_world_p(nullptr)
  {
  //m_runtime.set_compiled_path("Scripts" SK_BITS_ID "\\");
  }

//---------------------------------------------------------------------------------------
// This code will execute after your module is loaded into memory (but after global
// variables are initialized, of course.)
void FSkookumScriptRuntime::StartupModule()
  {
  A_DPRINT(A_SOURCE_STR " Starting up SkookumScript plug-in modules\n");

  // Note that FWorldDelegates::OnPostWorldCreation has world_p->WorldType set to None
  // Note that FWorldDelegates::OnPreWorldFinishDestroy has world_p->GetName() set to "None"

  auto on_world_init_pre_delegate = FWorldDelegates::FWorldInitializationEvent::FDelegate::CreateRaw(this, &FSkookumScriptRuntime::on_world_init_pre);
  auto on_world_init_post_delegate = FWorldDelegates::FWorldInitializationEvent::FDelegate::CreateRaw(this, &FSkookumScriptRuntime::on_world_init_post);
  auto on_world_cleanup_delegate = FWorldDelegates::FWorldCleanupEvent::FDelegate::CreateRaw(this, &FSkookumScriptRuntime::on_world_cleanup);
  auto on_asset_loaded_delegate = FCoreUObjectDelegates::FOnAssetLoaded::FDelegate::CreateRaw(this, &FSkookumScriptRuntime::on_asset_loaded);

  m_on_world_init_pre_handle = FWorldDelegates::OnPreWorldInitialization.Add(on_world_init_pre_delegate);
  m_on_world_init_post_handle = FWorldDelegates::OnPostWorldInitialization.Add(on_world_init_post_delegate);
  m_on_world_cleanup_handle = FWorldDelegates::OnWorldCleanup.Add(on_world_cleanup_delegate);
  m_on_asset_loaded_handle = FCoreUObjectDelegates::OnAssetLoaded.Add(on_asset_loaded_delegate);

  // Hook up Unreal memory allocator
  AMemory::override_functions(&Agog::malloc_func, &Agog::free_func, &Agog::req_byte_size_func);

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Start up SkookumScript
  m_runtime.on_init();
  }

//---------------------------------------------------------------------------------------
// Called after the module has been reloaded
void FSkookumScriptRuntime::PostLoadCallback()
  {
  A_DPRINT(A_SOURCE_STR " SkookumScript - loaded.\n");
  }

//---------------------------------------------------------------------------------------
void FSkookumScriptRuntime::on_world_init_pre(UWorld * world_p, const UWorld::InitializationValues init_vals)
  {
  //A_DPRINT("on_world_init_pre: %S %p\n", *world_p->GetName(), world_p);

  #ifdef SKOOKUM_REMOTE_UNREAL
    SkUERemote::ms_client_p->ensure_connected();
  #endif

  if (world_p->IsGameWorld())
    {
    // Set global world pointer
    set_game_world(world_p);
    }
  }

//---------------------------------------------------------------------------------------
void FSkookumScriptRuntime::on_world_init_post(UWorld * world_p, const UWorld::InitializationValues init_vals)
  {
  //A_DPRINT("on_world_init_post: %S %p\n", *world_p->GetName(), world_p);
  }

//---------------------------------------------------------------------------------------
void FSkookumScriptRuntime::on_world_cleanup(UWorld * world_p, bool session_ended_b, bool cleanup_resources_b)
  {
  //A_DPRINT("on_world_cleanup: %S %p\n", *world_p->GetName(), world_p);

  if (world_p->IsGameWorld())
    {
    // Set world pointer to null if it was pointing to us
    if (m_game_world_p == world_p)
      {
      // Clear global world pointer
      set_game_world(nullptr);
      }

    // Simple shutdown
    //SkookumScript::get_world()->clear_coroutines();
    A_DPRINT(
      "SkookumScript resetting session...\n"
      "  cleaning up...\n");
    SkookumScript::deinitialize_session();
    SkookumScript::initialize_session();
    A_DPRINT("  ...done!\n\n");
    }
  }

//---------------------------------------------------------------------------------------
void FSkookumScriptRuntime::on_asset_loaded(UObject * new_object_p)
  {
  // is this a new blueprint?
  UBlueprint * blueprint_p = Cast<UBlueprint>(new_object_p);
  if (blueprint_p)
    {
    // Reinitialize bindings for this new blueprint
    SkClass * sk_class_p = SkUEClassBindingHelper::get_sk_class_from_ue_class(blueprint_p->GeneratedClass);
    if (sk_class_p)
      {
      m_runtime.get_blueprint_interface()->reinitialize_class(sk_class_p);
      }
    }
  }

//---------------------------------------------------------------------------------------
// Called before the module has been unloaded
void FSkookumScriptRuntime::PreUnloadCallback()
  {
  A_DPRINT(A_SOURCE_STR " SkookumScript - about to unload.\n");
  }

//---------------------------------------------------------------------------------------
// This function may be called during shutdown to clean up your module.  For modules that
// support dynamic reloading, we call this function before unloading the module.
void FSkookumScriptRuntime::ShutdownModule()
  {
  A_DPRINT(A_SOURCE_STR " Shutting down SkookumScript plug-in modules\n");

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Clean up SkookumScript
  m_runtime.on_deinit();

  #ifdef SKOOKUM_REMOTE_UNREAL
    // Remote communication to and from SkookumScript IDE
    m_remote_client.disconnect();
  #endif

  // Clear out our registered delegates
  FWorldDelegates::OnPreWorldInitialization.Remove(m_on_world_init_pre_handle);
  FWorldDelegates::OnPostWorldInitialization.Remove(m_on_world_init_post_handle);
  FWorldDelegates::OnWorldCleanup.Remove(m_on_world_cleanup_handle);
  FCoreUObjectDelegates::OnAssetLoaded.Remove(m_on_asset_loaded_handle);
  }

//---------------------------------------------------------------------------------------
// Determines whether an object is ready to be ticked. This is required for example for
// all UObject derived classes as they might be loaded async and therefore won't be ready
// immediately.
bool FSkookumScriptRuntime::IsTickable() const
  {
  return true;
  }

// Used to determine if an object should be ticked when the game is paused.
bool FSkookumScriptRuntime::IsTickableWhenPaused() const
  {
  return true;
  }

// Used to determine whether the object should be ticked in the editor
bool FSkookumScriptRuntime::IsTickableInEditor() const
  {
  // Yes - since remote IDE update also occurs there
  return true;
  }

//---------------------------------------------------------------------------------------
// Called from within LevelTick.cpp after ticking all actors or from the rendering thread
// (depending on bIsRenderingThreadObject)
//
// #Params:
//   deltaTime: Game time passed since the last call.
//   
// #Author(s): Conan Reis
void FSkookumScriptRuntime::Tick(float deltaTime)
  {
  #ifdef SKOOKUM_REMOTE_UNREAL
    // Remote communication to and from SkookumScript IDE.
    // Needs to be called whether in editor or game and whether paused or not
    // $Revisit - CReis This is probably a hack. The remote client update should probably
    // live somewhere other than a tick method such as its own thread.
    m_remote_client.process_incoming();
  #endif

  if (m_game_world_p)
    {
    // Intentionally still called even when paused and deltaTime is 0.0f
    m_runtime.update(deltaTime);
    }
  }

//---------------------------------------------------------------------------------------
// return the stat id to use for this tickable
//   
// #Author(s): Conan Reis
TStatId FSkookumScriptRuntime::GetStatId() const
  {
  RETURN_QUICK_DECLARE_CYCLE_STAT(FSkookumScriptRuntime, STATGROUP_Tickables);
  }

//---------------------------------------------------------------------------------------
// Set the game world (C++ and Skookum variable) to a specific world object
void FSkookumScriptRuntime::set_game_world(UWorld * world_p)
  {
  m_game_world_p = world_p;
  SkClass * obj_class_p = SkBrain::get_class(ASymbol_Object);
  SK_ASSERTX(obj_class_p, "Couldn't find the World class!");
  uint32_t var_pos;
  if (obj_class_p->get_class_data().find(ASymbolX_c_world, AMatch_first_found, &var_pos))
    {
    obj_class_p->get_class_data().get_at(var_pos)->set_data(world_p ? SkUEWorld::new_instance(world_p) : SkBrain::ms_nil_p);
    }
  else
    {
    SK_ASSERTX(false, "Couldn't find the @@world class member variable!");
    }
  }

//---------------------------------------------------------------------------------------
// 
void FSkookumScriptRuntime::set_editor_interface(ISkookumScriptRuntimeEditorInterface * editor_interface_p)
  {
  m_editor_interface_p = editor_interface_p;
  #ifdef SKOOKUM_REMOTE_UNREAL
    m_remote_client.set_editor_interface(editor_interface_p);
  #endif
  }

//---------------------------------------------------------------------------------------
// 
bool FSkookumScriptRuntime::is_skookum_blueprint_function(UFunction * function_p) const
  {
  return m_runtime.get_blueprint_interface()->is_skookum_blueprint_function(function_p);
  }

//---------------------------------------------------------------------------------------
// 
bool FSkookumScriptRuntime::is_skookum_blueprint_event(UFunction * function_p) const
  {
  return m_runtime.get_blueprint_interface()->is_skookum_blueprint_event(function_p);
  }


//#pragma optimize("g", on)
