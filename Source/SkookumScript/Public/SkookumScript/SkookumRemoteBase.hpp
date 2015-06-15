//=======================================================================================
// SkookumScript C++ library.
// Copyright (c) 2011 Agog Labs Inc.,
// All rights reserved.
//
// SkookumScript Remote IDE Commands
// Author(s):   Conan Reis
// Notes:          
//=======================================================================================


#ifndef __SKOOKUMREMOTEBASE_HPP
#define __SKOOKUMREMOTEBASE_HPP


//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkookumScript.hpp>


//=======================================================================================
// Global Defines / Macros
//=======================================================================================

// SKOOKUM_IDE_EMBEDDED must be defined if the Skookum IDE is embedded in the application
// and SKOOKUM_REMOTE is defined if this application is a client/server that communicates
// with a remote Skookum IDE/runtime.  Neither is defined if there is no access to the
// Skookum IDE.
#if (SKOOKUM & SK_DEBUG) && !defined(SKOOKUM_IDE_EMBEDDED)
  // Default to use remote Skookum IDE for debugging.
  #define SKOOKUM_REMOTE
#endif


//=======================================================================================
// Global Structures
//=======================================================================================

// Forward Declarations
class ADatum;
class ASymbolTable;


//---------------------------------------------------------------------------------------
// Used to indicate both the online state:
//   SkLocale_embedded - Local Skookum environment debugged/controlled by its own local embedded IDE (not online)
//   SkLocale_runtime  - Client Skookum environment/runtime debugged/controlled by remote IDE
//   SkLocale_ide      - IDE remotely debugging/controlling client Skookum environment/runtime
//   
// and which side a given command should run on: local, remote or both.
//   SkLocale_local    - run command locally: translates to embedded, runtime or IDE as needed
//   SkLocale_remote   - run command remotely: translates to embedded, runtime or IDE as needed
//   SkLocale_all      - run command both locally and remotely
enum eSkLocale
  {
  // Actual codes
  SkLocale_embedded = 0x0, 
  SkLocale_runtime  = 1 << 0,
  SkLocale_ide      = 1 << 1,

  // Virtual codes
  SkLocale_local    = 1 << 2,  // Translated to embedded, runtime or IDE as needed
  SkLocale_remote   = 1 << 3,  // Translated to embedded, runtime or IDE as needed

  SkLocale_all      = SkLocale_runtime | SkLocale_ide | SkLocale_local | SkLocale_remote
  };


//---------------------------------------------------------------------------------------
enum eSkRemoteFlag
  {
  SkRemoteFlag_none        = 0x0, 
  SkRemoteFlag_symbol_db   = 1 << 0  // Remote side has a symbol database (Symbols need to be updated, less or no symbol id translation required)
  };

const uint32_t SkRemote_version_byte_size       = 9u;   // command(4) + version(1) + authorization(4)
const uint32_t SkRemote_version_reply_byte_size = 13u;  // command(4) + version(1) + authorization(4) + client_flags(4)

