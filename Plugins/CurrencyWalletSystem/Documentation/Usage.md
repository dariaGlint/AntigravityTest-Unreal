# 使用ガイドとベストプラクティス

## アーキテクチャ

### GameInstanceSubsystemの利用

WalletManagerは `UGameInstanceSubsystem` として実装されています。これにより：
- ゲームセッション全体で単一のインスタンスが保証される
- レベル遷移時にも状態が維持される
- どこからでも簡単にアクセス可能

```cpp
UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();
```

## ベストプラクティス

### 1. 通貨の登録タイミング

ゲーム開始時（GameModeのBeginPlayなど）に、使用するすべての通貨を登録することを推奨します。

```cpp
void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    // すべての通貨を一度に登録
    RegisterAllCurrencies(WalletManager);
}
```

### 2. 通貨IDの管理

通貨IDは文字列リテラルではなく、定数や列挙型で管理することを推奨します。

```cpp
// ヘッダーファイル
namespace CurrencyIDs
{
    const FName Gold = TEXT("Gold");
    const FName Gem = TEXT("Gem");
    const FName Token = TEXT("Token");
}

// 使用例
WalletManager->AddCurrency(CurrencyIDs::Gold, 100);
```

または、Blueprint用にはData Assetを使用：

```cpp
UCLASS()
class UGameCurrencyData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FCurrencyInfo> Currencies;
};
```

### 3. エラーハンドリング

すべての通貨操作は戻り値を確認してください。

```cpp
// 良い例
if (WalletManager->RemoveCurrency(TEXT("Gold"), Price, TEXT("購入")))
{
    // 購入成功
    GiveItemToPlayer();
}
else
{
    // 購入失敗 - エラーメッセージを表示
    ShowInsufficientFundsMessage();
}

// 悪い例
WalletManager->RemoveCurrency(TEXT("Gold"), Price, TEXT("購入"));
GiveItemToPlayer(); // 失敗してもアイテムを付与してしまう
```

### 4. 取引の理由を記録

すべての取引に理由を付けることで、デバッグや分析が容易になります。

```cpp
// 良い例
WalletManager->AddCurrency(
    CurrencyIDs::Gold,
    RewardAmount,
    FString::Printf(TEXT("Quest: %s - Level: %d"), *QuestName, QuestLevel)
);

// 悪い例
WalletManager->AddCurrency(CurrencyIDs::Gold, RewardAmount); // 理由が不明
```

### 5. イベントの活用

UIの更新にはイベントを活用してください。

```cpp
// UIクラスの初期化
void UWalletUI::NativeConstruct()
{
    Super::NativeConstruct();

    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();
    WalletManager->OnCurrencyChanged.AddDynamic(this, &UWalletUI::UpdateDisplay);

    // 初期表示
    UpdateDisplay(NAME_None, 0, 0);
}

void UWalletUI::UpdateDisplay(FName CurrencyID, int64 OldAmount, int64 NewAmount)
{
    // すべての通貨を再表示
    GoldText->SetText(FText::AsNumber(WalletManager->GetCurrencyAmount(CurrencyIDs::Gold)));
    GemText->SetText(FText::AsNumber(WalletManager->GetCurrencyAmount(CurrencyIDs::Gem)));
}
```

### 6. 上限値の設定

各通貨には適切な上限値を設定してください。

```cpp
// ゲーム内通貨（大量に獲得可能）
FCurrencyInfo GoldInfo(TEXT("Gold"), FText::FromString(TEXT("ゴールド")), 99999999);

// プレミアム通貨（課金など、慎重に管理）
FCurrencyInfo GemInfo(TEXT("Gem"), FText::FromString(TEXT("ジェム")), 9999);

// イベント限定通貨（期間限定）
FCurrencyInfo EventTokenInfo(TEXT("EventToken"), FText::FromString(TEXT("イベントトークン")), 100);
```

### 7. 通貨変換レートの設計

変換レートは慎重に設定してください。

```cpp
// 一方向の変換のみ許可する場合
WalletManager->SetConversionRate(TEXT("Gold"), TEXT("Gem"), 0.01f); // 100Gold = 1Gem
// 逆方向は設定しない（Gem -> Goldの変換を防ぐ）

// 双方向変換を許可する場合（異なるレートも可能）
WalletManager->SetConversionRate(TEXT("Gold"), TEXT("Gem"), 0.01f);  // 100Gold = 1Gem
WalletManager->SetConversionRate(TEXT("Gem"), TEXT("Gold"), 80.0f);  // 1Gem = 80Gold (手数料20%)
```

