#include "LocalizationTranslationManager.h"
#include "LocalizationHelper.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

bool ULocalizationTranslationManager::ExportForTranslation(const FString& StringTablePath, const FString& SourceLanguage, const FString& OutputPath)
{
	// Phase 3: StringTable export to CSV for translation
	// This would:
	// 1. Load StringTable
	// 2. Export entries with context information for translators
	// 3. Include metadata like character limits, formatting rules
	UE_LOG(LogLocalizationHelper, Warning, TEXT("ExportForTranslation is a Phase 3 feature - not yet implemented"));
	return false;
}

bool ULocalizationTranslationManager::ImportTranslations(const FString& CSVPath, const FString& StringTablePath, const FString& TargetLanguage)
{
	// Phase 3: Translation import from CSV
	// This would:
	// 1. Read CSV file with translations
	// 2. Validate translations (placeholders, length, etc.)
	// 3. Update StringTable with validated translations
	UE_LOG(LogLocalizationHelper, Warning, TEXT("ImportTranslations is a Phase 3 feature - not yet implemented"));
	return false;
}

TArray<FString> ULocalizationTranslationManager::FindMissingTranslations(const FString& StringTablePath, const FString& TargetLanguage)
{
	TArray<FString> MissingKeys;

	// Phase 3: Missing translation detection
	// This would:
	// 1. Load StringTable
	// 2. Check which keys have no translation for target language
	// 3. Return list of keys needing translation
	UE_LOG(LogLocalizationHelper, Warning, TEXT("FindMissingTranslations is a Phase 3 feature - not yet implemented"));

	return MissingKeys;
}

float ULocalizationTranslationManager::GetTranslationProgress(const FString& StringTablePath, const FString& TargetLanguage)
{
	// Phase 3: Calculate translation progress
	// This would:
	// 1. Load StringTable
	// 2. Count total entries vs translated entries
	// 3. Return percentage (0.0 to 100.0)
	UE_LOG(LogLocalizationHelper, Warning, TEXT("GetTranslationProgress is a Phase 3 feature - not yet implemented"));
	return 0.0f;
}

// Optimized helper function to extract placeholders from text
static void ExtractPlaceholders(const FString& Text, TSet<FString>& OutPlaceholders)
{
	// Find all placeholders in text (e.g., {0}, {1}, {PlayerName})
	// Using single pass through the string for better performance

	int32 Index = 0;
	const int32 TextLen = Text.Len();

	while (Index < TextLen)
	{
		// Find opening brace
		int32 StartIndex = Text.Find(TEXT("{"), ESearchCase::IgnoreCase, ESearchDir::FromStart, Index);
		if (StartIndex == INDEX_NONE)
			break;

		// Find closing brace
		int32 EndIndex = Text.Find(TEXT("}"), ESearchCase::IgnoreCase, ESearchDir::FromStart, StartIndex + 1);
		if (EndIndex == INDEX_NONE)
			break;

		// Extract placeholder including braces
		FString Placeholder = Text.Mid(StartIndex, EndIndex - StartIndex + 1);
		OutPlaceholders.Add(Placeholder);

		// Continue search after this placeholder
		Index = EndIndex + 1;
	}
}

bool ULocalizationTranslationManager::ValidatePlaceholders(const FString& SourceText, const FString& TranslatedText)
{
	// Extract placeholders from both texts using optimized single-pass algorithm
	TSet<FString> SourcePlaceholders;
	TSet<FString> TranslatedPlaceholders;

	ExtractPlaceholders(SourceText, SourcePlaceholders);
	ExtractPlaceholders(TranslatedText, TranslatedPlaceholders);

	// Check if placeholder sets match
	if (SourcePlaceholders.Num() != TranslatedPlaceholders.Num())
	{
		return false;
	}

	// Verify all source placeholders exist in translation
	for (const FString& SourcePlaceholder : SourcePlaceholders)
	{
		if (!TranslatedPlaceholders.Contains(SourcePlaceholder))
		{
			return false;
		}
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
