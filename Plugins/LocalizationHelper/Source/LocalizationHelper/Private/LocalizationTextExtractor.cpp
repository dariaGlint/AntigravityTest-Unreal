#include "LocalizationTextExtractor.h"
#include "LocalizationHelper.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

// Helper function to validate file paths and prevent path traversal attacks
static bool ValidateFilePath(const FString& FilePath, FString& OutNormalizedPath)
{
	// Normalize the path to resolve .. and . components
	OutNormalizedPath = FPaths::ConvertRelativePathToFull(FilePath);

	// Check for suspicious path components
	if (FilePath.Contains(TEXT("..")) || FilePath.Contains(TEXT("~")))
	{
		UE_LOG(LogLocalizationHelper, Error, TEXT("Invalid file path detected (potential path traversal): %s"), *FilePath);
		return false;
	}

	// Ensure the file exists
	if (!FPaths::FileExists(OutNormalizedPath))
	{
		UE_LOG(LogLocalizationHelper, Warning, TEXT("File does not exist: %s"), *OutNormalizedPath);
		return false;
	}

	return true;
}

// Helper function to properly escape CSV fields
static FString EscapeCSVField(const FString& Field)
{
	// CSV escaping rules:
	// 1. If field contains quotes, newlines, carriage returns, or commas, it must be quoted
	// 2. Any quotes inside the field must be doubled

	bool bNeedsQuoting = Field.Contains(TEXT("\"")) ||
	                      Field.Contains(TEXT("\n")) ||
	                      Field.Contains(TEXT("\r")) ||
	                      Field.Contains(TEXT(","));

	if (!bNeedsQuoting)
	{
		return Field;
	}

	// Escape quotes by doubling them
	FString Escaped = Field.Replace(TEXT("\""), TEXT("\"\""));

	// Wrap in quotes
	return FString::Printf(TEXT("\"%s\""), *Escaped);
}

TArray<FExtractedTextEntry> ULocalizationTextExtractor::ExtractFromBlueprint(const FString& BlueprintPath)
{
	TArray<FExtractedTextEntry> ExtractedTexts;

	// Validate file path
	FString NormalizedPath;
	if (!ValidateFilePath(BlueprintPath, NormalizedPath))
	{
		return ExtractedTexts;
	}

	// TODO: Phase 2 - Implement Blueprint text extraction
	// This would involve loading the Blueprint asset and parsing Widget text properties
	// For now, we log a message indicating this is not yet implemented
	UE_LOG(LogLocalizationHelper, Warning, TEXT("ExtractFromBlueprint not yet fully implemented for: %s"), *BlueprintPath);

	return ExtractedTexts;
}

// Helper function to extract text from LOCTEXT macro
// LOCTEXT("Key", "Text")
static bool ParseLOCTEXT(const FString& Line, FString& OutKey, FString& OutText)
{
	int32 StartIdx = Line.Find(TEXT("LOCTEXT("));
	if (StartIdx == INDEX_NONE)
	{
		return false;
	}

	StartIdx += 8; // Skip "LOCTEXT("

	// Find the first string parameter (key)
	int32 FirstQuoteIdx = Line.Find(TEXT("\""), ESearchCase::IgnoreCase, ESearchDir::FromStart, StartIdx);
	if (FirstQuoteIdx == INDEX_NONE)
	{
		return false;
	}

	int32 FirstQuoteEndIdx = Line.Find(TEXT("\""), ESearchCase::IgnoreCase, ESearchDir::FromStart, FirstQuoteIdx + 1);
	if (FirstQuoteEndIdx == INDEX_NONE)
	{
		return false;
	}

	OutKey = Line.Mid(FirstQuoteIdx + 1, FirstQuoteEndIdx - FirstQuoteIdx - 1);

	// Find the second string parameter (text)
	int32 SecondQuoteIdx = Line.Find(TEXT("\""), ESearchCase::IgnoreCase, ESearchDir::FromStart, FirstQuoteEndIdx + 1);
	if (SecondQuoteIdx == INDEX_NONE)
	{
		return false;
	}

	int32 SecondQuoteEndIdx = Line.Find(TEXT("\""), ESearchCase::IgnoreCase, ESearchDir::FromStart, SecondQuoteIdx + 1);
	if (SecondQuoteEndIdx == INDEX_NONE)
	{
		return false;
	}

	OutText = Line.Mid(SecondQuoteIdx + 1, SecondQuoteEndIdx - SecondQuoteIdx - 1);

	return true;
}

