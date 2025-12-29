# 使用例

## 基本的なセットアップ

### ゲーム開始時の初期化

```cpp
void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    // 通貨の登録
    FCurrencyInfo GoldInfo(TEXT("Gold"), FText::FromString(TEXT("ゴールド")), 999999);
    GoldInfo.Description = FText::FromString(TEXT("基本通貨"));
    WalletManager->RegisterCurrency(GoldInfo);

    FCurrencyInfo GemInfo(TEXT("Gem"), FText::FromString(TEXT("ジェム")), 9999);
    GemInfo.Description = FText::FromString(TEXT("プレミアム通貨"));
    WalletManager->RegisterCurrency(GemInfo);

    FCurrencyInfo TokenInfo(TEXT("Token"), FText::FromString(TEXT("トークン")), 100);
    TokenInfo.Description = FText::FromString(TEXT("イベント通貨"));
    WalletManager->RegisterCurrency(TokenInfo);

    // 変換レートの設定
    WalletManager->SetConversionRate(TEXT("Gold"), TEXT("Gem"), 0.01f);  // 100ゴールド = 1ジェム
    WalletManager->SetConversionRate(TEXT("Gem"), TEXT("Token"), 2.0f);  // 1ジェム = 2トークン

    // 初期残高の付与
    WalletManager->SetCurrency(TEXT("Gold"), 1000, TEXT("初期付与"));
    WalletManager->SetCurrency(TEXT("Gem"), 50, TEXT("初期付与"));
}
```

## ショップシステム

### アイテム購入

```cpp
bool UShopSystem::PurchaseItem(const FString& ItemID, int32 Price)
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    // 残高確認
    if (!WalletManager->HasEnoughCurrency(TEXT("Gold"), Price))
    {
        UE_LOG(LogTemp, Warning, TEXT("残高不足: アイテム %s の購入に失敗"), *ItemID);
        return false;
    }

    // 購入処理
    FString Reason = FString::Printf(TEXT("アイテム購入: %s"), *ItemID);
    if (WalletManager->RemoveCurrency(TEXT("Gold"), Price, Reason))
    {
        // アイテムをインベントリに追加
        AddItemToInventory(ItemID);
        UE_LOG(LogTemp, Log, TEXT("アイテム %s を購入しました"), *ItemID);
        return true;
    }

    return false;
}
```

### プレミアムアイテム購入

```cpp
bool UShopSystem::PurchasePremiumItem(const FString& ItemID, int32 GemPrice)
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    if (WalletManager->HasEnoughCurrency(TEXT("Gem"), GemPrice))
    {
        FString Reason = FString::Printf(TEXT("プレミアムアイテム購入: %s"), *ItemID);
        if (WalletManager->RemoveCurrency(TEXT("Gem"), GemPrice, Reason))
        {
            AddItemToInventory(ItemID);
            return true;
        }
    }

    return false;
}
```

## 報酬システム

### クエスト報酬

```cpp
void UQuestSystem::CompleteQuest(const FString& QuestID)
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    // クエストデータから報酬情報を取得
    FQuestReward Reward = GetQuestReward(QuestID);

    // 報酬を付与
    FString Reason = FString::Printf(TEXT("クエスト報酬: %s"), *QuestID);
    WalletManager->AddCurrency(TEXT("Gold"), Reward.GoldAmount, Reason);

    if (Reward.GemAmount > 0)
    {
        WalletManager->AddCurrency(TEXT("Gem"), Reward.GemAmount, Reason);
    }

    UE_LOG(LogTemp, Log, TEXT("クエスト %s をクリアしました。報酬: Gold %d, Gem %d"),
        *QuestID, Reward.GoldAmount, Reward.GemAmount);
}
```

### デイリーログインボーナス

```cpp
void ULoginBonusSystem::GrantDailyBonus(int32 DayCount)
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    // 日数に応じて報酬を増加
    int64 GoldBonus = 100 * DayCount;
    int64 GemBonus = DayCount / 7; // 7日ごとに1ジェム

    WalletManager->AddCurrency(TEXT("Gold"), GoldBonus, TEXT("デイリーログインボーナス"));

    if (GemBonus > 0)
    {
        WalletManager->AddCurrency(TEXT("Gem"), GemBonus, TEXT("ウィークリーボーナス"));
    }
}
```

## 通貨変換システム

### 通貨交換所

```cpp
void UCurrencyExchangeUI::ExchangeGoldToGem(int64 GoldAmount)
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    // 変換後の量を計算
    int64 GemAmount = WalletManager->CalculateConversionAmount(TEXT("Gold"), TEXT("Gem"), GoldAmount);

    // 確認ダイアログを表示
    FString Message = FString::Printf(
        TEXT("%lld ゴールドを %lld ジェムに交換しますか？"),
        GoldAmount, GemAmount
    );

    // ユーザーが承認した場合
    if (ShowConfirmDialog(Message))
    {
        if (WalletManager->ConvertCurrency(TEXT("Gold"), TEXT("Gem"), GoldAmount))
        {
            UE_LOG(LogTemp, Log, TEXT("通貨を交換しました: %lld Gold -> %lld Gem"), GoldAmount, GemAmount);
            UpdateUI();
        }
    }
}
```