## パフォーマンス最適化

### 取引履歴のサイズ管理

デフォルトでは1000件の取引履歴が保存されます。必要に応じて調整してください。

```cpp
// C++で設定
WalletManager->MaxTransactionHistorySize = 500;

// または、Config/DefaultGame.iniで設定
[/Script/CurrencyWalletSystem.WalletManager]
MaxTransactionHistorySize=500
```

### 頻繁な残高確認の最適化

残高を頻繁に確認する場合は、キャッシュを検討してください。

```cpp
// UIクラスで残高をキャッシュ
UPROPERTY()
int64 CachedGoldAmount;

void UWalletUI::OnCurrencyChangedHandler(FName CurrencyID, int64 OldAmount, int64 NewAmount)
{
    if (CurrencyID == CurrencyIDs::Gold)
    {
        CachedGoldAmount = NewAmount;
        UpdateGoldDisplay();
    }
}
```

## セキュリティと不正対策

### サーバー側の検証

マルチプレイヤーゲームでは、重要な通貨操作はサーバー側で検証してください。

```cpp
// クライアント側
void UShopSystem::RequestPurchase_Client(const FString& ItemID)
{
    // サーバーにリクエストを送信
    ServerPurchaseItem(ItemID);
}

// サーバー側
bool UShopSystem::ServerPurchaseItem_Validate(const FString& ItemID)
{
    return true;
}

void UShopSystem::ServerPurchaseItem_Implementation(const FString& ItemID)
{
    // サーバー側でのみ通貨を減少
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    int32 Price = GetItemPrice(ItemID);
    if (WalletManager->RemoveCurrency(CurrencyIDs::Gold, Price, TEXT("Purchase: ") + ItemID))
    {
        // 購入成功 - クライアントに通知
        ClientOnPurchaseSuccess(ItemID);
    }
}
```

### 取引ログの活用

不正検出のために取引履歴を定期的にログに出力してください。

```cpp
void UAntiCheatSystem::LogSuspiciousActivity()
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

    TArray<FTransactionRecord> RecentTransactions = WalletManager->GetAllTransactionHistory(100);

    for (const FTransactionRecord& Transaction : RecentTransactions)
    {
        if (Transaction.Amount > SuspiciousThreshold)
        {
            UE_LOG(LogTemp, Warning, TEXT("Suspicious transaction: %s, Amount: %lld, Reason: %s"),
                *Transaction.CurrencyID.ToString(), Transaction.Amount, *Transaction.Reason);
        }
    }
}
```

## デバッグとテスト

### コンソールコマンドの追加

デバッグ用のコンソールコマンドを追加すると便利です。

```cpp
// GameMode.hに追加
UFUNCTION(Exec)
void AddGold(int64 Amount);

UFUNCTION(Exec)
void SetGold(int64 Amount);

UFUNCTION(Exec)
void ShowWalletInfo();

// GameMode.cppに実装
void AMyGameMode::AddGold(int64 Amount)
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();
    WalletManager->AddCurrency(CurrencyIDs::Gold, Amount, TEXT("Debug Command"));
}

void AMyGameMode::ShowWalletInfo()
{
    UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();
    TArray<FWalletCurrency> AllCurrencies = WalletManager->GetAllCurrencies();

    for (const FWalletCurrency& Currency : AllCurrencies)
    {
        UE_LOG(LogTemp, Log, TEXT("%s: %lld"), *Currency.CurrencyID.ToString(), Currency.Amount);
    }
}
```

使用方法：
```
~ (コンソールを開く)
AddGold 1000
SetGold 5000
ShowWalletInfo
```

## トラブルシューティング

### 通貨が登録されていない

```cpp
if (!WalletManager->IsCurrencyRegistered(CurrencyID))
{
    UE_LOG(LogTemp, Error, TEXT("Currency %s is not registered!"), *CurrencyID.ToString());
    // 通貨を登録
    RegisterCurrency(CurrencyID);
}
```

### 残高が保存されない

GameInstanceSubsystemは自動的には保存されません。SaveGameシステムを使用してください。

### イベントが呼ばれない

イベントのバインドタイミングを確認してください。WalletManagerの初期化後にバインドする必要があります。

## まとめ

- 通貨は起動時に一括登録
- 通貨IDは定数で管理
- すべての操作で戻り値を確認
- 取引には理由を記録
- UIはイベント駆動で更新
- マルチプレイヤーではサーバー検証を実装
- デバッグ用のコンソールコマンドを追加
