// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetFolderValidator.h"

UAssetFolderValidator::UAssetFolderValidator()
{
	InitializeDefaultRules();
}

void UAssetFolderValidator::InitializeDefaultRules()
{
	// Textures
	FolderRules.Add(FFolderRule(
		TEXT("Texture2D"),
		{TEXT("Textures"), TEXT("Materials")},
		TEXT("Textures should be in Textures or Materials folder")
	));

	// Materials
	FolderRules.Add(FFolderRule(
		TEXT("Material"),
		{TEXT("Materials")},
		TEXT("Materials should be in Materials folder")
	));
	FolderRules.Add(FFolderRule(
		TEXT("MaterialInstance"),
		{TEXT("Materials")},
		TEXT("Material instances should be in Materials folder")
	));
	FolderRules.Add(FFolderRule(
		TEXT("MaterialInstanceConstant"),
		{TEXT("Materials")},
		TEXT("Material instance constants should be in Materials folder")
	));

	// Meshes
	FolderRules.Add(FFolderRule(
		TEXT("StaticMesh"),
		{TEXT("Meshes"), TEXT("StaticMeshes")},
		TEXT("Static meshes should be in Meshes or StaticMeshes folder")
	));
	FolderRules.Add(FFolderRule(
		TEXT("SkeletalMesh"),
		{TEXT("Meshes"), TEXT("SkeletalMeshes")},
		TEXT("Skeletal meshes should be in Meshes or SkeletalMeshes folder")
	));

	// Animations
	FolderRules.Add(FFolderRule(
		TEXT("AnimSequence"),
		{TEXT("Animations"), TEXT("Anims")},
		TEXT("Animation sequences should be in Animations or Anims folder")
	));
	FolderRules.Add(FFolderRule(
		TEXT("AnimMontage"),
		{TEXT("Animations"), TEXT("Anims")},
		TEXT("Animation montages should be in Animations or Anims folder")
	));
	FolderRules.Add(FFolderRule(
		TEXT("AnimBlueprint"),
		{TEXT("Animations"), TEXT("Anims"), TEXT("Blueprints")},
		TEXT("Animation blueprints should be in Animations, Anims, or Blueprints folder")
	));

	// Blueprints
	FolderRules.Add(FFolderRule(
		TEXT("Blueprint"),
		{TEXT("Blueprints")},
		TEXT("Blueprints should be in Blueprints folder")
	));

	// UI
	FolderRules.Add(FFolderRule(
		TEXT("WidgetBlueprint"),
		{TEXT("UI"), TEXT("Widgets")},
		TEXT("Widget blueprints should be in UI or Widgets folder")
	));

	// Audio
	FolderRules.Add(FFolderRule(
		TEXT("SoundWave"),
		{TEXT("Audio"), TEXT("Sounds")},
		TEXT("Sound waves should be in Audio or Sounds folder")
	));
	FolderRules.Add(FFolderRule(
		TEXT("SoundCue"),
		{TEXT("Audio"), TEXT("Sounds")},
		TEXT("Sound cues should be in Audio or Sounds folder")
	));

	// VFX
	FolderRules.Add(FFolderRule(
		TEXT("ParticleSystem"),
		{TEXT("VFX"), TEXT("Particles")},
		TEXT("Particle systems should be in VFX or Particles folder")
	));
	FolderRules.Add(FFolderRule(
		TEXT("NiagaraSystem"),
		{TEXT("VFX"), TEXT("Niagara")},
		TEXT("Niagara systems should be in VFX or Niagara folder")
	));

	// Input
	FolderRules.Add(FFolderRule(
		TEXT("InputAction"),
		{TEXT("Input"), TEXT("Input/Actions")},
		TEXT("Input actions should be in Input or Input/Actions folder")
	));
	FolderRules.Add(FFolderRule(
		TEXT("InputMappingContext"),
		{TEXT("Input")},
		TEXT("Input mapping contexts should be in Input folder")
	));

	// Data
	FolderRules.Add(FFolderRule(
		TEXT("DataTable"),
		{TEXT("Data"), TEXT("DataTables")},
		TEXT("Data tables should be in Data or DataTables folder")
	));
}

TArray<FValidationIssue> UAssetFolderValidator::ValidateFolderStructure(const FString& AssetPath, const FString& AssetClass)
{
	TArray<FValidationIssue> Issues;

	// Check if asset is in a recommended folder
	if (!IsInRecommendedFolder(AssetPath, AssetClass))
	{
		TArray<FString> RecommendedFolders = GetRecommendedFolders(AssetClass);
		if (RecommendedFolders.Num() > 0)
		{
			FString FolderSuggestion;
			for (int32 i = 0; i < RecommendedFolders.Num(); ++i)
			{
				if (i > 0)
				{
					FolderSuggestion += TEXT(", ");
				}
				FolderSuggestion += RecommendedFolders[i];
			}

			Issues.Add(FValidationIssue(
				AssetPath,
				FString::Printf(TEXT("Asset is not in a recommended folder for its type (%s)"), *AssetClass),
				EValidationSeverity::Info,
				EValidationType::FolderStructure,
				FString::Printf(TEXT("Consider moving to: %s"), *FolderSuggestion)
			));
		}
	}

	return Issues;
}

bool UAssetFolderValidator::IsInRecommendedFolder(const FString& AssetPath, const FString& AssetClass) const
{
	TArray<FString> RecommendedFolders = GetRecommendedFolders(AssetClass);
	if (RecommendedFolders.Num() == 0)
	{
		// No rules for this asset type, consider it valid
		return true;
	}

	// Check if asset path contains any of the recommended folders
	for (const FString& Folder : RecommendedFolders)
	{
		// Match folder name in path (case insensitive)
		if (AssetPath.Contains(TEXT("/") + Folder + TEXT("/"), ESearchCase::IgnoreCase) ||
		    AssetPath.EndsWith(TEXT("/") + Folder, ESearchCase::IgnoreCase))
		{
			return true;
		}
	}

	return false;
}

TArray<FString> UAssetFolderValidator::GetRecommendedFolders(const FString& AssetClass) const
{
	for (const FFolderRule& Rule : FolderRules)
	{
		if (Rule.AssetClass == AssetClass)
		{
			return Rule.RecommendedFolders;
		}
	}
	return TArray<FString>();
}

void UAssetFolderValidator::AddFolderRule(const FFolderRule& Rule)
{
	// Check if rule already exists for this class
	for (int32 i = 0; i < FolderRules.Num(); ++i)
	{
		if (FolderRules[i].AssetClass == Rule.AssetClass)
		{
			// Update existing rule
			FolderRules[i] = Rule;
			return;
		}
	}

	// Add new rule
	FolderRules.Add(Rule);
}
