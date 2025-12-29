# サンプルコード

## 基本的なショップの実装

### Game Instanceでのショップ初期化

```cpp
// MyGameInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

UCLASS()
class UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Shop")
    TArray<UProductItem*> InitialShopProducts;
};

// MyGameInstance.cpp
#include "MyGameInstance.h"
#include "ShopManager.h"

void UMyGameInstance::Init()
{
    Super::Init();

    UShopManager* ShopManager = GetSubsystem<UShopManager>();
    if (ShopManager)
    {
        // 初期商品をショップに追加
        for (UProductItem* Product : InitialShopProducts)
        {
            if (Product)
            {
                // 在庫10個、有限在庫として追加
                ShopManager->AddProductToShop(Product, 10, false);
            }
        }

        // 価格変動の設定
        FPriceFluctuationConfig Config;
        Config.FluctuationMode = EPriceFluctuationMode::SupplyDemand;
        Config.MinPriceMultiplier = 0.8f;
        Config.MaxPriceMultiplier = 1.5f;
        Config.bAffectsByDemand = true;
        ShopManager->SetPriceFluctuationConfig(Config);
    }
}
```

## プレイヤーキャラクターの設定

```cpp
// MyPlayerCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InventoryComponent.h"
#include "MyPlayerCharacter.generated.h"

UCLASS()
class AMyPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyPlayerCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    UInventoryComponent* InventoryComponent;

    UFUNCTION(BlueprintCallable, Category = "Shop")
    void OpenShop();

protected:
    virtual void BeginPlay() override;
};

// MyPlayerCharacter.cpp
#include "MyPlayerCharacter.h"

AMyPlayerCharacter::AMyPlayerCharacter()
{
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void AMyPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 初期通貨を設定
    if (InventoryComponent)
    {
        InventoryComponent->SetCurrency(1000.0f);
    }
}

void AMyPlayerCharacter::OpenShop()
{
    // ショップUIを開く処理
    // UIウィジェットでShopManagerを使用してトランザクションを実行
}
```

## ショップUIウィジェット（Blueprint例）

### C++バックエンド

```cpp
// ShopWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProductItem.h"
#include "ShopWidget.generated.h"

UCLASS()
class UShopWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Shop")
    void RefreshProductList();

    UFUNCTION(BlueprintCallable, Category = "Shop")
    void BuySelectedProduct(UProductItem* Product, int32 Quantity);

    UFUNCTION(BlueprintCallable, Category = "Shop")
    void SellSelectedProduct(UProductItem* Product, int32 Quantity);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shop")
    void OnProductListRefreshed(const TArray<FShopProductEntry>& Products);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shop")
    void OnTransactionResult(bool bSuccess, const FText& Message);

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void HandleTransactionComplete(bool bSuccess, UProductItem* Product, int32 Quantity);

    UPROPERTY()
    class UShopManager* ShopManager;

    UPROPERTY()
    class UInventoryComponent* PlayerInventory;
};

// ShopWidget.cpp
#include "ShopWidget.h"
#include "ShopManager.h"
#include "GameFramework/PlayerController.h"

void UShopWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ShopManager = GetGameInstance()->GetSubsystem<UShopManager>();
    if (ShopManager)
    {
        ShopManager->OnTransactionComplete.AddDynamic(this, &UShopWidget::HandleTransactionComplete);
    }

    APlayerController* PC = GetOwningPlayer();
    if (PC && PC->GetPawn())
    {
        PlayerInventory = PC->GetPawn()->FindComponentByClass<UInventoryComponent>();
    }

    RefreshProductList();
}

void UShopWidget::RefreshProductList()
{
    if (ShopManager)
    {
        TArray<FShopProductEntry> Products = ShopManager->GetAvailableProducts();
        OnProductListRefreshed(Products);
    }
}

void UShopWidget::BuySelectedProduct(UProductItem* Product, int32 Quantity)
{
    if (!ShopManager || !PlayerInventory || !Product)
    {
        OnTransactionResult(false, FText::FromString("Invalid transaction"));
        return;
    }

    if (!ShopManager->CanBuyProduct(PlayerInventory, Product, Quantity))
    {
        OnTransactionResult(false, FText::FromString("Cannot buy product"));
        return;
    }

    bool bSuccess = ShopManager->BuyProduct(PlayerInventory, Product, Quantity);
}

void UShopWidget::SellSelectedProduct(UProductItem* Product, int32 Quantity)
{
    if (!ShopManager || !PlayerInventory || !Product)
    {
        OnTransactionResult(false, FText::FromString("Invalid transaction"));
        return;
    }

    if (!ShopManager->CanSellProduct(PlayerInventory, Product, Quantity))
    {
        OnTransactionResult(false, FText::FromString("Cannot sell product"));
        return;
    }

    bool bSuccess = ShopManager->SellProduct(PlayerInventory, Product, Quantity);
}

void UShopWidget::HandleTransactionComplete(bool bSuccess, UProductItem* Product, int32 Quantity)
{
    if (bSuccess)
    {
        FText Message = FText::Format(
            FText::FromString("Transaction completed: {0} x{1}"),
            Product->ProductName,
            FText::AsNumber(Quantity)
        );
        OnTransactionResult(true, Message);
        RefreshProductList();
    }
    else
    {
        OnTransactionResult(false, FText::FromString("Transaction failed"));
    }
}
```

