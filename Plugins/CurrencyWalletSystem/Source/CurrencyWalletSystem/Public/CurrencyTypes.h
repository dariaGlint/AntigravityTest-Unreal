#pragma once

#include "CoreMinimal.h"
#include "CurrencyTypes.generated.h"

UENUM(BlueprintType)
enum class ETransactionType : uint8
{
	Earn UMETA(DisplayName = "Earn"),
	Spend UMETA(DisplayName = "Spend"),
	Convert UMETA(DisplayName = "Convert"),
	Grant UMETA(DisplayName = "Grant"),
	Remove UMETA(DisplayName = "Remove")
};

USTRUCT(BlueprintType)
struct CURRENCYWALLETSYSTEM_API FCurrencyInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency")
	FName CurrencyID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency")
	int64 MaxAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency")
	UTexture2D* Icon;

	FCurrencyInfo()
		: CurrencyID(NAME_None)
		, DisplayName(FText::GetEmpty())
		, Description(FText::GetEmpty())
		, MaxAmount(999999999)
		, Icon(nullptr)
	{}

	FCurrencyInfo(FName InID, FText InDisplayName, int64 InMaxAmount = 999999999)
		: CurrencyID(InID)
		, DisplayName(InDisplayName)
		, Description(FText::GetEmpty())
		, MaxAmount(InMaxAmount)
		, Icon(nullptr)
	{}
};

USTRUCT(BlueprintType)
struct CURRENCYWALLETSYSTEM_API FWalletCurrency
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wallet")
	FName CurrencyID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wallet")
	int64 Amount;

	FWalletCurrency()
		: CurrencyID(NAME_None)
		, Amount(0)
	{}

	FWalletCurrency(FName InCurrencyID, int64 InAmount)
		: CurrencyID(InCurrencyID)
		, Amount(InAmount)
	{}
};

USTRUCT(BlueprintType)
struct CURRENCYWALLETSYSTEM_API FTransactionRecord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transaction")
	FName CurrencyID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transaction")
	ETransactionType TransactionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transaction")
	int64 Amount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transaction")
	int64 BalanceAfter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transaction")
	FDateTime Timestamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transaction")
	FString Reason;

	FTransactionRecord()
		: CurrencyID(NAME_None)
		, TransactionType(ETransactionType::Earn)
		, Amount(0)
		, BalanceAfter(0)
		, Timestamp(FDateTime::Now())
		, Reason(TEXT(""))
	{}

	FTransactionRecord(FName InCurrencyID, ETransactionType InType, int64 InAmount, int64 InBalance, const FString& InReason = TEXT(""))
		: CurrencyID(InCurrencyID)
		, TransactionType(InType)
		, Amount(InAmount)
		, BalanceAfter(InBalance)
		, Timestamp(FDateTime::Now())
		, Reason(InReason)
	{}
};

USTRUCT(BlueprintType)
struct CURRENCYWALLETSYSTEM_API FCurrencyConversionRate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversion")
	FName FromCurrencyID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversion")
	FName ToCurrencyID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversion")
	float Rate;

	FCurrencyConversionRate()
		: FromCurrencyID(NAME_None)
		, ToCurrencyID(NAME_None)
		, Rate(1.0f)
	{}

	FCurrencyConversionRate(FName InFromID, FName InToID, float InRate)
		: FromCurrencyID(InFromID)
		, ToCurrencyID(InToID)
		, Rate(InRate)
	{}
};
