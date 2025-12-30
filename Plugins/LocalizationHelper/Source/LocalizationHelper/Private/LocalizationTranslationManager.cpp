#include "LocalizationTranslationManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

bool ULocalizationTranslationManager::ExportForTranslation(const FString& StringTablePath, const FString& SourceLanguage, const FString& OutputPath)
{
	// TODO: Implement StringTable export to CSV
	// This would include context information for translators
	UE_LOG(LogTemp, Warning, TEXT("ExportForTranslation not yet fully implemented"));
	return false;
}

bool ULocalizationTranslationManager::ImportTranslations(const FString& CSVPath, const FString& StringTablePath, const FString& TargetLanguage)
{
	// TODO: Implement translation import from CSV
	// This would validate and import translations into the StringTable
	UE_LOG(LogTemp, Warning, TEXT("ImportTranslations not yet fully implemented"));
	return false;
}

TArray<FString> ULocalizationTranslationManager::FindMissingTranslations(const FString& StringTablePath, const FString& TargetLanguage)
{
	TArray<FString> MissingKeys;

	// TODO: Implement missing translation detection
	UE_LOG(LogTemp, Warning, TEXT("FindMissingTranslations not yet fully implemented"));

	return MissingKeys;
}

float ULocalizationTranslationManager::GetTranslationProgress(const FString& StringTablePath, const FString& TargetLanguage)
{
	// TODO: Calculate translation progress
	// Returns percentage of translated entries
	UE_LOG(LogTemp, Warning, TEXT("GetTranslationProgress not yet fully implemented"));
	return 0.0f;
}

bool ULocalizationTranslationManager::ValidatePlaceholders(const FString& SourceText, const FString& TranslatedText)
{
	// Find all placeholders in source text (e.g., {0}, {1}, {PlayerName})
	TArray<FString> SourcePlaceholders;
	TArray<FString> TranslatedPlaceholders;

	// Simple regex-like search for {xxx} patterns
	int32 Index = 0;
	while (Index < SourceText.Len())
	{
		int32 StartIndex = SourceText.Find(TEXT("{"), ESearchCase::IgnoreCase, ESearchDir::FromStart, Index);
		if (StartIndex == INDEX_NONE)
			break;

		int32 EndIndex = SourceText.Find(TEXT("}"), ESearchCase::IgnoreCase, ESearchDir::FromStart, StartIndex);
		if (EndIndex == INDEX_NONE)
			break;

		FString Placeholder = SourceText.Mid(StartIndex, EndIndex - StartIndex + 1);
		SourcePlaceholders.AddUnique(Placeholder);
		Index = EndIndex + 1;
	}

	// Same for translated text
	Index = 0;
	while (Index < TranslatedText.Len())
	{
		int32 StartIndex = TranslatedText.Find(TEXT("{"), ESearchCase::IgnoreCase, ESearchDir::FromStart, Index);
		if (StartIndex == INDEX_NONE)
			break;

		int32 EndIndex = TranslatedText.Find(TEXT("}"), ESearchCase::IgnoreCase, ESearchDir::FromStart, StartIndex);
		if (EndIndex == INDEX_NONE)
			break;

		FString Placeholder = TranslatedText.Mid(StartIndex, EndIndex - StartIndex + 1);
		TranslatedPlaceholders.AddUnique(Placeholder);
		Index = EndIndex + 1;
	}

	// Check if placeholders match
	if (SourcePlaceholders.Num() != TranslatedPlaceholders.Num())
		return false;

	for (const FString& SourcePlaceholder : SourcePlaceholders)
	{
		if (!TranslatedPlaceholders.Contains(SourcePlaceholder))
			return false;
	}

	return true;
}

bool ULocalizationTranslationManager::ValidateTextLength(const FString& SourceText, const FString& TranslatedText, float MaxLengthRatio)
{
	int32 SourceLength = SourceText.Len();
	int32 TranslatedLength = TranslatedText.Len();

	if (SourceLength == 0)
		return true;

	float Ratio = static_cast<float>(TranslatedLength) / static_cast<float>(SourceLength);
	return Ratio <= MaxLengthRatio;
}