## 動的価格更新システム

```cpp
// PriceUpdateManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PriceUpdateManager.generated.h"

UCLASS()
class APriceUpdateManager : public AActor
{
    GENERATED_BODY()

public:
    APriceUpdateManager();

    UPROPERTY(EditAnywhere, Category = "Price Update")
    float UpdateInterval = 300.0f; // 5分ごと

protected:
    virtual void BeginPlay() override;

private:
    FTimerHandle PriceUpdateTimer;

    UFUNCTION()
    void UpdatePrices();
};

// PriceUpdateManager.cpp
#include "PriceUpdateManager.h"
#include "ShopManager.h"

APriceUpdateManager::APriceUpdateManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APriceUpdateManager::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetTimerManager().SetTimer(
        PriceUpdateTimer,
        this,
        &APriceUpdateManager::UpdatePrices,
        UpdateInterval,
        true
    );
}

void APriceUpdateManager::UpdatePrices()
{
    UShopManager* ShopManager = GetGameInstance()->GetSubsystem<UShopManager>();
    if (ShopManager)
    {
        ShopManager->UpdatePrices();
        UE_LOG(LogTemp, Log, TEXT("Shop prices updated"));
    }
}
```

## セーブ/ロードシステム

```cpp
// MySaveGame.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ProductItem.h"
#include "MySaveGame.generated.h"

USTRUCT()
struct FInventorySaveData
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<FString, int32> Items; // ProductItem名 -> 数量

    UPROPERTY()
    float Currency = 0.0f;
};

UCLASS()
class UMySaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FInventorySaveData PlayerInventory;
};

// SaveGameHelper.cpp
void SavePlayerInventory(UInventoryComponent* Inventory)
{
    UMySaveGame* SaveGame = Cast<UMySaveGame>(
        UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass())
    );

    if (SaveGame && Inventory)
    {
        SaveGame->PlayerInventory.Currency = Inventory->GetCurrency();

        TArray<FProductInstance> Items = Inventory->GetAllItems();
        for (const FProductInstance& Item : Items)
        {
            if (Item.ProductData)
            {
                SaveGame->PlayerInventory.Items.Add(
                    Item.ProductData->GetName(),
                    Item.Quantity
                );
            }
        }

        UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("PlayerSave"), 0);
    }
}
```

## Blueprintでの使用例

### 商品購入フロー

1. Shop Manager Subsystemを取得
2. `Can Buy Product`で購入可能性をチェック
3. `Get Buy Price`で価格を表示
4. プレイヤーが確認したら`Buy Product`を呼び出し
5. `On Transaction Complete`イベントで結果を処理
6. UIを更新

### インベントリ表示

1. Inventory Componentから`Get All Items`を呼び出し
2. 各アイテムをリストウィジェットに表示
3. `On Inventory Changed`イベントでリアルタイム更新

これらのサンプルを参考に、プロジェクトに合わせてカスタマイズしてください。
