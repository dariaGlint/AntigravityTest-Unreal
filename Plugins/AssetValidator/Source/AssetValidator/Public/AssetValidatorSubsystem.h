// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "AssetValidationTypes.h"
#include "AssetNamingValidator.h"
#include "AssetFolderValidator.h"
#include "AssetValidatorSubsystem.generated.h"

/** Statistics about assets in the project */
USTRUCT(BlueprintType)
struct FAssetStatistics
{
	GENERATED_BODY()

	/** Total number of assets */
	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	int32 TotalAssets = 0;

	/** Assets by category */
	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	TMap<FString, int32> AssetsByCategory;

	/** Total size in bytes */
	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	int64 TotalSizeBytes = 0;

	/** Potentially unused assets (no references) */
	UPROPERTY(BlueprintReadOnly, Category = "Statistics")
	TArray<FString> PotentiallyUnusedAssets;
};

/**
 * Editor subsystem for validating project assets
 */
UCLASS()
class ASSETVALIDATOR_API UAssetValidatorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Validate all assets in a specific path
	 * @param Path - Content path to validate (e.g., "/Game/")
	 * @param bNamingOnly - Only check naming conventions
	 * @param bIncludeSubfolders - Include subfolders in validation
	 * @return Validation result with all issues found
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	FValidationResult ValidateAssets(const FString& Path = TEXT("/Game/"), bool bNamingOnly = false, bool bIncludeSubfolders = true);

	/**
	 * Validate a single asset
	 * @param AssetPath - Path to the asset
	 * @return Validation result
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	FValidationResult ValidateSingleAsset(const FString& AssetPath);

	/**
	 * Get statistics about assets in the project
	 * @param Path - Content path to analyze
	 * @param bIncludeSubfolders - Include subfolders
	 * @return Asset statistics
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	FAssetStatistics GetAssetStatistics(const FString& Path = TEXT("/Game/"), bool bIncludeSubfolders = true);

	/**
	 * Print validation results to log
	 * @param Result - Validation result to print
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	void PrintValidationResults(const FValidationResult& Result);

	/**
	 * Export validation results to a text file
	 * @param Result - Validation result to export
	 * @param FilePath - Path where to save the file
	 * @return True if export successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	bool ExportValidationResults(const FValidationResult& Result, const FString& FilePath);

	/**
	 * Get the naming validator
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	UAssetNamingValidator* GetNamingValidator() const { return NamingValidator; }

	/**
	 * Get the folder validator
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	UAssetFolderValidator* GetFolderValidator() const { return FolderValidator; }

protected:
	/** Naming convention validator */
	UPROPERTY()
	TObjectPtr<UAssetNamingValidator> NamingValidator;

	/** Folder structure validator */
	UPROPERTY()
	TObjectPtr<UAssetFolderValidator> FolderValidator;

	/** Get asset class name from asset data */
	FString GetAssetClassName(const struct FAssetData& AssetData) const;
};
