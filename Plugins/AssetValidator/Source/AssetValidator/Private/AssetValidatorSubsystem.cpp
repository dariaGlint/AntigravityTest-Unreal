// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetValidatorSubsystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/AssetData.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"

void UAssetValidatorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Create validators
	NamingValidator = NewObject<UAssetNamingValidator>(this);
	FolderValidator = NewObject<UAssetFolderValidator>(this);

	UE_LOG(LogTemp, Log, TEXT("AssetValidatorSubsystem initialized"));
}

void UAssetValidatorSubsystem::Deinitialize()
{
	Super::Deinitialize();

	UE_LOG(LogTemp, Log, TEXT("AssetValidatorSubsystem deinitialized"));
}

FValidationResult UAssetValidatorSubsystem::ValidateAssets(const FString& Path, bool bNamingOnly, bool bIncludeSubfolders)
{
	FValidationResult Result;

	// Get asset registry
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// Setup filter
	FARFilter Filter;
	Filter.PackagePaths.Add(FName(*Path));
	Filter.bRecursivePaths = bIncludeSubfolders;

	// Get all assets
	TArray<FAssetData> AssetDataList;
	AssetRegistry.GetAssets(Filter, AssetDataList);

	Result.TotalAssetsChecked = AssetDataList.Num();

	// Validate each asset
	for (const FAssetData& AssetData : AssetDataList)
	{
		FString AssetPath = AssetData.GetObjectPathString();
		FString AssetClass = GetAssetClassName(AssetData);

		// Validate naming
		if (NamingValidator)
		{
			TArray<FValidationIssue> NamingIssues = NamingValidator->ValidateAssetNaming(AssetPath, AssetClass);
			for (const FValidationIssue& Issue : NamingIssues)
			{
				Result.AddIssue(Issue);
			}
		}

		// Validate folder structure (unless naming only)
		if (!bNamingOnly && FolderValidator)
		{
			TArray<FValidationIssue> FolderIssues = FolderValidator->ValidateFolderStructure(AssetPath, AssetClass);
			for (const FValidationIssue& Issue : FolderIssues)
			{
				Result.AddIssue(Issue);
			}
		}
	}

	return Result;
}

FValidationResult UAssetValidatorSubsystem::ValidateSingleAsset(const FString& AssetPath)
{
	FValidationResult Result;

	// Get asset registry
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// Get asset data
	FAssetData AssetData = AssetRegistry.GetAssetByObjectPath(FSoftObjectPath(AssetPath));
	if (!AssetData.IsValid())
	{
		Result.AddIssue(FValidationIssue(
			AssetPath,
			TEXT("Asset not found in asset registry"),
			EValidationSeverity::Error,
			EValidationType::Other,
			TEXT("")
		));
		return Result;
	}

	Result.TotalAssetsChecked = 1;

	FString AssetClass = GetAssetClassName(AssetData);

	// Validate naming
	if (NamingValidator)
	{
		TArray<FValidationIssue> NamingIssues = NamingValidator->ValidateAssetNaming(AssetPath, AssetClass);
		for (const FValidationIssue& Issue : NamingIssues)
		{
			Result.AddIssue(Issue);
		}
	}

	// Validate folder structure
	if (FolderValidator)
	{
		TArray<FValidationIssue> FolderIssues = FolderValidator->ValidateFolderStructure(AssetPath, AssetClass);
		for (const FValidationIssue& Issue : FolderIssues)
		{
			Result.AddIssue(Issue);
		}
	}

	return Result;
}

FAssetStatistics UAssetValidatorSubsystem::GetAssetStatistics(const FString& Path, bool bIncludeSubfolders)
{
	FAssetStatistics Stats;

	// Get asset registry
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// Setup filter
	FARFilter Filter;
	Filter.PackagePaths.Add(FName(*Path));
	Filter.bRecursivePaths = bIncludeSubfolders;

	// Get all assets
	TArray<FAssetData> AssetDataList;
	AssetRegistry.GetAssets(Filter, AssetDataList);

	Stats.TotalAssets = AssetDataList.Num();

	// Categorize assets
	for (const FAssetData& AssetData : AssetDataList)
	{
		FString AssetClass = GetAssetClassName(AssetData);

		// Count by category
		if (!Stats.AssetsByCategory.Contains(AssetClass))
		{
			Stats.AssetsByCategory.Add(AssetClass, 0);
		}
		Stats.AssetsByCategory[AssetClass]++;

		// Check for unused assets (basic check - no referencers)
		TArray<FName> Referencers;
		AssetRegistry.GetReferencers(AssetData.PackageName, Referencers);
		if (Referencers.Num() == 0)
		{
			Stats.PotentiallyUnusedAssets.Add(AssetData.GetObjectPathString());
		}
	}

	return Stats;
}

