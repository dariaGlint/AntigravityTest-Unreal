#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LocalizationTextExtractor.generated.h"

/**
 * Structure to hold extracted localization text information
 */
USTRUCT(BlueprintType)
struct FExtractedTextEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Localization")
	FString Key;

	UPROPERTY(BlueprintReadWrite, Category = "Localization")
	FString SourceString;

	UPROPERTY(BlueprintReadWrite, Category = "Localization")
	FString Context;

	UPROPERTY(BlueprintReadWrite, Category = "Localization")
	FString Location;

	FExtractedTextEntry()
		: Key(TEXT(""))
		, SourceString(TEXT(""))
		, Context(TEXT(""))
		, Location(TEXT(""))
	{}
};

/**
 * Text Extractor for localization workflow
 * Extracts text from Blueprints, C++ code, and data assets
 */
UCLASS(BlueprintType)
class LOCALIZATIONHELPER_API ULocalizationTextExtractor : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Extract text from Blueprint assets
	 * @param BlueprintPath Path to the Blueprint asset
	 * @return Array of extracted text entries
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|Extraction")
	static TArray<FExtractedTextEntry> ExtractFromBlueprint(const FString& BlueprintPath);

	/**
	 * Extract text from C++ source files
	 * @param SourceFilePath Path to the C++ source file
	 * @return Array of extracted text entries
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|Extraction")
	static TArray<FExtractedTextEntry> ExtractFromCppSource(const FString& SourceFilePath);

	/**
	 * Extract text from data assets
	 * @param AssetPath Path to the data asset
	 * @return Array of extracted text entries
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|Extraction")
	static TArray<FExtractedTextEntry> ExtractFromDataAsset(const FString& AssetPath);

	/**
	 * Extract all text from a project directory
	 * @param ProjectPath Path to the project root
	 * @return Array of all extracted text entries
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|Extraction")
	static TArray<FExtractedTextEntry> ExtractAllTexts(const FString& ProjectPath);

	/**
	 * Export extracted texts to CSV format
	 * @param Entries Array of extracted text entries
	 * @param OutputPath Path to save the CSV file
	 * @return True if export succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|Extraction")
	static bool ExportToCSV(const TArray<FExtractedTextEntry>& Entries, const FString& OutputPath);
};
