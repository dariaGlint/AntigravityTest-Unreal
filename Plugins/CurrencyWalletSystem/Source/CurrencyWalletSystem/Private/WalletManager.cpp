#include "WalletManager.h"

void UWalletManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	MaxTransactionHistorySize = 1000;
	RegisteredCurrencies.Empty();
	WalletBalances.Empty();
	TransactionHistory.Empty();
	ConversionRates.Empty();
}

void UWalletManager::Deinitialize()
{
	RegisteredCurrencies.Empty();
	WalletBalances.Empty();
	TransactionHistory.Empty();
	ConversionRates.Empty();

	Super::Deinitialize();
}

// Currency Registration
void UWalletManager::RegisterCurrency(const FCurrencyInfo& CurrencyInfo)
{
	if (CurrencyInfo.CurrencyID == NAME_None)
	{
		UE_LOG(LogTemp, Warning, TEXT("WalletManager: Cannot register currency with invalid ID"));
		return;
	}

	RegisteredCurrencies.Add(CurrencyInfo.CurrencyID, CurrencyInfo);

	if (!WalletBalances.Contains(CurrencyInfo.CurrencyID))
	{
		WalletBalances.Add(CurrencyInfo.CurrencyID, 0);
	}
}

void UWalletManager::UnregisterCurrency(FName CurrencyID)
{
	RegisteredCurrencies.Remove(CurrencyID);
	WalletBalances.Remove(CurrencyID);
}

bool UWalletManager::IsCurrencyRegistered(FName CurrencyID) const
{
	return RegisteredCurrencies.Contains(CurrencyID);
}

FCurrencyInfo UWalletManager::GetCurrencyInfo(FName CurrencyID) const
{
	if (const FCurrencyInfo* Info = RegisteredCurrencies.Find(CurrencyID))
	{
		return *Info;
	}
	return FCurrencyInfo();
}

TArray<FCurrencyInfo> UWalletManager::GetAllRegisteredCurrencies() const
{
	TArray<FCurrencyInfo> Result;
	RegisteredCurrencies.GenerateValueArray(Result);
	return Result;
}

// Wallet Operations
bool UWalletManager::AddCurrency(FName CurrencyID, int64 Amount, const FString& Reason)
{
	if (!ValidateCurrencyOperation(CurrencyID, Amount, true))
	{
		return false;
	}

	int64* CurrentAmount = WalletBalances.Find(CurrencyID);
	if (!CurrentAmount)
	{
		WalletBalances.Add(CurrencyID, 0);
		CurrentAmount = WalletBalances.Find(CurrencyID);
	}

	const int64 OldAmount = *CurrentAmount;
	const FCurrencyInfo* Info = RegisteredCurrencies.Find(CurrencyID);
	const int64 MaxAmount = Info ? Info->MaxAmount : 999999999;

	int64 NewAmount = OldAmount + Amount;
	if (NewAmount > MaxAmount)
	{
		NewAmount = MaxAmount;
		UE_LOG(LogTemp, Warning, TEXT("WalletManager: Currency %s reached maximum amount %lld"), *CurrencyID.ToString(), MaxAmount);
	}

	*CurrentAmount = NewAmount;

	FTransactionRecord Transaction(CurrencyID, ETransactionType::Earn, Amount, NewAmount, Reason);
	RecordTransaction(Transaction);

	OnCurrencyChanged.Broadcast(CurrencyID, OldAmount, NewAmount);

	return true;
}

bool UWalletManager::RemoveCurrency(FName CurrencyID, int64 Amount, const FString& Reason)
{
	if (!ValidateCurrencyOperation(CurrencyID, Amount, false))
	{
		return false;
	}

	int64* CurrentAmount = WalletBalances.Find(CurrencyID);
	if (!CurrentAmount || *CurrentAmount < Amount)
	{
		UE_LOG(LogTemp, Warning, TEXT("WalletManager: Insufficient currency %s. Required: %lld, Available: %lld"),
			*CurrencyID.ToString(), Amount, CurrentAmount ? *CurrentAmount : 0);
		return false;
	}

	const int64 OldAmount = *CurrentAmount;
	const int64 NewAmount = OldAmount - Amount;
	*CurrentAmount = NewAmount;

	FTransactionRecord Transaction(CurrencyID, ETransactionType::Spend, Amount, NewAmount, Reason);
	RecordTransaction(Transaction);

	OnCurrencyChanged.Broadcast(CurrencyID, OldAmount, NewAmount);

	return true;
}