//---------------------------------------------------------------------------------------
// Skookum remote IDE communication commands that are common to both the Skookum client
// environment and the server IDE.
class SkookumRemoteBase
  {
  public:
  // Nested Types

    enum eCommand
      {                                // (I)DE, (R)untime or (C)ommon
      // Start-up / Simple

        Command_version,                 // I->R cmd_version()
        Command_version_reply,           // R->I cmd_version_reply()
        Command_authenticate,            // I->R cmd_authenticate()
        Command_preferences,             // I->R cmd_preferences()

        Command_print,                   // C->C cmd_print()

      // No commands greater than this point are permitted until the client is authenticated.
      Command__auth_limit = Command_print,

      // IDE State

        Command_show,                    // R->I cmd_show()
        Command_disconnect,              // I->R cmd_disconnect() - Some socket implementations cannot detect or do not have an event for disconnection of a socket so explicitly tell runtim that IDE is disconnecting.

      // Invocation (REPL)

        Command_invoke,                  // C->C cmd_invoke()
        Command_invoke_result,           // C->C cmd_invoke_result()

      // Recompile and update

        Command_symbol_update,           // C->C cmd_symbols_update() - Packet of remote symbols to merge into local symbol DB
        Command_freshen_compiled,        // R->I cmd_compiled_state() - Determine if/ensure the compiled binary is up-to-date
        Command_freshen_compiled_reply,  // I->R cmd_compiled_state_reply() - Notify that the compiled binary is up-to-date or has errors
        Command_class_hierarchy_update,  // I->R cmd_hierarchy_update() - List of classes to create/remove
        Command_class_update,            // I->R cmd_class_update() - Class binary to replace existing or to create new
        Command_recompile_classes,       // R->I cmd_recompile_classes() - Request class (& optional subclasses) to be recompiled
        Command_recompile_classes_reply, // I->R cmd_recompile_classes_reply() - Result of class recompilation

      // Debugging

        Command_breakpoint_create,       // I->R cmd_breakpoint_create()
        Command_breakpoint_update,       // I->R cmd_breakpoint_update() - enable, disable, remove 1/all breakpoints
        Command_breakpoint_hit,          // R->I cmd_breakpoint_hit()
        Command_break_continue,          // I->R cmd_break_continue()
        Command_break_step,              // I->R cmd_break_step()
        Command_break_expression,        // R->I cmd_break_expression()
        Command_break_print_callstack,   // I->R cmd_print_callstack()
        Command_break_print_locals,      // I->R cmd_print_locals()


      Command__implemented_last = Command_recompile_classes_reply,

      // Planned Future Commands

        Command_ping,                    // C->C cmd_ping() - determines of other side is alive and response with Command_ping_reply
        Command_ping_reply,              // C->C cmd_ping_reply() - Acknowledgement of Command_ping

        Command_recompile_method,        // I->R
        Command_recompile_coroutine,     // I->R
        Command_recompile_incremental,
        Command_recompile_all,

        Command_browse,
        Command_browse_break,

        Command_debug_settings,          // Settings: thread locking

        Command_break_scripted,          // R->I
        Command_break_all,               // I->R
        Command_break_run_to_cursor,     // I->R Could be combination of breakpoint set and continue
        Command_break_set_statement,

        Command_locals,
        Command_locals_reply,

        Command_memory,
        Command_memory_reply,

      Command__last
      };

    enum eConnectState
      {
      ConnectState_connecting,       // -> authenticating | disconnecting/disconnected
      ConnectState_authenticating,   // -> authenticated | disconnecting
      ConnectState_authenticated,    // * End state - on-line  -> disconnecting
      ConnectState_disconnecting,    // -> disconnected
      ConnectState_disconnected      // * End state - off-line  -> connecting
      };

    enum eCompiledState
      {
      CompiledState_unknown,
      CompiledState_determining,
      CompiledState_compiling,
      CompiledState_stale,
      CompiledState_errors,
      CompiledState_fresh
      };


  // Public Class Data

    static SkookumRemoteBase * ms_default_p;
    static const uint8_t       ms_version;

  // Common Methods

    SkookumRemoteBase();
    ~SkookumRemoteBase()                                          { ms_default_p = nullptr; }


    eSkLocale    get_mode() const                                 { return m_mode; }
    virtual void set_mode(eSkLocale mode);
    bool         is_authenticating() const                        { return this && ((m_connect_state == ConnectState_authenticating) || (m_connect_state == ConnectState_authenticated)); }
    bool         is_authenticated() const                         { return this && (m_connect_state == ConnectState_authenticated); }
    bool         is_embedded() const                              { return (this == nullptr) || (m_mode == SkLocale_embedded); }
    bool         is_remote_ide() const                            { return this && (m_mode == SkLocale_ide); }
    bool         is_remote_runtime() const                        { return this && (m_mode == SkLocale_runtime); }
    bool         is_connecting() const                            { return this && (m_connect_state <= ConnectState_authenticated); }
    virtual bool is_connected() const                             { return this && (m_mode != SkLocale_embedded); }
    bool         is_symbol_db_remote() const                      { return this && ((m_remote_flags & SkRemoteFlag_symbol_db) != 0u); }
    virtual void disconnect();

    virtual bool is_class_ctor_called() const;
    
  // Commands

    #ifdef SKOOKUM_REMOTE

      // *Note all these commands do byte swapping as necessary since they could be run on
      // the client of any platform.

      void cmd_simple(eCommand cmd);
      void cmd_simple_uint32(eCommand cmd, uint32_t value);
      void cmd_print(const AString & str, uint32_t type);
      void cmd_invoke(const AString & code);
      void cmd_invoke_result(const AString & str);
      void cmd_symbols_update(const ASymbolTable & syms);

      virtual void on_cmd_invoke_result(AString & result_str);
      void         on_cmd_print(AString & str, uint32_t type_flags);
      virtual void on_cmd_send(const ADatum & datum) = 0;
      virtual bool on_cmd_recv(eCommand cmd, const uint8_t * data_p, uint32_t data_length);

    #endif  // SKOOKUM_REMOTE

  // Class Methods

    static eSkLocale locale_as_virtual(eSkLocale locale);
    static eSkLocale locale_as_actual(eSkLocale locale);

  protected:

  // Internal Methods

    void set_connect_state(eConnectState state);

    virtual void on_connect_change(eConnectState old_state);

  // Data Members

    eSkLocale     m_mode;
    eConnectState m_connect_state;

    // Flags from remote side - see eSkRemoteFlag
    uint32_t m_remote_flags;

  };  // SkookumRemoteBase


//=======================================================================================
// Inline Methods
//=======================================================================================


#endif  // __SKOOKUMREMOTEBASE_HPP

