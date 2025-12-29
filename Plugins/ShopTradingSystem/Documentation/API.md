# API リファレンス

## UProductItem

商品データを定義するデータアセット。

### プロパティ

| 名前 | 型 | 説明 |
|------|-----|------|
| ProductName | FText | 商品名 |
| Description | FText | 商品の説明 |
| Category | EProductCategory | 商品カテゴリ |
| Rarity | EProductRarity | レアリティ |
| Icon | UTexture2D* | アイコン画像 |
| MaxStackSize | int32 | 最大スタック数（デフォルト: 99） |
| BasePrice | float | 基本価格（デフォルト: 100.0） |
| SellPriceMultiplier | float | 売却価格倍率（デフォルト: 0.5） |
| bCanBuy | bool | 購入可能か（デフォルト: true） |
| bCanSell | bool | 売却可能か（デフォルト: true） |
| bIsUnlimited | bool | 在庫無制限か（デフォルト: false） |

### 列挙型

**EProductCategory:**
- Consumable: 消耗品
- Equipment: 装備品
- Material: 素材
- Quest: クエストアイテム
- Misc: その他

**EProductRarity:**
- Common: コモン
- Uncommon: アンコモン
- Rare: レア
- Epic: エピック
- Legendary: レジェンダリー

---

## UInventoryComponent

インベントリを管理するコンポーネント。

### 主要メソッド

#### AddItem
```cpp
bool AddItem(UProductItem* Product, int32 Quantity = 1)
```
インベントリにアイテムを追加します。

**パラメータ:**
- `Product`: 追加する商品
- `Quantity`: 追加する数量

**戻り値:** 成功したらtrue

#### RemoveItem
```cpp
bool RemoveItem(UProductItem* Product, int32 Quantity = 1)
```
インベントリからアイテムを削除します。

#### HasItem
```cpp
bool HasItem(UProductItem* Product, int32 Quantity = 1) const
```
指定した商品を指定数量持っているか確認します。

#### GetItemQuantity
```cpp
int32 GetItemQuantity(UProductItem* Product) const
```
指定した商品の所持数を取得します。

#### AddCurrency / RemoveCurrency
```cpp
bool AddCurrency(float Amount)
bool RemoveCurrency(float Amount)
```
通貨を追加/削除します。

#### GetCurrency
```cpp
float GetCurrency() const
```
現在の通貨量を取得します。

### イベント

#### OnInventoryChanged
```cpp
FOnInventoryChanged OnInventoryChanged
```
インベントリが変更されたときに呼ばれます。

**パラメータ:**
- `Product`: 変更された商品
- `NewQuantity`: 新しい数量

#### OnCurrencyChanged
```cpp
FOnCurrencyChanged OnCurrencyChanged
```
通貨が変更されたときに呼ばれます。

**パラメータ:**
- `NewAmount`: 新しい通貨量

---

## UShopManager

ショップシステムを管理するGame Instanceサブシステム。

### ショップインベントリ管理

#### AddProductToShop
```cpp
void AddProductToShop(UProductItem* Product, int32 InitialStock, bool bIsUnlimited = false)
```
ショップに商品を追加します。

**パラメータ:**
- `Product`: 追加する商品
- `InitialStock`: 初期在庫数
- `bIsUnlimited`: 在庫無制限フラグ

#### RemoveProductFromShop
```cpp
void RemoveProductFromShop(UProductItem* Product)
```
ショップから商品を削除します。

#### GetAvailableProducts
```cpp
TArray<FShopProductEntry> GetAvailableProducts() const
```
購入可能な商品のリストを取得します。

### トランザクション

#### BuyProduct
```cpp
bool BuyProduct(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity)
```
商品を購入します。

**パラメータ:**
- `PlayerInventory`: プレイヤーのインベントリコンポーネント
- `Product`: 購入する商品
- `Quantity`: 購入数量

**戻り値:** 成功したらtrue

#### SellProduct
```cpp
bool SellProduct(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity)
```
商品を売却します。

#### CanBuyProduct / CanSellProduct
```cpp
bool CanBuyProduct(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity) const
bool CanSellProduct(UInventoryComponent* PlayerInventory, UProductItem* Product, int32 Quantity) const
```
購入/売却が可能か確認します。

#### GetBuyPrice / GetSellPrice
```cpp
float GetBuyPrice(UProductItem* Product, int32 Quantity) const
float GetSellPrice(UProductItem* Product, int32 Quantity) const
```
購入/売却価格を計算します。

### 価格変動

#### SetPriceFluctuationConfig
```cpp
void SetPriceFluctuationConfig(const FPriceFluctuationConfig& NewConfig)
```
価格変動の設定を更新します。

#### UpdatePrices
```cpp
void UpdatePrices()
```
すべての商品の価格を更新します。

#### GetCurrentProductPrice
```cpp
float GetCurrentProductPrice(UProductItem* Product) const
```
現在の商品価格を取得します。

### イベント

#### OnTransactionComplete
```cpp
FOnTransactionComplete OnTransactionComplete
```
トランザクションが完了したときに呼ばれます。

**パラメータ:**
- `bSuccess`: 成功したか
- `Product`: 対象の商品
- `Quantity`: 数量

#### OnShopInventoryChanged
```cpp
FOnShopInventoryChanged OnShopInventoryChanged
```
ショップの在庫が変更されたときに呼ばれます。

---

## UPriceFluctuationSystem

価格変動を管理するシステム。

### 主要メソッド

#### CalculateCurrentPrice
```cpp
float CalculateCurrentPrice(UProductItem* Product, const FPriceFluctuationConfig& Config)
```
設定に基づいて現在の価格を計算します。

#### RegisterProductSale / RegisterProductPurchase
```cpp
void RegisterProductSale(UProductItem* Product, int32 Quantity)
void RegisterProductPurchase(UProductItem* Product, int32 Quantity)
```
商品の売却/購入を記録します（需要データの更新用）。

---

## FPriceFluctuationConfig

価格変動の設定構造体。

### プロパティ

| 名前 | 型 | 説明 |
|------|-----|------|
| FluctuationMode | EPriceFluctuationMode | 変動モード |
| MinPriceMultiplier | float | 最小価格倍率（0.1-2.0） |
| MaxPriceMultiplier | float | 最大価格倍率（1.0-10.0） |
| FluctuationInterval | float | 変動更新間隔（秒） |
| bAffectsByDemand | bool | 需要の影響を受けるか |
| DemandInfluence | float | 需要の影響度（0.0-1.0） |

### 列挙型

**EPriceFluctuationMode:**
- None: 変動なし
- Random: ランダム変動
- SupplyDemand: 需要と供給
- TimeBased: 時間ベース
- EventBased: イベントベース
