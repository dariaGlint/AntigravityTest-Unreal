// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetNamingValidator.h"

UAssetNamingValidator::UAssetNamingValidator()
{
	InitializeDefaultRules();

	// Forbidden characters
	ForbiddenCharacters = {
		TEXT(" "),  // Spaces
		TEXT("!"),
		TEXT("@"),
		TEXT("#"),
		TEXT("$"),
		TEXT("%"),
		TEXT("^"),
		TEXT("&"),
		TEXT("*"),
		TEXT("("),
		TEXT(")"),
		TEXT("+"),
		TEXT("="),
		TEXT("{"),
		TEXT("}"),
		TEXT("["),
		TEXT("]"),
		TEXT("|"),
		TEXT("\\"),
		TEXT(":"),
		TEXT(";"),
		TEXT("\""),
		TEXT("'"),
		TEXT("<"),
		TEXT(">"),
		TEXT(","),
		TEXT("?"),
		TEXT("/")
	};
}

void UAssetNamingValidator::InitializeDefaultRules()
{
	// Textures
	NamingRules.Add(FNamingRule(TEXT("Texture2D"), TEXT("T_"), TEXT(""), TEXT("Textures should start with T_")));
	NamingRules.Add(FNamingRule(TEXT("TextureCube"), TEXT("TC_"), TEXT(""), TEXT("Texture cubes should start with TC_")));
	NamingRules.Add(FNamingRule(TEXT("TextureRenderTarget2D"), TEXT("RT_"), TEXT(""), TEXT("Render targets should start with RT_")));

	// Materials
	NamingRules.Add(FNamingRule(TEXT("Material"), TEXT("M_"), TEXT(""), TEXT("Materials should start with M_")));
	NamingRules.Add(FNamingRule(TEXT("MaterialInstance"), TEXT("MI_"), TEXT(""), TEXT("Material instances should start with MI_")));
	NamingRules.Add(FNamingRule(TEXT("MaterialInstanceConstant"), TEXT("MI_"), TEXT(""), TEXT("Material instance constants should start with MI_")));
	NamingRules.Add(FNamingRule(TEXT("MaterialFunction"), TEXT("MF_"), TEXT(""), TEXT("Material functions should start with MF_")));
	NamingRules.Add(FNamingRule(TEXT("MaterialParameterCollection"), TEXT("MPC_"), TEXT(""), TEXT("Material parameter collections should start with MPC_")));

	// Meshes
	NamingRules.Add(FNamingRule(TEXT("StaticMesh"), TEXT("SM_"), TEXT(""), TEXT("Static meshes should start with SM_")));
	NamingRules.Add(FNamingRule(TEXT("SkeletalMesh"), TEXT("SK_"), TEXT(""), TEXT("Skeletal meshes should start with SK_")));

	// Animations
	NamingRules.Add(FNamingRule(TEXT("AnimSequence"), TEXT("A_"), TEXT(""), TEXT("Animation sequences should start with A_")));
	NamingRules.Add(FNamingRule(TEXT("AnimMontage"), TEXT("AM_"), TEXT(""), TEXT("Animation montages should start with AM_")));
	NamingRules.Add(FNamingRule(TEXT("AnimBlueprint"), TEXT("ABP_"), TEXT(""), TEXT("Animation blueprints should start with ABP_")));
	NamingRules.Add(FNamingRule(TEXT("BlendSpace"), TEXT("BS_"), TEXT(""), TEXT("Blend spaces should start with BS_")));
	NamingRules.Add(FNamingRule(TEXT("BlendSpace1D"), TEXT("BS_"), TEXT(""), TEXT("1D Blend spaces should start with BS_")));

	// Blueprints
	NamingRules.Add(FNamingRule(TEXT("Blueprint"), TEXT("BP_"), TEXT(""), TEXT("Blueprints should start with BP_")));

	// Particles & VFX
	NamingRules.Add(FNamingRule(TEXT("ParticleSystem"), TEXT("PS_"), TEXT(""), TEXT("Particle systems should start with PS_")));
	NamingRules.Add(FNamingRule(TEXT("NiagaraSystem"), TEXT("NS_"), TEXT(""), TEXT("Niagara systems should start with NS_")));

	// Audio
	NamingRules.Add(FNamingRule(TEXT("SoundWave"), TEXT("SW_"), TEXT(""), TEXT("Sound waves should start with SW_")));
	NamingRules.Add(FNamingRule(TEXT("SoundCue"), TEXT("SC_"), TEXT(""), TEXT("Sound cues should start with SC_")));

	// UI
	NamingRules.Add(FNamingRule(TEXT("WidgetBlueprint"), TEXT("WBP_"), TEXT(""), TEXT("Widget blueprints should start with WBP_")));

	// Physics
	NamingRules.Add(FNamingRule(TEXT("PhysicalMaterial"), TEXT("PM_"), TEXT(""), TEXT("Physical materials should start with PM_")));
	NamingRules.Add(FNamingRule(TEXT("PhysicsAsset"), TEXT("PA_"), TEXT(""), TEXT("Physics assets should start with PA_")));

	// Data
	NamingRules.Add(FNamingRule(TEXT("DataTable"), TEXT("DT_"), TEXT(""), TEXT("Data tables should start with DT_")));
	NamingRules.Add(FNamingRule(TEXT("CurveTable"), TEXT("CT_"), TEXT(""), TEXT("Curve tables should start with CT_")));

	// Input (Enhanced Input System)
	NamingRules.Add(FNamingRule(TEXT("InputAction"), TEXT("IA_"), TEXT(""), TEXT("Input actions should start with IA_")));
	NamingRules.Add(FNamingRule(TEXT("InputMappingContext"), TEXT("IMC_"), TEXT(""), TEXT("Input mapping contexts should start with IMC_")));
}

