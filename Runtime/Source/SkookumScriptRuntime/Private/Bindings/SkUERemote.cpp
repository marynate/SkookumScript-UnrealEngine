//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript Remote Client
//
// Author:  Conan Reis
//=======================================================================================


//=======================================================================================
// Includes
//=======================================================================================

#include "SkookumScriptRuntimePrivatePCH.h"
#include "SkUERemote.hpp"
#include "Bindings/SkUEBlueprintInterface.hpp"
#include <AssertionMacros.h>
//#include <ws2tcpip.h>


//=======================================================================================
// Local Global Structures
//=======================================================================================

namespace
{
  const int32_t SkUERemote_ide_port = 12357;

} // End unnamed namespace


//=======================================================================================
// SkUERemote Methods
//=======================================================================================

#ifdef SKOOKUM_REMOTE_UNREAL

//---------------------------------------------------------------------------------------
// Constructor
// 
// #Author(s): Conan Reis
SkUERemote::SkUERemote() :
  m_socket_p(nullptr),
  m_data_idx(ADef_uint32),
  m_editor_interface_p(nullptr)
  {
  }

//---------------------------------------------------------------------------------------
// Destructor
// 
// #Author(s): Conan Reis
SkUERemote::~SkUERemote()
  {
  }

//---------------------------------------------------------------------------------------
// Processes any remotely received data and call on_cmd_recv() whenever enough data is
// accumulated.
// 
// #Author(s): Conan Reis
void SkUERemote::process_incoming()
  {
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Connected and data available?
  uint32 bytes_available;

  while (is_connected() && m_socket_p->HasPendingData(bytes_available))
    {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Get datum size & prep datum
    int32    bytes_read;
    uint32_t datum_size;

    if (m_data_idx == ADef_uint32)
      {
      // Not working on a partially filled datum so get size of new datum
      if (bytes_available < sizeof(uint32_t))
        {
        // wait until there is enough data for size
        return;
        }

      // Read datum size from socket
      m_socket_p->Recv(reinterpret_cast<uint8 *>(&datum_size), sizeof(uint32_t), bytes_read);

      bytes_available -= sizeof(uint32_t);
      datum_size -= ADatum_header_size;
      m_data_in.ensure_size(datum_size, false);
      m_data_in.set_data_length(datum_size);
      m_data_idx = 0u;
      }
    else
      {
      datum_size = m_data_in.get_data_length();
      }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Begin or resume filling datum
    uint32_t cmd;
    uint32_t bytes_to_read;
    uint8 *  buffer_p = m_data_in.get_data_writable();

    while (bytes_available
      || (is_connected() && m_socket_p->HasPendingData(bytes_available)))
      {
      bytes_to_read = a_min(bytes_available, datum_size - m_data_idx);

      // Read datum data from socket
      m_socket_p->Recv(buffer_p + m_data_idx, bytes_to_read, bytes_read);

      m_data_idx += bytes_read;
      bytes_available = 0; // Reset to refresh on next loop

      if (m_data_idx == datum_size)
        {
        // Datum fully received and ready to use
        m_data_idx = ADef_uint32;

        // Parse command from IDE
        A_BYTE_STREAM_IN32(&cmd, &buffer_p);
        on_cmd_recv(eCommand(cmd), buffer_p, datum_size - 4u);

        // Exit filling datum while loop & look for new datum
        break;
        }
      // loop back and continue to try filling datum
      }
    } // While connected and has data
  }

//---------------------------------------------------------------------------------------
// Get (ANSI) string representation of socket IP Address and port
// 
// #Author(s): Conan Reis
AString SkUERemote::get_socket_str()
  {
  if (!is_connected())
    {
    return "SkookumIDE.RemoteConnection(Disconnected)";
    }

  AString str;
  uint8_t ipv4addr[4];
  ISocketSubsystem * socket_system_p = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
  TSharedRef<FInternetAddr> local_addr = socket_system_p->CreateInternetAddr();

  m_socket_p->GetAddress(*local_addr);
  local_addr->GetIp(*reinterpret_cast<uint32 *>(ipv4addr));
  str.ensure_size(32u);
  str.format("%u.%u.%u.%u:%u", ipv4addr[0], ipv4addr[1], ipv4addr[2], ipv4addr[3], m_socket_p->GetPortNo());

  return str;
  }