void UAssetValidatorSubsystem::PrintValidationResults(const FValidationResult& Result)
{
	UE_LOG(LogTemp, Log, TEXT("=== Asset Validation Results ==="));
	UE_LOG(LogTemp, Log, TEXT("Total Assets Checked: %d"), Result.TotalAssetsChecked);
	UE_LOG(LogTemp, Log, TEXT("Errors: %d"), Result.ErrorCount);
	UE_LOG(LogTemp, Log, TEXT("Warnings: %d"), Result.WarningCount);
	UE_LOG(LogTemp, Log, TEXT("Info: %d"), Result.InfoCount);
	UE_LOG(LogTemp, Log, TEXT(""));

	if (Result.Issues.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("=== Issues Found ==="));

		for (const FValidationIssue& Issue : Result.Issues)
		{
			FString SeverityStr;
			switch (Issue.Severity)
			{
			case EValidationSeverity::Error:
				SeverityStr = TEXT("[ERROR]");
				break;
			case EValidationSeverity::Warning:
				SeverityStr = TEXT("[WARNING]");
				break;
			case EValidationSeverity::Info:
				SeverityStr = TEXT("[INFO]");
				break;
			}

			UE_LOG(LogTemp, Log, TEXT("%s %s: %s"), *SeverityStr, *Issue.AssetPath, *Issue.Message);
			if (!Issue.Suggestion.IsEmpty())
			{
				UE_LOG(LogTemp, Log, TEXT("  Suggestion: %s"), *Issue.Suggestion);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No issues found!"));
	}
}

bool UAssetValidatorSubsystem::ExportValidationResults(const FValidationResult& Result, const FString& FilePath)
{
	FString OutputText;

	OutputText += TEXT("=== Asset Validation Results ===\n");
	OutputText += FString::Printf(TEXT("Total Assets Checked: %d\n"), Result.TotalAssetsChecked);
	OutputText += FString::Printf(TEXT("Errors: %d\n"), Result.ErrorCount);
	OutputText += FString::Printf(TEXT("Warnings: %d\n"), Result.WarningCount);
	OutputText += FString::Printf(TEXT("Info: %d\n\n"), Result.InfoCount);

	if (Result.Issues.Num() > 0)
	{
		OutputText += TEXT("=== Issues Found ===\n\n");

		for (const FValidationIssue& Issue : Result.Issues)
		{
			FString SeverityStr;
			switch (Issue.Severity)
			{
			case EValidationSeverity::Error:
				SeverityStr = TEXT("[ERROR]");
				break;
			case EValidationSeverity::Warning:
				SeverityStr = TEXT("[WARNING]");
				break;
			case EValidationSeverity::Info:
				SeverityStr = TEXT("[INFO]");
				break;
			}

			OutputText += FString::Printf(TEXT("%s %s\n"), *SeverityStr, *Issue.AssetPath);
			OutputText += FString::Printf(TEXT("  Message: %s\n"), *Issue.Message);
			if (!Issue.Suggestion.IsEmpty())
			{
				OutputText += FString::Printf(TEXT("  Suggestion: %s\n"), *Issue.Suggestion);
			}
			OutputText += TEXT("\n");
		}
	}
	else
	{
		OutputText += TEXT("No issues found!\n");
	}

	// Save to file
	return FFileHelper::SaveStringToFile(OutputText, *FilePath);
}

FString UAssetValidatorSubsystem::GetAssetClassName(const FAssetData& AssetData) const
{
	return AssetData.AssetClassPath.GetAssetName().ToString();
}
