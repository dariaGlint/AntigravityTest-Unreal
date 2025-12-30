#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LocalizationTranslationManager.generated.h"

/**
 * Translation entry for import/export
 */
USTRUCT(BlueprintType)
struct FTranslationEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Localization")
	FString Key;

	UPROPERTY(BlueprintReadWrite, Category = "Localization")
	FString SourceText;

	UPROPERTY(BlueprintReadWrite, Category = "Localization")
	FString TranslatedText;

	UPROPERTY(BlueprintReadWrite, Category = "Localization")
	FString Context;

	UPROPERTY(BlueprintReadWrite, Category = "Localization")
	FString SourceLanguage;

	UPROPERTY(BlueprintReadWrite, Category = "Localization")
	FString TargetLanguage;

	FTranslationEntry()
		: Key(TEXT(""))
		, SourceText(TEXT(""))
		, TranslatedText(TEXT(""))
		, Context(TEXT(""))
		, SourceLanguage(TEXT("en"))
		, TargetLanguage(TEXT("ja"))
	{}
};

/**
 * Translation Workflow Manager
 * Handles translation import/export, validation, and machine translation integration
 */
UCLASS(BlueprintType)
class LOCALIZATIONHELPER_API ULocalizationTranslationManager : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Export StringTable to CSV for translation
	 * @param StringTablePath Path to the StringTable
	 * @param SourceLanguage Source language code (e.g., "en")
	 * @param OutputPath Path to save the CSV file
	 * @return True if export succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|Translation")
	static bool ExportForTranslation(const FString& StringTablePath, const FString& SourceLanguage, const FString& OutputPath);

	/**
	 * Import translations from CSV back to StringTable
	 * @param CSVPath Path to the CSV file with translations
	 * @param StringTablePath Path to the StringTable to update
	 * @param TargetLanguage Target language code (e.g., "ja")
	 * @return True if import succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|Translation")
	static bool ImportTranslations(const FString& CSVPath, const FString& StringTablePath, const FString& TargetLanguage);

	/**
	 * Validate translation completeness
	 * @param StringTablePath Path to the StringTable
	 * @param TargetLanguage Target language code
	 * @return Array of keys that are missing translations
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|Translation")
	static TArray<FString> FindMissingTranslations(const FString& StringTablePath, const FString& TargetLanguage);

	/**
	 * Calculate translation progress percentage
	 * @param StringTablePath Path to the StringTable
	 * @param TargetLanguage Target language code
	 * @return Percentage of translated entries (0.0 to 100.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|Translation")
	static float GetTranslationProgress(const FString& StringTablePath, const FString& TargetLanguage);

	/**
	 * Validate placeholder consistency between source and translation
	 * @param SourceText Source text with placeholders
	 * @param TranslatedText Translated text
	 * @return True if placeholders match
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|Translation")
	static bool ValidatePlaceholders(const FString& SourceText, const FString& TranslatedText);

	/**
	 * Check if translation text length is within acceptable range
	 * @param SourceText Source text
	 * @param TranslatedText Translated text
	 * @param MaxLengthRatio Maximum allowed length ratio (default 1.5)
	 * @return True if length is acceptable
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|Translation")
	static bool ValidateTextLength(const FString& SourceText, const FString& TranslatedText, float MaxLengthRatio = 1.5f);
};
