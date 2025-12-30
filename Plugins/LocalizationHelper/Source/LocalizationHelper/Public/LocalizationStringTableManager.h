#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LocalizationStringTableManager.generated.h"

/**
 * StringTable Manager for localization workflow
 * Manages StringTable creation, updates, and validation
 */
UCLASS(BlueprintType)
class LOCALIZATIONHELPER_API ULocalizationStringTableManager : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Create a new StringTable from CSV data
	 * @param CSVPath Path to the CSV file containing text entries
	 * @param StringTableName Name for the new StringTable
	 * @param OutputPath Path where the StringTable should be saved
	 * @return True if StringTable was created successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|StringTable")
	static bool CreateStringTableFromCSV(const FString& CSVPath, const FString& StringTableName, const FString& OutputPath);

	/**
	 * Add entries to an existing StringTable
	 * @param StringTablePath Path to the existing StringTable
	 * @param CSVPath Path to the CSV file containing new entries
	 * @return True if entries were added successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|StringTable")
	static bool AddEntriesToStringTable(const FString& StringTablePath, const FString& CSVPath);

	/**
	 * Find duplicate entries across StringTables
	 * @param StringTablePaths Array of StringTable paths to check
	 * @return Array of duplicate entry keys
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|StringTable")
	static TArray<FString> FindDuplicateEntries(const TArray<FString>& StringTablePaths);

	/**
	 * Find unused StringTable entries
	 * @param StringTablePath Path to the StringTable
	 * @param ProjectPath Path to project root for searching references
	 * @return Array of unused entry keys
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|StringTable")
	static TArray<FString> FindUnusedEntries(const FString& StringTablePath, const FString& ProjectPath);

	/**
	 * Generate a consistent key for a text entry
	 * @param Category Category prefix (e.g., "UI", "Quest", "Dialog")
	 * @param Subcategory Subcategory (e.g., "Combat", "Q001", "NPC01")
	 * @param Identifier Specific identifier (e.g., "Health", "Title", "Greeting")
	 * @return Generated key in format "Category.Subcategory.Identifier"
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|StringTable")
	static FString GenerateKey(const FString& Category, const FString& Subcategory, const FString& Identifier);

	/**
	 * Validate StringTable key naming conventions
	 * @param StringTablePath Path to the StringTable
	 * @return Array of keys that don't follow naming conventions
	 */
	UFUNCTION(BlueprintCallable, Category = "Localization|StringTable")
	static TArray<FString> ValidateKeyNamingConventions(const FString& StringTablePath);
};