TArray<FValidationIssue> UAssetNamingValidator::ValidateAssetNaming(const FString& AssetPath, const FString& AssetClass)
{
	TArray<FValidationIssue> Issues;

	// Extract asset name from path
	FString AssetName;
	if (AssetPath.Contains(TEXT("/")))
	{
		int32 LastSlashIndex;
		AssetPath.FindLastChar(TEXT('/'), LastSlashIndex);
		AssetName = AssetPath.RightChop(LastSlashIndex + 1);
	}
	else
	{
		AssetName = AssetPath;
	}

	// Check for forbidden characters
	if (ContainsForbiddenCharacters(AssetName))
	{
		FString ForbiddenCharsFound;
		for (const FString& ForbiddenChar : ForbiddenCharacters)
		{
			if (AssetName.Contains(ForbiddenChar))
			{
				if (!ForbiddenCharsFound.IsEmpty())
				{
					ForbiddenCharsFound += TEXT(", ");
				}
				if (ForbiddenChar == TEXT(" "))
				{
					ForbiddenCharsFound += TEXT("(space)");
				}
				else
				{
					ForbiddenCharsFound += ForbiddenChar;
				}
			}
		}

		Issues.Add(FValidationIssue(
			AssetPath,
			FString::Printf(TEXT("Asset name contains forbidden characters: %s"), *ForbiddenCharsFound),
			EValidationSeverity::Error,
			EValidationType::NamingConvention,
			TEXT("Remove special characters and spaces from asset name")
		));
	}

	// Check naming convention based on asset class
	FString ExpectedPrefix = GetExpectedPrefix(AssetClass);
	if (!ExpectedPrefix.IsEmpty())
	{
		if (!AssetName.StartsWith(ExpectedPrefix))
		{
			// Find the rule for better description
			FString RuleDescription;
			for (const FNamingRule& Rule : NamingRules)
			{
				if (Rule.AssetClass == AssetClass)
				{
					RuleDescription = Rule.Description;
					break;
				}
			}

			Issues.Add(FValidationIssue(
				AssetPath,
				FString::Printf(TEXT("Asset does not follow naming convention. Expected prefix: %s"), *ExpectedPrefix),
				EValidationSeverity::Warning,
				EValidationType::NamingConvention,
				FString::Printf(TEXT("Rename to: %s%s"), *ExpectedPrefix, *AssetName)
			));
		}
	}

	return Issues;
}

bool UAssetNamingValidator::ContainsForbiddenCharacters(const FString& AssetName)
{
	for (const FString& ForbiddenChar : ForbiddenCharacters)
	{
		if (AssetName.Contains(ForbiddenChar))
		{
			return true;
		}
	}
	return false;
}

FString UAssetNamingValidator::GetExpectedPrefix(const FString& AssetClass) const
{
	for (const FNamingRule& Rule : NamingRules)
	{
		if (Rule.AssetClass == AssetClass)
		{
			return Rule.RequiredPrefix;
		}
	}
	return FString();
}

void UAssetNamingValidator::AddNamingRule(const FNamingRule& Rule)
{
	// Check if rule already exists for this class
	for (int32 i = 0; i < NamingRules.Num(); ++i)
	{
		if (NamingRules[i].AssetClass == Rule.AssetClass)
		{
			// Update existing rule
			NamingRules[i] = Rule;
			return;
		}
	}

	// Add new rule
	NamingRules.Add(Rule);
}
