// Copyright 2000 Agog Labs Inc., All Rights Reserved.
using System.IO;
using UnrealBuildTool;


public class AgogCore : ModuleRules
{
  public AgogCore(TargetInfo Target)
  {  
    Type = ModuleType.External;
    
    var bPlatformAllowed = false;
    
    string platPathSuffix = Target.Platform.ToString();
    
    switch (Target.Platform)
    {
    case UnrealTargetPlatform.Win32:
      bPlatformAllowed = true;
      platPathSuffix = Path.Combine("Win32", "VS2013");
      Definitions.Add("WIN32_LEAN_AND_MEAN");
      break;
    case UnrealTargetPlatform.Win64:
      bPlatformAllowed = true;
      platPathSuffix = Path.Combine("Win64", "VS2013");
      Definitions.Add("WIN32_LEAN_AND_MEAN");
      break;
    }

    string libNameSuffix = "";
    
    switch (Target.Configuration)
    {
    case UnrealTargetConfiguration.Debug:
    case UnrealTargetConfiguration.DebugGame:
      Definitions.Add("A_EXTRA_CHECK=1");
      Definitions.Add("A_UNOPTIMIZED=1");
      Definitions.Add("SKOOKUM=31");
      
      if (BuildConfiguration.bDebugBuildsActuallyUseDebugCRT)
      {
        // Use Debug C run-time
        libNameSuffix = "-Debug.lib";
      }
      else
      {
        // Use Optimized C run-time
        libNameSuffix = "-DebugCRTOpt.lib";
      }

      break;

    case UnrealTargetConfiguration.Development:
    case UnrealTargetConfiguration.Test:
      Definitions.Add("A_EXTRA_CHECK=1");
      Definitions.Add("SKOOKUM=31");
      libNameSuffix = "-Development.lib";
      break;

    case UnrealTargetConfiguration.Shipping:
      Definitions.Add("SKOOKUM=8");
      libNameSuffix = "-Shipping.lib";
      break;
    }
    
    // Determine path to directory containing this Build.cs file
    var modulePath = Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name));
    
    PublicIncludePaths.Add(Path.Combine(modulePath, "Public"));

    if (bPlatformAllowed)
    {
      var libPath = Path.Combine(modulePath, "Lib", platPathSuffix);
      
      Log.TraceVerbose("AgogCore library added to path: {0}", libPath);
      
      PublicLibraryPaths.Add(libPath);
      PublicAdditionalLibraries.Add(Path.Combine(libPath, "AgogCore" + libNameSuffix));
    }
  }    
}