bool UWalletManager::SetCurrency(FName CurrencyID, int64 Amount, const FString& Reason)
{
	if (!IsCurrencyRegistered(CurrencyID))
	{
		UE_LOG(LogTemp, Warning, TEXT("WalletManager: Currency %s is not registered"), *CurrencyID.ToString());
		return false;
	}

	const FCurrencyInfo* Info = RegisteredCurrencies.Find(CurrencyID);
	const int64 MaxAmount = Info ? Info->MaxAmount : 999999999;

	if (Amount > MaxAmount)
	{
		UE_LOG(LogTemp, Warning, TEXT("WalletManager: Amount %lld exceeds maximum %lld for currency %s"),
			Amount, MaxAmount, *CurrencyID.ToString());
		Amount = MaxAmount;
	}

	int64* CurrentAmount = WalletBalances.Find(CurrencyID);
	if (!CurrentAmount)
	{
		WalletBalances.Add(CurrencyID, Amount);
		CurrentAmount = WalletBalances.Find(CurrencyID);
	}

	const int64 OldAmount = *CurrentAmount;
	*CurrentAmount = Amount;

	ETransactionType TransactionType = (Amount > OldAmount) ? ETransactionType::Grant : ETransactionType::Remove;
	FTransactionRecord Transaction(CurrencyID, TransactionType, FMath::Abs(Amount - OldAmount), Amount, Reason);
	RecordTransaction(Transaction);

	OnCurrencyChanged.Broadcast(CurrencyID, OldAmount, Amount);

	return true;
}

int64 UWalletManager::GetCurrencyAmount(FName CurrencyID) const
{
	if (const int64* Amount = WalletBalances.Find(CurrencyID))
	{
		return *Amount;
	}
	return 0;
}

bool UWalletManager::HasEnoughCurrency(FName CurrencyID, int64 Amount) const
{
	return GetCurrencyAmount(CurrencyID) >= Amount;
}

TArray<FWalletCurrency> UWalletManager::GetAllCurrencies() const
{
	TArray<FWalletCurrency> Result;
	for (const auto& Pair : WalletBalances)
	{
		Result.Add(FWalletCurrency(Pair.Key, Pair.Value));
	}
	return Result;
}

// Currency Conversion
void UWalletManager::SetConversionRate(FName FromCurrencyID, FName ToCurrencyID, float Rate)
{
	if (!IsCurrencyRegistered(FromCurrencyID) || !IsCurrencyRegistered(ToCurrencyID))
	{
		UE_LOG(LogTemp, Warning, TEXT("WalletManager: Cannot set conversion rate for unregistered currencies"));
		return;
	}

	FCurrencyConversionRate ConversionRate(FromCurrencyID, ToCurrencyID, Rate);

	TArray<FCurrencyConversionRate>* Rates = ConversionRates.Find(FromCurrencyID);
	if (!Rates)
	{
		ConversionRates.Add(FromCurrencyID, TArray<FCurrencyConversionRate>());
		Rates = ConversionRates.Find(FromCurrencyID);
	}

	bool bFound = false;
	for (int32 i = 0; i < Rates->Num(); ++i)
	{
		if ((*Rates)[i].ToCurrencyID == ToCurrencyID)
		{
			(*Rates)[i].Rate = Rate;
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		Rates->Add(ConversionRate);
	}
}

float UWalletManager::GetConversionRate(FName FromCurrencyID, FName ToCurrencyID) const
{
	if (const TArray<FCurrencyConversionRate>* Rates = ConversionRates.Find(FromCurrencyID))
	{
		for (const FCurrencyConversionRate& Rate : *Rates)
		{
			if (Rate.ToCurrencyID == ToCurrencyID)
			{
				return Rate.Rate;
			}
		}
	}
	return 0.0f;
}

bool UWalletManager::ConvertCurrency(FName FromCurrencyID, FName ToCurrencyID, int64 Amount)
{
	if (!IsCurrencyRegistered(FromCurrencyID) || !IsCurrencyRegistered(ToCurrencyID))
	{
		UE_LOG(LogTemp, Warning, TEXT("WalletManager: Cannot convert between unregistered currencies"));
		return false;
	}

	if (!HasEnoughCurrency(FromCurrencyID, Amount))
	{
		UE_LOG(LogTemp, Warning, TEXT("WalletManager: Insufficient currency %s for conversion"), *FromCurrencyID.ToString());
		return false;
	}

	const int64 ConvertedAmount = CalculateConversionAmount(FromCurrencyID, ToCurrencyID, Amount);
	if (ConvertedAmount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("WalletManager: Invalid conversion rate between %s and %s"),
			*FromCurrencyID.ToString(), *ToCurrencyID.ToString());
		return false;
	}

	const FString Reason = FString::Printf(TEXT("Converted %lld %s to %s"), Amount, *FromCurrencyID.ToString(), *ToCurrencyID.ToString());

	if (RemoveCurrency(FromCurrencyID, Amount, Reason))
	{
		AddCurrency(ToCurrencyID, ConvertedAmount, Reason);
		OnCurrencyConverted.Broadcast(FromCurrencyID, ToCurrencyID);
		return true;
	}

	return false;
}

