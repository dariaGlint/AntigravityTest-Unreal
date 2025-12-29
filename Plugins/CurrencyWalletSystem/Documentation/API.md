# API リファレンス

## UWalletManager

GameInstanceSubsystemとして実装されたウォレット管理システムのメインクラス。

### 通貨登録API

#### RegisterCurrency
```cpp
void RegisterCurrency(const FCurrencyInfo& CurrencyInfo)
```
新しい通貨を登録します。

**パラメータ:**
- `CurrencyInfo`: 通貨情報（ID、表示名、上限値など）

**使用例:**
```cpp
FCurrencyInfo GoldInfo(TEXT("Gold"), FText::FromString(TEXT("ゴールド")), 999999);
WalletManager->RegisterCurrency(GoldInfo);
```

#### UnregisterCurrency
```cpp
void UnregisterCurrency(FName CurrencyID)
```
登録された通貨を削除します。

**パラメータ:**
- `CurrencyID`: 通貨ID

#### IsCurrencyRegistered
```cpp
bool IsCurrencyRegistered(FName CurrencyID) const
```
指定した通貨が登録されているか確認します。

**戻り値:** 登録されていればtrue

#### GetCurrencyInfo
```cpp
FCurrencyInfo GetCurrencyInfo(FName CurrencyID) const
```
通貨の情報を取得します。

**戻り値:** 通貨情報構造体

#### GetAllRegisteredCurrencies
```cpp
TArray<FCurrencyInfo> GetAllRegisteredCurrencies() const
```
すべての登録済み通貨情報を取得します。

**戻り値:** 通貨情報の配列

### ウォレット操作API

#### AddCurrency
```cpp
bool AddCurrency(FName CurrencyID, int64 Amount, const FString& Reason = TEXT(""))
```
通貨を追加します。上限値を超える場合は上限値に制限されます。

**パラメータ:**
- `CurrencyID`: 通貨ID
- `Amount`: 追加する量
- `Reason`: 取引の理由（任意）

**戻り値:** 成功した場合true

#### RemoveCurrency
```cpp
bool RemoveCurrency(FName CurrencyID, int64 Amount, const FString& Reason = TEXT(""))
```
通貨を削除します。残高不足の場合は失敗します。

**パラメータ:**
- `CurrencyID`: 通貨ID
- `Amount`: 削除する量
- `Reason`: 取引の理由（任意）

**戻り値:** 成功した場合true

#### SetCurrency
```cpp
bool SetCurrency(FName CurrencyID, int64 Amount, const FString& Reason = TEXT(""))
```
通貨の残高を直接設定します。

**パラメータ:**
- `CurrencyID`: 通貨ID
- `Amount`: 設定する量
- `Reason`: 取引の理由（任意）

**戻り値:** 成功した場合true

#### GetCurrencyAmount
```cpp
int64 GetCurrencyAmount(FName CurrencyID) const
```
指定した通貨の現在の残高を取得します。

**戻り値:** 残高（登録されていない場合は0）

#### HasEnoughCurrency
```cpp
bool HasEnoughCurrency(FName CurrencyID, int64 Amount) const
```
指定した量の通貨があるか確認します。

**戻り値:** 十分な残高があればtrue

#### GetAllCurrencies
```cpp
TArray<FWalletCurrency> GetAllCurrencies() const
```
すべての通貨の残高を取得します。

**戻り値:** 通貨と残高のペアの配列

### 通貨変換API

#### SetConversionRate
```cpp
void SetConversionRate(FName FromCurrencyID, FName ToCurrencyID, float Rate)
```
2つの通貨間の変換レートを設定します。

**パラメータ:**
- `FromCurrencyID`: 変換元の通貨ID
- `ToCurrencyID`: 変換先の通貨ID
- `Rate`: 変換レート（1.0 = 1:1）

**使用例:**
```cpp
// 10ゴールド = 1ジェム
WalletManager->SetConversionRate(TEXT("Gold"), TEXT("Gem"), 0.1f);
```

#### GetConversionRate
```cpp
float GetConversionRate(FName FromCurrencyID, FName ToCurrencyID) const
```
2つの通貨間の変換レートを取得します。

**戻り値:** 変換レート（設定されていない場合は0.0）

