// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetValidationTypes.h"
#include "UObject/Object.h"
#include "AssetNamingValidator.generated.h"

/** Asset naming convention rules */
USTRUCT(BlueprintType)
struct FNamingRule
{
	GENERATED_BODY()

	/** Asset class name (e.g., "Texture2D", "Material") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Naming")
	FString AssetClass;

	/** Required prefix (e.g., "T_", "M_") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Naming")
	FString RequiredPrefix;

	/** Optional suffix */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Naming")
	FString OptionalSuffix;

	/** Description of the rule */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Naming")
	FString Description;

	FNamingRule() = default;

	FNamingRule(const FString& InClass, const FString& InPrefix, const FString& InSuffix = FString(),
	            const FString& InDesc = FString())
		: AssetClass(InClass)
		, RequiredPrefix(InPrefix)
		, OptionalSuffix(InSuffix)
		, Description(InDesc)
	{
	}
};

/**
 * Validates asset naming conventions
 */
UCLASS(BlueprintType)
class ASSETVALIDATOR_API UAssetNamingValidator : public UObject
{
	GENERATED_BODY()

public:
	UAssetNamingValidator();

	/**
	 * Validate naming convention for a single asset path
	 * @param AssetPath - Path to the asset
	 * @param AssetClass - Class name of the asset
	 * @return Validation issue if found, empty if valid
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	TArray<FValidationIssue> ValidateAssetNaming(const FString& AssetPath, const FString& AssetClass);

	/**
	 * Check if asset name contains forbidden characters
	 * @param AssetName - Name to check
	 * @return True if contains forbidden characters
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	bool ContainsForbiddenCharacters(const FString& AssetName);

	/**
	 * Get the expected prefix for an asset class
	 * @param AssetClass - Class name
	 * @return Expected prefix or empty string if no rule exists
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	FString GetExpectedPrefix(const FString& AssetClass) const;

	/**
	 * Add a custom naming rule
	 * @param Rule - Naming rule to add
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	void AddNamingRule(const FNamingRule& Rule);

	/**
	 * Get all naming rules
	 * @return Array of all naming rules
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Validation")
	TArray<FNamingRule> GetNamingRules() const { return NamingRules; }

protected:
	/** Initialize default naming rules */
	void InitializeDefaultRules();

	/** Naming rules mapped by asset class */
	UPROPERTY()
	TArray<FNamingRule> NamingRules;

	/** Forbidden characters in asset names */
	UPROPERTY()
	TArray<FString> ForbiddenCharacters;
};
