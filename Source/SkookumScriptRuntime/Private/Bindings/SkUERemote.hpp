//=======================================================================================
// SkookumScript Plugin for Unreal Engine 4
// Copyright (c) 2015 Agog Labs Inc. All rights reserved.
//
// SkookumScript Remote Client
// 
// Author: Conan Reis
//=======================================================================================

#pragma once

//=======================================================================================
// Includes
//=======================================================================================

#include <SkookumScript/SkookumRemoteRuntimeBase.hpp>
#include <Networking.h>


//=======================================================================================
// Global Structures
//=======================================================================================

#ifdef SKOOKUM_REMOTE
  // Enable remote Skookum IDE for debugging in the SkookumScript Unreal plug-in
  #define SKOOKUM_REMOTE_UNREAL
#endif

#ifdef SKOOKUM_REMOTE_UNREAL

//---------------------------------------------------------------------------------------
// Skookum remote IDE communication commands that are specific to the server IDE.
class SkUERemote : public SkookumRemoteRuntimeBase
  {
  public:

  // Common Methods

    SkUERemote();
    ~SkUERemote();

    void                      process_incoming();

    TSharedPtr<FInternetAddr> get_ip_address_local();

    virtual bool              is_connected() const override;
    virtual void              set_mode(eSkLocale mode) override;

    //---------------------------------------------------------------------------------------
    // Determines amount of time elapsed time in seconds (from some consistent start time at
    // or before its first call like: start of system, start of app launch, etc.)
    //
    // Notes:
    //   Used to time system / debug wait intervals.
    //   Should only be used as a difference between an earlier call.
    //   The double has sufficient precision such that it can be used to represent elapsed
    //   milliseconds without error until 285,421 years have passed..
    virtual double get_elapsed_seconds() override;

    //---------------------------------------------------------------------------------------
    // Supply path for current project settings file (specific script overlays, etc. stored
    // in .ini file). If an empty path is returned then the default/core scripts are used
    // without any project specific scripts.
    // 
    // Returns: path to project file
    // 
    // See: SkookumRemoteRuntimeBase::cmd_compiled_state()
    virtual AString get_project_path() override;

    //---------------------------------------------------------------------------------------
    // Brute force spawn of remote IDE. Called when connect attempts don't work and it is
    // assumed that the remote IDE is not running.
    // 
    // Returns:  true if remote IDE started false if not (due to some error, etc.)
    virtual bool spawn_remote_ide() override;

    //---------------------------------------------------------------------------------------
    // Blocking 1/10 second (100 millisecond) wait for current thread/message handle as
    // appropriate for current platform and project.
    //
    // Notes:
    //   Should also update SkookumRemoteRuntimeBase derived objects if they aren't updated on
    //   one or more separate threads. This can be done by calling concurrent process updates
    //   like message handlers or by calling a custom SkookumRemoteRuntimeBase object's update
    //   method directly.
    virtual void wait_for_update() override;

  // Commands

  protected:

    AString                   get_socket_str();

  // Events

    virtual void              on_cmd_send(const ADatum & datum) override;
    virtual void              on_class_updated(SkClass * class_p) override;

  // Data Members

    FSocket *   m_socket_p;

    // Datum that is filled when data is received
    ADatum      m_data_in;

    // Data byte index point - ADef_uint32 when not in progress
    uint32_t    m_data_idx;
  
  };  // SkUERemote

#endif  // SKOOKUM_REMOTE_UNREAL

