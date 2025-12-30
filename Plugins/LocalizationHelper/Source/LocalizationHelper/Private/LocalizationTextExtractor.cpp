#include "LocalizationTextExtractor.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

TArray<FExtractedTextEntry> ULocalizationTextExtractor::ExtractFromBlueprint(const FString& BlueprintPath)
{
	TArray<FExtractedTextEntry> ExtractedTexts;

	// TODO: Implement Blueprint text extraction
	// This would involve loading the Blueprint asset and parsing Widget text properties
	UE_LOG(LogTemp, Warning, TEXT("ExtractFromBlueprint not yet fully implemented for: %s"), *BlueprintPath);

	return ExtractedTexts;
}

TArray<FExtractedTextEntry> ULocalizationTextExtractor::ExtractFromCppSource(const FString& SourceFilePath)
{
	TArray<FExtractedTextEntry> ExtractedTexts;

	FString FileContent;
	if (!FFileHelper::LoadFileToString(FileContent, *SourceFilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load C++ source file: %s"), *SourceFilePath);
		return ExtractedTexts;
	}

	// Search for NSLOCTEXT and LOCTEXT patterns
	TArray<FString> Lines;
	FileContent.ParseIntoArrayLines(Lines);

	for (int32 LineIndex = 0; LineIndex < Lines.Num(); ++LineIndex)
	{
		const FString& Line = Lines[LineIndex];

		// Check for NSLOCTEXT
		if (Line.Contains(TEXT("NSLOCTEXT")))
		{
			FExtractedTextEntry Entry;
			Entry.Location = FString::Printf(TEXT("%s:%d"), *SourceFilePath, LineIndex + 1);
			Entry.Context = TEXT("C++ Source - NSLOCTEXT");
			// TODO: Parse NSLOCTEXT parameters to extract actual text
			Entry.SourceString = Line.TrimStartAndEnd();
			Entry.Key = FString::Printf(TEXT("CPP_%d"), LineIndex);
			ExtractedTexts.Add(Entry);
		}

		// Check for LOCTEXT
		if (Line.Contains(TEXT("LOCTEXT(")))
		{
			FExtractedTextEntry Entry;
			Entry.Location = FString::Printf(TEXT("%s:%d"), *SourceFilePath, LineIndex + 1);
			Entry.Context = TEXT("C++ Source - LOCTEXT");
			// TODO: Parse LOCTEXT parameters to extract actual text
			Entry.SourceString = Line.TrimStartAndEnd();
			Entry.Key = FString::Printf(TEXT("CPP_%d"), LineIndex);
			ExtractedTexts.Add(Entry);
		}
	}

	return ExtractedTexts;
}

TArray<FExtractedTextEntry> ULocalizationTextExtractor::ExtractFromDataAsset(const FString& AssetPath)
{
	TArray<FExtractedTextEntry> ExtractedTexts;

	// TODO: Implement data asset text extraction
	UE_LOG(LogTemp, Warning, TEXT("ExtractFromDataAsset not yet fully implemented for: %s"), *AssetPath);

	return ExtractedTexts;
}

TArray<FExtractedTextEntry> ULocalizationTextExtractor::ExtractAllTexts(const FString& ProjectPath)
{
	TArray<FExtractedTextEntry> AllExtractedTexts;

	// TODO: Recursively search project directories for text to extract
	UE_LOG(LogTemp, Warning, TEXT("ExtractAllTexts not yet fully implemented for: %s"), *ProjectPath);

	return AllExtractedTexts;
}

bool ULocalizationTextExtractor::ExportToCSV(const TArray<FExtractedTextEntry>& Entries, const FString& OutputPath)
{
	FString CSVContent = TEXT("Key,SourceString,Context,Location\n");

	for (const FExtractedTextEntry& Entry : Entries)
	{
		// Escape quotes in CSV
		FString EscapedSourceString = Entry.SourceString.Replace(TEXT("\""), TEXT("\"\""));
		FString EscapedContext = Entry.Context.Replace(TEXT("\""), TEXT("\"\""));
		FString EscapedLocation = Entry.Location.Replace(TEXT("\""), TEXT("\"\""));

		CSVContent += FString::Printf(TEXT("%s,\"%s\",\"%s\",\"%s\"\n"),
			*Entry.Key,
			*EscapedSourceString,
			*EscapedContext,
			*EscapedLocation
		);
	}

	return FFileHelper::SaveStringToFile(CSVContent, *OutputPath);
}
