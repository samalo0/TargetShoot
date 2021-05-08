// Copyright Stephen Maloney, 2020

using UnrealBuildTool;
using System.IO;

public class TargetShoot : ModuleRules
{
	public TargetShoot(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "HeadMountedDisplay", "Niagara" });

		if(Target.Platform == UnrealTargetPlatform.Android)
		{
			var manifestFile = Path.Combine(ModuleDirectory, "AndroidSanitizePermissions_UPL.xml");
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", manifestFile);
		}
	}
}