// Helper function to extract text from NSLOCTEXT macro
// NSLOCTEXT("Namespace", "Key", "Text")
static bool ParseNSLOCTEXT(const FString& Line, FString& OutNamespace, FString& OutKey, FString& OutText)
{
	int32 StartIdx = Line.Find(TEXT("NSLOCTEXT("));
	if (StartIdx == INDEX_NONE)
	{
		return false;
	}

	StartIdx += 10; // Skip "NSLOCTEXT("

	// Extract three string parameters
	TArray<FString> Parameters;
	int32 SearchIdx = StartIdx;

	for (int32 i = 0; i < 3; ++i)
	{
		int32 QuoteStartIdx = Line.Find(TEXT("\""), ESearchCase::IgnoreCase, ESearchDir::FromStart, SearchIdx);
		if (QuoteStartIdx == INDEX_NONE)
		{
			return false;
		}

		int32 QuoteEndIdx = Line.Find(TEXT("\""), ESearchCase::IgnoreCase, ESearchDir::FromStart, QuoteStartIdx + 1);
		if (QuoteEndIdx == INDEX_NONE)
		{
			return false;
		}

		Parameters.Add(Line.Mid(QuoteStartIdx + 1, QuoteEndIdx - QuoteStartIdx - 1));
		SearchIdx = QuoteEndIdx + 1;
	}

	if (Parameters.Num() == 3)
	{
		OutNamespace = Parameters[0];
		OutKey = Parameters[1];
		OutText = Parameters[2];
		return true;
	}

	return false;
}

TArray<FExtractedTextEntry> ULocalizationTextExtractor::ExtractFromCppSource(const FString& SourceFilePath)
{
	TArray<FExtractedTextEntry> ExtractedTexts;

	// Validate file path
	FString NormalizedPath;
	if (!ValidateFilePath(SourceFilePath, NormalizedPath))
	{
		return ExtractedTexts;
	}

	FString FileContent;
	if (!FFileHelper::LoadFileToString(FileContent, *NormalizedPath))
	{
		UE_LOG(LogLocalizationHelper, Error, TEXT("Failed to load C++ source file: %s"), *NormalizedPath);
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
			FString Namespace, Key, Text;
			if (ParseNSLOCTEXT(Line, Namespace, Key, Text))
			{
				FExtractedTextEntry Entry;
				Entry.Location = FString::Printf(TEXT("%s:%d"), *SourceFilePath, LineIndex + 1);
				Entry.Context = FString::Printf(TEXT("C++ Source - NSLOCTEXT(%s)"), *Namespace);
				Entry.SourceString = Text;
				Entry.Key = Key;
				ExtractedTexts.Add(Entry);
			}
			else
			{
				UE_LOG(LogLocalizationHelper, Warning, TEXT("Failed to parse NSLOCTEXT at %s:%d"), *SourceFilePath, LineIndex + 1);
			}
		}

		// Check for LOCTEXT
		if (Line.Contains(TEXT("LOCTEXT(")))
		{
			FString Key, Text;
			if (ParseLOCTEXT(Line, Key, Text))
			{
				FExtractedTextEntry Entry;
				Entry.Location = FString::Printf(TEXT("%s:%d"), *SourceFilePath, LineIndex + 1);
				Entry.Context = TEXT("C++ Source - LOCTEXT");
				Entry.SourceString = Text;
				Entry.Key = Key;
				ExtractedTexts.Add(Entry);
			}
			else
			{
				UE_LOG(LogLocalizationHelper, Warning, TEXT("Failed to parse LOCTEXT at %s:%d"), *SourceFilePath, LineIndex + 1);
			}
		}
	}

	UE_LOG(LogLocalizationHelper, Log, TEXT("Extracted %d text entries from %s"), ExtractedTexts.Num(), *SourceFilePath);

	return ExtractedTexts;
}

TArray<FExtractedTextEntry> ULocalizationTextExtractor::ExtractFromDataAsset(const FString& AssetPath)
{
	TArray<FExtractedTextEntry> ExtractedTexts;

	// Validate file path
	FString NormalizedPath;
	if (!ValidateFilePath(AssetPath, NormalizedPath))
	{
		return ExtractedTexts;
	}

	// TODO: Phase 2 - Implement data asset text extraction
	// This would involve loading data assets and extracting FText properties
	UE_LOG(LogLocalizationHelper, Warning, TEXT("ExtractFromDataAsset not yet fully implemented for: %s"), *AssetPath);

	return ExtractedTexts;
}

