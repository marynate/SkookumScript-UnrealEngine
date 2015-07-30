// Copyright (c) 2015 Agog Labs, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
  public class SkookumScriptRuntime : ModuleRules
  {
    public SkookumScriptRuntime(TargetInfo Target)
    {
      //PCHUsage = PCHUsageMode.NoSharedPCHs;
      //MinFilesUsingPrecompiledHeaderOverride = 0;
      //Target.bForceIncludePrecompiledHeader = true;
      //string AdditionalArguments = "/Y-";
      //BuildConfiguration.bUsePCHFiles = false;
      //BuildConfiguration.bUseSharedPCHs = false;
      //OutCPPEnvironmentConfiguration.PrecompiledHeaderAction = PrecompiledHeaderAction.None;
      //OutCPPEnvironmentConfiguration.bForceIncludePrecompiledHeader = false;
      //SharedPCHHeaderFile = "../Plugins/Script/SkookumScript/Source/SkookumScriptRuntime/Private/AgogCore/AgogCore.hpp";

      bFasterWithoutUnity = false; // SSUEBindings.cpp takes a long time to compile due to auto-generated engine bindings

      // NOTE: All modules inside the SkookumScript plugin folder must use the exact same definitions!
      switch (Target.Configuration)
      {
      case UnrealTargetConfiguration.Debug:
      case UnrealTargetConfiguration.DebugGame:
        Definitions.Add("A_EXTRA_CHECK=1");
        Definitions.Add("A_UNOPTIMIZED=1");
        Definitions.Add("SKOOKUM=31");
        break;

      case UnrealTargetConfiguration.Development:
      case UnrealTargetConfiguration.Test:
        Definitions.Add("A_EXTRA_CHECK=1");
        Definitions.Add("SKOOKUM=31");
        break;

      case UnrealTargetConfiguration.Shipping:
        Definitions.Add("A_SYMBOL_STR_DB=1");
        Definitions.Add("A_NO_SYMBOL_REF_LINK=1");
        Definitions.Add("SKOOKUM=8");
        break;
      }
      
      // Add public include paths required here ...
      //PublicIncludePaths.AddRange(
      //  new string[] {          
      //    //"Programs/UnrealHeaderTool/Public",
      //    }
      //  );

      PrivateIncludePaths.Add("SkookumScriptRuntime/Private");

      // Add public dependencies that you statically link with here ...
      PublicDependencyModuleNames.AddRange(
        new string[]
          {
            "Core",
            "CoreUObject",
            "Engine",
          }
        );

      if (UEBuildConfiguration.bBuildEditor == true)
      {
        PublicDependencyModuleNames.Add("UnrealEd");
      }

      // ... add private dependencies that you statically link with here ...
      PrivateDependencyModuleNames.AddRange(
        new string[]
          {
            "InputCore",
            "Sockets",
            "Networking",
            "SlateCore",
            "Slate",
            "AgogCore",
            "SkookumScript"
          }
        );

      // Add any modules that your module loads dynamically here ...
      //DynamicallyLoadedModuleNames.AddRange(new string[] {});
    }
  }
}