//---------------------------------------------------------------------------------------
// Gets local host IP address using ini file as a guide
// 
// #Author(s): Conan Reis
TSharedPtr<FInternetAddr> SkUERemote::get_ip_address_local()
  {
  ISocketSubsystem * socket_system_p = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

  bool bind_all_b;
  TSharedPtr<FInternetAddr> local_addr = socket_system_p->GetLocalHostAddr(*GLog, bind_all_b);

  return local_addr;
  }

//---------------------------------------------------------------------------------------
// Determines if runtime connected to remote Skookum IDE
// 
// #Author(s): Conan Reis
bool SkUERemote::is_connected() const
  {
  return this && m_socket_p
    && (m_socket_p->GetConnectionState() == SCS_Connected);
  }

//---------------------------------------------------------------------------------------
// Set remote connection mode.
// 
// #Author(s): Conan Reis
void SkUERemote::set_mode(eSkLocale mode)
  {
  if (m_mode != mode)
    {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Stop old mode
    if (m_socket_p)
      {
      SkDebug::print(a_str_format("SkookumScript: Disconnecting... %s\n", get_socket_str().as_cstr()), SkLocale_local);

      ISocketSubsystem * socket_system_p = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

      if (!m_socket_p->Close())
        {
        SkDebug::print(a_str_format("  error closing socket: %i\n", (int32)socket_system_p->GetLastErrorCode()), SkLocale_local);
        }

      // Free the memory the OS allocated for this socket
      socket_system_p->DestroySocket(m_socket_p);
      m_socket_p = NULL;
      }


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Start new mode

    SkookumRemoteBase::set_mode(mode);

    // $Revisit - CReis Update debug UI of Skookum IDE connection state

    switch (mode)
      {
      case SkLocale_embedded:
        set_connect_state(ConnectState_disconnected);
        SkDebug::print("\nSkookumScript: Skookum IDE not connected (off-line)\n\n", SkLocale_local);
        break;

      case SkLocale_runtime:
        {
        SkDebug::print("SkookumScript: Attempting to connect to remote IDE\n", SkLocale_local);

        set_connect_state(ConnectState_connecting);

        m_socket_p = FTcpSocketBuilder(TEXT("SkookumIDE.RemoteConnection"))
          .AsReusable()
          .AsBlocking();

        bool success = false;

        if (m_socket_p)
          {
          TSharedPtr<FInternetAddr> local_addr = get_ip_address_local();
          local_addr->SetPort(SkUERemote_ide_port);
          success = m_socket_p->Connect(*local_addr);
          }

        if (!success)
          {
          SkDebug::print("\nSkookumScript: Failed attempt to connect with remote IDE.!\n\n", SkLocale_local);
          set_mode(SkLocale_embedded);
          return;
          }

        SkDebug::print(a_str_format("SkookumScript: Connected %s\n", get_socket_str().as_cstr()), SkLocale_local);

        set_connect_state(ConnectState_authenticating);
        break;
        }
      }
    }
  }

