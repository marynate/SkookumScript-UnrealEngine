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

      bFasterWithoutUnity = true; // SSUEBindings.cpp takes a long time to compile due to auto-generated engine bindings

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
            "InputCore",
            "Sockets",
            "Networking",
            "AgogCore",
            "SkookumScript"
          }
        );

      if (UEBuildConfiguration.bBuildEditor == true)
      {
        PublicDependencyModuleNames.Add("UnrealEd");
      }

      // ... add private dependencies that you statically link with here ...
      //PrivateDependencyModuleNames.AddRange(new string[] {});

      // Add any modules that your module loads dynamically here ...
      //DynamicallyLoadedModuleNames.AddRange(new string[] {});

      //if (!UnrealBuildTool.BuildingRocket())
      //{
      //}
    }
  }
}