#### ConvertCurrency
```cpp
bool ConvertCurrency(FName FromCurrencyID, FName ToCurrencyID, int64 Amount)
```
通貨を変換します。変換元の通貨が減少し、変換先の通貨が増加します。

**パラメータ:**
- `FromCurrencyID`: 変換元の通貨ID
- `ToCurrencyID`: 変換先の通貨ID
- `Amount`: 変換する量

**戻り値:** 成功した場合true

#### CalculateConversionAmount
```cpp
int64 CalculateConversionAmount(FName FromCurrencyID, FName ToCurrencyID, int64 Amount) const
```
変換後の量を計算します（実際には変換しません）。

**戻り値:** 変換後の量

### 取引履歴API

#### GetTransactionHistory
```cpp
TArray<FTransactionRecord> GetTransactionHistory(FName CurrencyID, int32 MaxRecords = 100) const
```
指定した通貨の取引履歴を取得します（新しい順）。

**パラメータ:**
- `CurrencyID`: 通貨ID
- `MaxRecords`: 取得する最大レコード数

**戻り値:** 取引履歴の配列

#### GetAllTransactionHistory
```cpp
TArray<FTransactionRecord> GetAllTransactionHistory(int32 MaxRecords = 100) const
```
すべての通貨の取引履歴を取得します（新しい順）。

**パラメータ:**
- `MaxRecords`: 取得する最大レコード数

**戻り値:** 取引履歴の配列

#### ClearTransactionHistory
```cpp
void ClearTransactionHistory(FName CurrencyID)
```
指定した通貨の取引履歴をクリアします。

#### ClearAllTransactionHistory
```cpp
void ClearAllTransactionHistory()
```
すべての取引履歴をクリアします。

#### GetTransactionCount
```cpp
int32 GetTransactionCount(FName CurrencyID) const
```
指定した通貨の取引履歴の数を取得します。

**戻り値:** 取引履歴の数

### イベント

#### OnCurrencyChanged
```cpp
FOnCurrencyChanged OnCurrencyChanged
```
通貨の残高が変更された時に呼ばれるイベント。

**パラメータ:**
- `CurrencyID`: 変更された通貨のID
- `OldAmount`: 変更前の量
- `NewAmount`: 変更後の量

#### OnTransactionRecorded
```cpp
FOnTransactionRecorded OnTransactionRecorded
```
取引が記録された時に呼ばれるイベント。

**パラメータ:**
- `Transaction`: 記録された取引情報

#### OnCurrencyConverted
```cpp
FOnCurrencyConverted OnCurrencyConverted
```
通貨が変換された時に呼ばれるイベント。

**パラメータ:**
- `FromCurrencyID`: 変換元の通貨ID
- `ToCurrencyID`: 変換先の通貨ID

## データ構造

### FCurrencyInfo
通貨の情報を保持する構造体。

```cpp
struct FCurrencyInfo
{
    FName CurrencyID;        // 通貨ID
    FText DisplayName;       // 表示名
    FText Description;       // 説明
    int64 MaxAmount;         // 最大値
    UTexture2D* Icon;        // アイコン
}
```

### FWalletCurrency
ウォレット内の通貨と残高を表す構造体。

```cpp
struct FWalletCurrency
{
    FName CurrencyID;        // 通貨ID
    int64 Amount;            // 残高
}
```

### FTransactionRecord
取引履歴のレコード。

```cpp
struct FTransactionRecord
{
    FName CurrencyID;              // 通貨ID
    ETransactionType TransactionType; // 取引タイプ
    int64 Amount;                  // 取引量
    int64 BalanceAfter;            // 取引後の残高
    FDateTime Timestamp;           // タイムスタンプ
    FString Reason;                // 理由
}
```

### ETransactionType
取引タイプの列挙型。

```cpp
enum class ETransactionType : uint8
{
    Earn,      // 獲得
    Spend,     // 消費
    Convert,   // 変換
    Grant,     // 付与
    Remove     // 削除
}
```

### FCurrencyConversionRate
通貨変換レート。

```cpp
struct FCurrencyConversionRate
{
    FName FromCurrencyID;    // 変換元通貨ID
    FName ToCurrencyID;      // 変換先通貨ID
    float Rate;              // 変換レート
}
```