//---------------------------------------------------------------------------------------
// Sends a command in datum form to the remote IDE
// 
// #Params
//   datum: command in datum form from a cmd_*() method
//   
// #Modifiers: virtual
// #Author(s): Conan Reis
void SkUERemote::on_cmd_send(const ADatum & datum)
  {
  if (is_connected())
    {
    int32 bytes_sent = 0;

    // $Note - CReis Assumes that Send() is able to transfer entire datum in 1 pass.
    m_socket_p->Send(datum.get_buffer(), datum.get_length(), bytes_sent);

    //if (bytes_sent < int32(datum.get_length()))
    //  {
    //  SkDebug::print(
    //    "SkookumScript: Only part of the data was sent to the remote IDE!!\n",
    //    SkLocale_local,
    //    SkDPrintType_warning);
    //  }
    }
  else
    {
    SkDebug::print(
      "SkookumScript: Remote IDE is not connected - command ignored!\n"
      "[Connect runtime to Skookum IDE and try again.]\n",
      SkLocale_local,
      SkDPrintType_warning);
    }
  }

//---------------------------------------------------------------------------------------
void SkUERemote::on_class_updated(SkClass * class_p)
  {
  // Only care to do anything if editor is present
  if (m_editor_interface_p)
    {
    UClass * uclass_p = SkUEBlueprintInterface::get()->reinitialize_class(class_p);
    if (uclass_p)
      {
      m_editor_interface_p->on_class_updated(class_p, uclass_p);
      }
    }
  }

//---------------------------------------------------------------------------------------
double SkUERemote::get_elapsed_seconds()
  {
  return FPlatformTime::Seconds() - GStartTime;
  }

//---------------------------------------------------------------------------------------
void SkUERemote::wait_for_update()
  {
  FPlatformProcess::Sleep(.1f);
  process_incoming();
  }

//---------------------------------------------------------------------------------------
void SkUERemote::set_editor_interface(ISkookumScriptRuntimeEditorInterface * editor_interface_p)
  {
  m_editor_interface_p = editor_interface_p;
  }

//---------------------------------------------------------------------------------------
bool SkUERemote::spawn_remote_ide()
  {
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Look for Skookum IDE in game/project plug-in folder first.
  FString ide_path(
    FPaths::GamePluginsDir()
    / TEXT("SkookumScript/Runtime/SkookumIDE/SkookumIDE") TEXT(SK_BITS_ID) TEXT(".exe"));

  bool ide_exists = FPaths::FileExists(ide_path);

  if (!ide_exists)
    {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Look for Skookum IDE in engine plug-in folder next.

    // Don't change ide_path yet so the game version stays the default if neither found.
    FString ide_engine_path(
      FPaths::EnginePluginsDir() / TEXT("SkookumScript/Runtime/SkookumIDE/SkookumIDE") TEXT(SK_BITS_ID) TEXT(".exe"));

    ide_exists = FPaths::FileExists(ide_engine_path);

    if (!ide_exists)
      {
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      // Couldn't find IDE
      UE_LOG(LogSkookum, Warning,
        TEXT("Could not run SkookumScript IDE!\n")
        TEXT("Looked in both game/project and engine folders and did not find it:\n")
        TEXT("  %s\n")
        TEXT("  %s\n\n")
        TEXT("Please ensure SkookumScript IDE app is present.\n"),
        *ide_path,
        *ide_engine_path);

      return false;
      }

    ide_path = ide_engine_path;
    }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Found IDE app - now try to run it.
  
  // Path seems to need to be made fully qualified in order to work
  FPaths::MakePathRelativeTo(ide_path, TEXT("/"));

  FPlatformProcess::LaunchFileInDefaultExternalApplication(*ide_path);

  return true;
  }


//---------------------------------------------------------------------------------------
AString SkUERemote::get_project_path()
  {
  // Look for specific SkookumScript project in game/project folder.
  FString game_name(FApp::GetGameName());

  if (game_name.IsEmpty())
    {
    // Use default project
    return AString::ms_empty;
    }

  FString project_path(
    FPaths::GameDir() / TEXT("Scripts/Skookum-project.ini"));

  if (!FPaths::FileExists(project_path))
    {
    // Use default project
    return AString::ms_empty;
    }

  return FStringToAString(FPaths::ConvertRelativePathToFull(project_path));
  }


#endif  // SKOOKUM_REMOTE_UNREAL