## イベント処理

### 残高変更時のUI更新

```cpp
void UWalletUI::Initialize()
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    // イベントをバインド
    WalletManager->OnCurrencyChanged.AddDynamic(this, &UWalletUI::OnCurrencyChangedHandler);
}

void UWalletUI::OnCurrencyChangedHandler(FName CurrencyID, int64 OldAmount, int64 NewAmount)
{
    // UIを更新
    if (CurrencyID == TEXT("Gold"))
    {
        UpdateGoldDisplay(NewAmount);

        // 増減をアニメーション表示
        int64 Delta = NewAmount - OldAmount;
        if (Delta > 0)
        {
            ShowGainAnimation(Delta);
        }
        else if (Delta < 0)
        {
            ShowLossAnimation(FMath::Abs(Delta));
        }
    }
}
```

### 取引履歴の表示

```cpp
void UTransactionHistoryUI::RefreshHistory()
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    // 最新10件の取引を取得
    TArray<FTransactionRecord> History = WalletManager->GetTransactionHistory(TEXT("Gold"), 10);

    // リストをクリア
    TransactionListWidget->ClearChildren();

    // 各取引をUIに追加
    for (const FTransactionRecord& Transaction : History)
    {
        UTransactionEntryWidget* EntryWidget = CreateWidget<UTransactionEntryWidget>(this, TransactionEntryClass);

        EntryWidget->SetTransactionType(Transaction.TransactionType);
        EntryWidget->SetAmount(Transaction.Amount);
        EntryWidget->SetTimestamp(Transaction.Timestamp);
        EntryWidget->SetReason(Transaction.Reason);

        TransactionListWidget->AddChild(EntryWidget);
    }
}
```

## セーブ/ロードシステム

### ウォレット状態の保存

```cpp
void UMySaveGame::SaveWalletState()
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    // すべての通貨残高を保存
    TArray<FWalletCurrency> AllCurrencies = WalletManager->GetAllCurrencies();

    SavedCurrencies.Empty();
    for (const FWalletCurrency& Currency : AllCurrencies)
    {
        SavedCurrencies.Add(Currency);
    }
}

void UMySaveGame::LoadWalletState()
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    // 保存された残高を復元
    for (const FWalletCurrency& Currency : SavedCurrencies)
    {
        WalletManager->SetCurrency(Currency.CurrencyID, Currency.Amount, TEXT("セーブデータから復元"));
    }
}
```

## 統計・分析

### 通貨使用統計

```cpp
void UAnalyticsSystem::GenerateCurrencyReport()
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    // 全取引履歴を取得
    TArray<FTransactionRecord> AllHistory = WalletManager->GetAllTransactionHistory(1000);

    int64 TotalEarned = 0;
    int64 TotalSpent = 0;

    for (const FTransactionRecord& Transaction : AllHistory)
    {
        if (Transaction.CurrencyID == TEXT("Gold"))
        {
            if (Transaction.TransactionType == ETransactionType::Earn ||
                Transaction.TransactionType == ETransactionType::Grant)
            {
                TotalEarned += Transaction.Amount;
            }
            else if (Transaction.TransactionType == ETransactionType::Spend ||
                     Transaction.TransactionType == ETransactionType::Remove)
            {
                TotalSpent += Transaction.Amount;
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("ゴールド統計: 獲得 %lld, 消費 %lld, 純利益 %lld"),
        TotalEarned, TotalSpent, TotalEarned - TotalSpent);
}
```

## エラーハンドリング

### 安全な通貨操作

```cpp
bool USafeWalletOperations::SafeRemoveCurrency(FName CurrencyID, int64 Amount, const FString& Reason)
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    // 通貨が登録されているか確認
    if (!WalletManager->IsCurrencyRegistered(CurrencyID))
    {
        UE_LOG(LogTemp, Error, TEXT("通貨 %s は登録されていません"), *CurrencyID.ToString());
        return false;
    }

    // 十分な残高があるか確認
    if (!WalletManager->HasEnoughCurrency(CurrencyID, Amount))
    {
        int64 CurrentAmount = WalletManager->GetCurrencyAmount(CurrencyID);
        UE_LOG(LogTemp, Warning, TEXT("残高不足: 必要 %lld, 保有 %lld"), Amount, CurrentAmount);
        return false;
    }

    // 削除実行
    return WalletManager->RemoveCurrency(CurrencyID, Amount, Reason);
}
```
