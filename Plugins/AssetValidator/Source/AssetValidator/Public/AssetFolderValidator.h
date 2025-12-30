// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetValidationTypes.h"
#include "UObject/Object.h"
#include "AssetFolderValidator.generated.h"

/** Recommended folder for specific asset types */
USTRUCT(BlueprintType)
struct FFolderRule
{
	GENERATED_BODY()

	/** Asset class name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Folder")
	FString AssetClass;

	/** Recommended folder paths (relative to Content) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Folder")
	TArray<FString> RecommendedFolders;

	/** Description of the rule */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Folder")
	FString Description;

	FFolderRule() = default;

	FFolderRule(const FString& InClass, const TArray<FString>& InFolders, const FString& InDesc = FString())
		: AssetClass(InClass)
		, RecommendedFolders(InFolders)
		, Description(InDesc)
	{
	}
};

/**
 * Validates asset folder structure
 */
UCLASS(BlueprintType)
class ASSETVALIDATOR_API UAssetFolderValidator : public UObject
{
	GENERATED_BODY()

public:
	UAssetFolderValidator();

	/**
	 * Validate folder structure for an asset
	 * @param AssetPath - Full path to the asset
	 * @param AssetClass - Class name of the asset
	 * @return Validation issues if found
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	TArray<FValidationIssue> ValidateFolderStructure(const FString& AssetPath, const FString& AssetClass);

	/**
	 * Check if asset is in a recommended folder
	 * @param AssetPath - Full path to the asset
	 * @param AssetClass - Class name of the asset
	 * @return True if in recommended folder
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	bool IsInRecommendedFolder(const FString& AssetPath, const FString& AssetClass) const;

	/**
	 * Get recommended folders for an asset class
	 * @param AssetClass - Class name
	 * @return Array of recommended folder paths
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	TArray<FString> GetRecommendedFolders(const FString& AssetClass) const;

	/**
	 * Add a custom folder rule
	 * @param Rule - Folder rule to add
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	void AddFolderRule(const FFolderRule& Rule);

	/**
	 * Get all folder rules
	 * @return Array of all folder rules
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	TArray<FFolderRule> GetFolderRules() const { return FolderRules; }

protected:
	/** Initialize default folder rules */
	void InitializeDefaultRules();

	/** Folder rules mapped by asset class */
	UPROPERTY()
	TArray<FFolderRule> FolderRules;
};
