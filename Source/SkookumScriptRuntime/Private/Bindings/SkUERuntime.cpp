//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2014 Agog Labs Inc.,
// All rights reserved.
//
// SkookumScript Runtime Hooks for Unreal - Input/Output Init/Update/Deinit Manager
// 
// #Author(s):  Conan Reis
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include "../SkookumScriptRuntimePrivatePCH.h"
#include "SkUERuntime.hpp"
#include "SkUERemote.hpp"
#include "SkUEBindings.hpp"

#include "GenericPlatformProcess.h"
#include <chrono>


//=======================================================================================
// Local Global Structures
//=======================================================================================

namespace
{

  //---------------------------------------------------------------------------------------
  // Custom Unreal Binary Handle Structure
  struct SkBinaryHandleUE : public SkBinaryHandle
    {
    // Public Methods

      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      SkBinaryHandleUE(void * binary_p, uint32_t size)
        {
        m_binary_p = binary_p;
        m_size = size;
        }

      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      static SkBinaryHandleUE * create(const TCHAR * path_p)
        {
        FArchive * reader_p = IFileManager::Get().CreateFileReader(path_p);

        if (!reader_p)
          {
          return nullptr;
          }
    
        int32   size     = reader_p->TotalSize();
        uint8 * binary_p = (uint8*)FMemory::Malloc(size);

        if (!binary_p)
          {
          reader_p->Close();
          delete reader_p;

          return nullptr;
          }

        reader_p->Serialize(binary_p, size);
        reader_p->Close();
        delete reader_p;

        return new SkBinaryHandleUE(binary_p, size);
        }
    };


} // End unnamed namespace


//=======================================================================================
// SkUERuntime Methods
//=======================================================================================

//---------------------------------------------------------------------------------------
// Description Initializes Skookum session.
// See Also    on_deinit()
// Author(s)   Conan Reis
void SkUERuntime::on_init()
  {
  A_DPRINT("\nSkookumScript starting up.\n");


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // One time-only per application session initializations
  static bool s_once_per_app_session_init = false;

  if (!s_once_per_app_session_init) 
    {
    // Let scripting system know that the game engine is present and is being hooked-in
    SkDebug::enable_engine_present();

    #ifdef SKOOKUM_REMOTE_UNREAL
      SkDebug::register_print_with_agog();
    #endif

    SkBrain::register_bind_atomics_func(SkookumRuntimeBase::bind_routines);

    s_once_per_app_session_init = true;
    }

  #ifdef SKOOKUM_REMOTE_UNREAL
    // Auto-connect to remote IDE
    if (SkookumRemoteRuntimeBase::ms_client_p->ensure_connected())
      {
      // Ensure compiled binaries are up to date
      switch (SkookumRemoteRuntimeBase::ms_client_p->ensure_compiled())
        {
        case AConfirm_yes: 
          SkDebug::print("\nSkookumNet: Compiled binaries are up-to-date.\n");
          break;

        case AConfirm_no:
          // $Revisit - CReis If not compiled then load old or quit
          SkDebug::print("\nSkookumNet: Compiled binaries are *not* up-to-date.\n", SkLocale_all, SkDPrintType_warning);
          break;

        default: // AConfirm_abort
          SkDebug::print("\nSkookumNet: Timed out while compiling binaries.\n", SkLocale_all, SkDPrintType_warning);
        }
      }
    else
      {
      SkDebug::print("\nSkookumNet: Timed out trying to connect!\n", SkLocale_all, SkDPrintType_warning);
      }
  #endif  // SKOOKUM_REMOTE_UNREAL

  // Load the Skookum class hierarchy scripts in compiled binary form
  bool success_b = load_compiled_scripts();

  SK_ASSERTX(success_b, AErrMsg("Unable to load SkookumScript compiled binaries!", AErrLevel_notify));
  }