TArray<FExtractedTextEntry> ULocalizationTextExtractor::ExtractAllTexts(const FString& ProjectPath)
{
	TArray<FExtractedTextEntry> AllExtractedTexts;

	// Normalize and validate project path
	FString NormalizedPath = FPaths::ConvertRelativePathToFull(ProjectPath);

	if (!FPaths::DirectoryExists(NormalizedPath))
	{
		UE_LOG(LogLocalizationHelper, Error, TEXT("Project directory does not exist: %s"), *NormalizedPath);
		return AllExtractedTexts;
	}

	UE_LOG(LogLocalizationHelper, Log, TEXT("Scanning project directory for C++ source files: %s"), *NormalizedPath);

	// Find all C++ source files in the project
	TArray<FString> SourceFiles;
	IFileManager& FileManager = IFileManager::Get();

	// Search for .cpp and .h files in Source directory
	FString SourceDir = FPaths::Combine(NormalizedPath, TEXT("Source"));
	if (FPaths::DirectoryExists(SourceDir))
	{
		FileManager.FindFilesRecursive(SourceFiles, *SourceDir, TEXT("*.cpp"), true, false);

		TArray<FString> HeaderFiles;
		FileManager.FindFilesRecursive(HeaderFiles, *SourceDir, TEXT("*.h"), true, false);
		SourceFiles.Append(HeaderFiles);
	}

	UE_LOG(LogLocalizationHelper, Log, TEXT("Found %d C++ source files"), SourceFiles.Num());

	// Extract text from each source file
	for (const FString& SourceFile : SourceFiles)
	{
		TArray<FExtractedTextEntry> FileTexts = ExtractFromCppSource(SourceFile);
		AllExtractedTexts.Append(FileTexts);
	}

	UE_LOG(LogLocalizationHelper, Log, TEXT("Total extracted text entries: %d"), AllExtractedTexts.Num());

	return AllExtractedTexts;
}

bool ULocalizationTextExtractor::ExportToCSV(const TArray<FExtractedTextEntry>& Entries, const FString& OutputPath)
{
	// Validate output path to prevent path traversal
	FString NormalizedOutputPath = FPaths::ConvertRelativePathToFull(OutputPath);

	if (OutputPath.Contains(TEXT("..")) || OutputPath.Contains(TEXT("~")))
	{
		UE_LOG(LogLocalizationHelper, Error, TEXT("Invalid output path detected (potential path traversal): %s"), *OutputPath);
		return false;
	}

	// Create directory if it doesn't exist
	FString OutputDir = FPaths::GetPath(NormalizedOutputPath);
	if (!FPaths::DirectoryExists(OutputDir))
	{
		if (!IFileManager::Get().MakeDirectory(*OutputDir, true))
		{
			UE_LOG(LogLocalizationHelper, Error, TEXT("Failed to create output directory: %s"), *OutputDir);
			return false;
		}
	}

	FString CSVContent = TEXT("Key,SourceString,Context,Location\n");

	for (const FExtractedTextEntry& Entry : Entries)
	{
		// Use proper CSV escaping for all fields
		FString EscapedKey = EscapeCSVField(Entry.Key);
		FString EscapedSourceString = EscapeCSVField(Entry.SourceString);
		FString EscapedContext = EscapeCSVField(Entry.Context);
		FString EscapedLocation = EscapeCSVField(Entry.Location);

		CSVContent += FString::Printf(TEXT("%s,%s,%s,%s\n"),
			*EscapedKey,
			*EscapedSourceString,
			*EscapedContext,
			*EscapedLocation
		);
	}

	if (!FFileHelper::SaveStringToFile(CSVContent, *NormalizedOutputPath))
	{
		UE_LOG(LogLocalizationHelper, Error, TEXT("Failed to save CSV file to: %s"), *NormalizedOutputPath);
		return false;
	}

	UE_LOG(LogLocalizationHelper, Log, TEXT("Successfully exported %d entries to CSV: %s"), Entries.Num(), *NormalizedOutputPath);
	return true;
}
