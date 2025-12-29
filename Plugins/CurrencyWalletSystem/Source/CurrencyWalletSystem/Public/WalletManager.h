#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CurrencyTypes.h"
#include "WalletManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCurrencyChanged, FName, CurrencyID, int64, OldAmount, int64, NewAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransactionRecorded, const FTransactionRecord&, Transaction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrencyConverted, FName, FromCurrencyID, FName, ToCurrencyID);

UCLASS()
class CURRENCYWALLETSYSTEM_API UWalletManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Currency Registration
	UFUNCTION(BlueprintCallable, Category = "Wallet")
	void RegisterCurrency(const FCurrencyInfo& CurrencyInfo);

	UFUNCTION(BlueprintCallable, Category = "Wallet")
	void UnregisterCurrency(FName CurrencyID);

	UFUNCTION(BlueprintPure, Category = "Wallet")
	bool IsCurrencyRegistered(FName CurrencyID) const;

	UFUNCTION(BlueprintPure, Category = "Wallet")
	FCurrencyInfo GetCurrencyInfo(FName CurrencyID) const;

	UFUNCTION(BlueprintPure, Category = "Wallet")
	TArray<FCurrencyInfo> GetAllRegisteredCurrencies() const;

	// Wallet Operations
	UFUNCTION(BlueprintCallable, Category = "Wallet")
	bool AddCurrency(FName CurrencyID, int64 Amount, const FString& Reason = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Wallet")
	bool RemoveCurrency(FName CurrencyID, int64 Amount, const FString& Reason = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Wallet")
	bool SetCurrency(FName CurrencyID, int64 Amount, const FString& Reason = TEXT(""));

	UFUNCTION(BlueprintPure, Category = "Wallet")
	int64 GetCurrencyAmount(FName CurrencyID) const;

	UFUNCTION(BlueprintPure, Category = "Wallet")
	bool HasEnoughCurrency(FName CurrencyID, int64 Amount) const;

	UFUNCTION(BlueprintPure, Category = "Wallet")
	TArray<FWalletCurrency> GetAllCurrencies() const;

	// Currency Conversion
	UFUNCTION(BlueprintCallable, Category = "Wallet|Conversion")
	void SetConversionRate(FName FromCurrencyID, FName ToCurrencyID, float Rate);

	UFUNCTION(BlueprintPure, Category = "Wallet|Conversion")
	float GetConversionRate(FName FromCurrencyID, FName ToCurrencyID) const;

	UFUNCTION(BlueprintCallable, Category = "Wallet|Conversion")
	bool ConvertCurrency(FName FromCurrencyID, FName ToCurrencyID, int64 Amount);

	UFUNCTION(BlueprintPure, Category = "Wallet|Conversion")
	int64 CalculateConversionAmount(FName FromCurrencyID, FName ToCurrencyID, int64 Amount) const;

	// Transaction History
	UFUNCTION(BlueprintPure, Category = "Wallet|History")
	TArray<FTransactionRecord> GetTransactionHistory(FName CurrencyID, int32 MaxRecords = 100) const;

	UFUNCTION(BlueprintPure, Category = "Wallet|History")
	TArray<FTransactionRecord> GetAllTransactionHistory(int32 MaxRecords = 100) const;

	UFUNCTION(BlueprintCallable, Category = "Wallet|History")
	void ClearTransactionHistory(FName CurrencyID);

	UFUNCTION(BlueprintCallable, Category = "Wallet|History")
	void ClearAllTransactionHistory();

	UFUNCTION(BlueprintPure, Category = "Wallet|History")
	int32 GetTransactionCount(FName CurrencyID) const;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Wallet")
	FOnCurrencyChanged OnCurrencyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wallet")
	FOnTransactionRecorded OnTransactionRecorded;

	UPROPERTY(BlueprintAssignable, Category = "Wallet")
	FOnCurrencyConverted OnCurrencyConverted;

protected:
	UPROPERTY()
	TMap<FName, FCurrencyInfo> RegisteredCurrencies;

	UPROPERTY()
	TMap<FName, int64> WalletBalances;

	UPROPERTY()
	TArray<FTransactionRecord> TransactionHistory;

	UPROPERTY()
	TMap<FName, TArray<FCurrencyConversionRate>> ConversionRates;

	UPROPERTY(EditDefaultsOnly, Category = "Wallet|Settings")
	int32 MaxTransactionHistorySize;

private:
	void RecordTransaction(const FTransactionRecord& Transaction);
	bool ValidateCurrencyOperation(FName CurrencyID, int64 Amount, bool bIsAddition) const;
	FString MakeConversionKey(FName FromCurrencyID, FName ToCurrencyID) const;
};