//---------------------------------------------------------------------------------------
// Override to add bindings to any custom C++ routines (methods & coroutines).
//
// #See Also   SkBrain::register_bind_atomics_func()
// #Modifiers  virtual
// #Author(s)  Conan Reis
void SkUERuntime::on_bind_routines()
  {
  A_DPRINT(A_SOURCE_STR "\nBind routines for SkUERuntime.\n");

  SkUEBindings::register_all();
  }

//---------------------------------------------------------------------------------------
// Author(s)   Conan Reis
void SkUERuntime::on_deinit()
  {
  A_DPRINT("\nSkookumScript shutting down.\n");

  #ifdef SKOOKUM_REMOTE_UNREAL
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Disconnect from remote client
    SkookumRemoteBase::ms_default_p->set_mode(SkLocale_embedded);
  #endif

  deinit();
  }

//---------------------------------------------------------------------------------------
// #Author(s):  Conan Reis
void SkUERuntime::deinit()
  {
  if (SkookumScript::is_flag_set(SkookumScript::Flag_updating))
    {
    A_DPRINT("\n  Attempting to shut down SkookumScript while it is in the middle of an update.\n");
    
    A_DPRINT("  ...might need to defer shutdown until after update is completed.\n");

    // $Revisit - CReis May still need defer mechanism
    // Still in the middle of an update - defer it
    //A_DPRINT("  ...deferring shutdown until after update is completed.\n");
    //ADeferFunc::post_func(deinit);
    //return;
    }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Unloads SkookumScript and cleans-up
  SkookumScript::deinitialize_session();
  SkookumScript::deinitialize();
  }

//---------------------------------------------------------------------------------------
// Determine the compiled file path
//   - usually Content\SkookumScript\Compiled[bits]\Classes.sk-bin
// 
// #Author(s):  Conan Reis
const FString & SkUERuntime::get_compiled_path() const
  {
  if (!m_compiled_file_b)
    {
    m_compiled_path =
      FPaths::GameContentDir() / TEXT("SkookumScript/Compiled") TEXT(SK_BITS_ID);

    FString compiled_file = m_compiled_path / TEXT("Classes.sk-bin");

    // If not found in *game* content directory then try *engine* directory
    m_compiled_file_b = FPaths::FileExists(compiled_file);

    if (!m_compiled_file_b)
      {
      // Don't change m_compiled_path yet so the game content version stays the default if
      // neither the game nor engine versions are found.
      FString compiled_path =
        FPaths::EngineContentDir() / TEXT("SkookumScript/Compiled") TEXT(SK_BITS_ID);

      compiled_file = compiled_path / TEXT("Classes.sk-bin");

      m_compiled_file_b = FPaths::FileExists(compiled_file);

      if (m_compiled_file_b)
        {
        m_compiled_path = compiled_path;
        }
      }
    }

  return m_compiled_path;
  }

//---------------------------------------------------------------------------------------
// Load the Skookum class hierarchy scripts in compiled binary form.
// 
// #Params
//   ensure_atomics:
//     If set makes sure all atomic (C++) scripts that were expecting a C++ function to be
//     bound/hooked up to them do actually have a binding.
//   ignore_classes_pp:
//     array of classes to ignore when ensure_atomics is set.
//     This allows some classes with optional or delayed bindings to be skipped such as
//     bindings to a in-game world editor.
//   ignore_count:  number of class pointers in ignore_classes_pp
//
// #Returns
//   true if compiled scrips successfully loaded, false if not
// 
// #See:        load_compiled_class_group(), SkCompiler::compiled_load()
// #Modifiers:  static
// #Author(s):  Conan Reis
bool SkUERuntime::load_compiled_scripts(
  bool       ensure_atomics,     // = true
  SkClass ** ignore_classes_pp,  // = nullptr
  uint32_t   ignore_count        // = 0u
  )
  {
  A_DPRINT("\nSkookumScript loading previously parsed compiled binary...\n");

  if (load_compiled_hierarchy() != SkLoadStatus_ok)
    {
    return false;
    }

  A_DPRINT("  ...done!\n\n");


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Bind atomics
  A_DPRINT("SkookumScript binding with C++ routines...\n");

  // Registers/connects Generic SkookumScript atomic classes, stimuli, coroutines, etc.
  // with the compiled binary that was just loaded.
  SkookumScript::initialize_post_load();

  #if (SKOOKUM & SK_DEBUG)
    // Ensure atomic (C++) methods/coroutines are properly bound to their C++ equivalents
    if (ensure_atomics)
      {
      SkBrain::ensure_atomics_registered(ignore_classes_pp, ignore_count);
      }
  #endif

  A_DPRINT("  ...done!\n\n");


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Enable SkookumScript evaluation
  SkookumScript::enable_flag(SkookumScript::Flag_evaluate);

  A_DPRINT("SkookumScript initializing session...\n");
  SkookumScript::initialize_session();
  A_DPRINT("  ...done!\n\n");

  return true;
  }

