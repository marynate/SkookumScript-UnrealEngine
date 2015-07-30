// Copyright 2015 Agog Labs, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class SkookumScriptEditor : ModuleRules
	{
		public SkookumScriptEditor(TargetInfo Target)
		{
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
      
			PublicIncludePaths.AddRange(
				new string[] {					
					//"Programs/UnrealHeaderTool/Public",
					// ... add other public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"UnrealEd",
					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"InputCore",
					"AssetTools",
					"SkookumScriptRuntime",
					"ClassViewer",
					"KismetCompiler",
					"Kismet",
					"BlueprintGraph",
                    "AgogCore",
                    "SkookumScript"
					// ... add private dependencies that you statically link with here ...
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
				);
		}
	}
}