int64 UWalletManager::CalculateConversionAmount(FName FromCurrencyID, FName ToCurrencyID, int64 Amount) const
{
	const float Rate = GetConversionRate(FromCurrencyID, ToCurrencyID);
	return FMath::FloorToInt64(Amount * Rate);
}

// Transaction History
TArray<FTransactionRecord> UWalletManager::GetTransactionHistory(FName CurrencyID, int32 MaxRecords) const
{
	TArray<FTransactionRecord> Result;

	for (int32 i = TransactionHistory.Num() - 1; i >= 0 && Result.Num() < MaxRecords; --i)
	{
		if (TransactionHistory[i].CurrencyID == CurrencyID)
		{
			Result.Add(TransactionHistory[i]);
		}
	}

	return Result;
}

TArray<FTransactionRecord> UWalletManager::GetAllTransactionHistory(int32 MaxRecords) const
{
	TArray<FTransactionRecord> Result;

	const int32 StartIndex = FMath::Max(0, TransactionHistory.Num() - MaxRecords);
	for (int32 i = TransactionHistory.Num() - 1; i >= StartIndex; --i)
	{
		Result.Add(TransactionHistory[i]);
	}

	return Result;
}

void UWalletManager::ClearTransactionHistory(FName CurrencyID)
{
	TransactionHistory.RemoveAll([CurrencyID](const FTransactionRecord& Record)
	{
		return Record.CurrencyID == CurrencyID;
	});
}

void UWalletManager::ClearAllTransactionHistory()
{
	TransactionHistory.Empty();
}

int32 UWalletManager::GetTransactionCount(FName CurrencyID) const
{
	int32 Count = 0;
	for (const FTransactionRecord& Record : TransactionHistory)
	{
		if (Record.CurrencyID == CurrencyID)
		{
			++Count;
		}
	}
	return Count;
}

// Private Methods
void UWalletManager::RecordTransaction(const FTransactionRecord& Transaction)
{
	TransactionHistory.Add(Transaction);

	if (TransactionHistory.Num() > MaxTransactionHistorySize)
	{
		TransactionHistory.RemoveAt(0);
	}

	OnTransactionRecorded.Broadcast(Transaction);
}

bool UWalletManager::ValidateCurrencyOperation(FName CurrencyID, int64 Amount, bool bIsAddition) const
{
	if (!IsCurrencyRegistered(CurrencyID))
	{
		UE_LOG(LogTemp, Warning, TEXT("WalletManager: Currency %s is not registered"), *CurrencyID.ToString());
		return false;
	}

	if (Amount < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("WalletManager: Amount cannot be negative"));
		return false;
	}

	return true;
}

FString UWalletManager::MakeConversionKey(FName FromCurrencyID, FName ToCurrencyID) const
{
	return FString::Printf(TEXT("%s_to_%s"), *FromCurrencyID.ToString(), *ToCurrencyID.ToString());
}