//---------------------------------------------------------------------------------------
// Determines if binary for class hierarchy and associated info exists.
// 
// This check is done before the symbol file check since the symbol file is only needed
// when debugging and giving an error about missing the compiled binary is more intuitive
// to the end user than a missing symbol file.
// 
// #See Also:   get_binary_hierarchy()
// #Modifiers:  virtual - overridden from SkookumRuntimeBase
// #Author(s):  Conan Reis
bool SkUERuntime::is_binary_hierarchy_existing()
  {
  get_compiled_path();

  return m_compiled_file_b;
  }

//---------------------------------------------------------------------------------------
// Gets memory representing binary for class hierarchy and associated info.
// 
// #See Also:   load_compiled_scripts(), SkCompiler::get_binary_class_group()
// #Modifiers:  virtual - overridden from SkookumRuntimeBase
// #Author(s):  Conan Reis
SkBinaryHandle * SkUERuntime::get_binary_hierarchy()
  {
  FString compiled_file = get_compiled_path() / TEXT("Classes.sk-bin");

  A_DPRINT("  Loading compiled binary file '%ls'...\n", *compiled_file);

  return SkBinaryHandleUE::create(*compiled_file);
  }

//---------------------------------------------------------------------------------------
// Gets memory representing binary for group of classes with specified class as root.
// Used as a mechanism to "demand load" scripts.
// 
// #See Also:   load_compiled_scripts(), SkCompiler::get_binary_class_group()
// #Modifiers:  virtual - overridden from SkookumRuntimeBase
// #Author(s):  Conan Reis
SkBinaryHandle * SkUERuntime::get_binary_class_group(const SkClass & cls)
  {
  FString compiled_file = get_compiled_path();
  
  // $Revisit - CReis Should use fast custom uint32_t to hex string function.
  compiled_file += a_cstr_format("/Class[%x].sk-bin", cls.get_name_id());
  return SkBinaryHandleUE::create(*compiled_file);
  }


#if (SKOOKUM & SK_DEBUG) && defined(A_SYMBOL_STR_DB)
  
//---------------------------------------------------------------------------------------
// Gets memory representing binary for class hierarchy and associated info.
// 
// #See Also:   load_compiled_scripts(), SkCompiler::get_binary_class_group()
// #Modifiers:  virtual - overridden from SkookumRuntimeBase
// #Author(s):  Conan Reis
SkBinaryHandle * SkUERuntime::get_binary_symbol_table()
  {
  FString sym_file = get_compiled_path() / TEXT("Classes.sk-sym");

  A_DPRINT("  Loading compiled binary symbol file '%ls'...\n", *sym_file);

  SkBinaryHandleUE * handle_p = SkBinaryHandleUE::create(*sym_file);

  // Ensure symbol table binary exists
  if (!handle_p)
    {
    A_DPRINT("  ...it does not exist!\n\n", *sym_file);
    }

  return handle_p;
  }                                                                  

#endif


//---------------------------------------------------------------------------------------
// #Author(s):  Conan Reis
void SkUERuntime::release_binary(SkBinaryHandle * handle_p)
  {
  delete static_cast<SkBinaryHandleUE *>(handle_p);
  }



