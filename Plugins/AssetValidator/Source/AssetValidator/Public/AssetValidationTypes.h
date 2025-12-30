// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetValidationTypes.generated.h"

/** Severity level of validation issues */
UENUM(BlueprintType)
enum class EValidationSeverity : uint8
{
	Info UMETA(DisplayName = "Info"),
	Warning UMETA(DisplayName = "Warning"),
	Error UMETA(DisplayName = "Error")
};

/** Type of validation issue */
UENUM(BlueprintType)
enum class EValidationType : uint8
{
	NamingConvention UMETA(DisplayName = "Naming Convention"),
	FolderStructure UMETA(DisplayName = "Folder Structure"),
	AssetReference UMETA(DisplayName = "Asset Reference"),
	Performance UMETA(DisplayName = "Performance"),
	Other UMETA(DisplayName = "Other")
};

/** Single validation issue */
USTRUCT(BlueprintType)
struct FValidationIssue
{
	GENERATED_BODY()

	/** Path to the asset with the issue */
	UPROPERTY(BlueprintReadOnly, Category = "Validation")
	FString AssetPath;

	/** Description of the issue */
	UPROPERTY(BlueprintReadOnly, Category = "Validation")
	FString Message;

	/** Severity of the issue */
	UPROPERTY(BlueprintReadOnly, Category = "Validation")
	EValidationSeverity Severity = EValidationSeverity::Warning;

	/** Type of validation that found this issue */
	UPROPERTY(BlueprintReadOnly, Category = "Validation")
	EValidationType ValidationType = EValidationType::Other;

	/** Optional suggestion for fixing the issue */
	UPROPERTY(BlueprintReadOnly, Category = "Validation")
	FString Suggestion;

	FValidationIssue() = default;

	FValidationIssue(const FString& InAssetPath, const FString& InMessage,
	                 EValidationSeverity InSeverity, EValidationType InType,
	                 const FString& InSuggestion = FString())
		: AssetPath(InAssetPath)
		, Message(InMessage)
		, Severity(InSeverity)
		, ValidationType(InType)
		, Suggestion(InSuggestion)
	{
	}
};

/** Results of validation */
USTRUCT(BlueprintType)
struct FValidationResult
{
	GENERATED_BODY()

	/** List of all issues found */
	UPROPERTY(BlueprintReadOnly, Category = "Validation")
	TArray<FValidationIssue> Issues;

	/** Total number of assets checked */
	UPROPERTY(BlueprintReadOnly, Category = "Validation")
	int32 TotalAssetsChecked = 0;

	/** Number of errors found */
	UPROPERTY(BlueprintReadOnly, Category = "Validation")
	int32 ErrorCount = 0;

	/** Number of warnings found */
	UPROPERTY(BlueprintReadOnly, Category = "Validation")
	int32 WarningCount = 0;

	/** Number of info messages */
	UPROPERTY(BlueprintReadOnly, Category = "Validation")
	int32 InfoCount = 0;

	/** Add an issue and update counters */
	void AddIssue(const FValidationIssue& Issue)
	{
		Issues.Add(Issue);

		switch (Issue.Severity)
		{
		case EValidationSeverity::Error:
			ErrorCount++;
			break;
		case EValidationSeverity::Warning:
			WarningCount++;
			break;
		case EValidationSeverity::Info:
			InfoCount++;
			break;
		}
	}
};
