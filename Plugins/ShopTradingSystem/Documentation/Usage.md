# 使用方法

## インベントリシステム

### プレイヤーインベントリの設定

1. プレイヤーキャラクターのBlueprintを開く
2. `Add Component` → `Inventory Component` を追加
3. 初期通貨を設定（デフォルト: 0）

### アイテムの追加/削除

**C++:**
```cpp
UInventoryComponent* Inventory = PlayerCharacter->FindComponentByClass<UInventoryComponent>();

// アイテムを追加
bool bAdded = Inventory->AddItem(ProductAsset, 5);

// アイテムを削除
bool bRemoved = Inventory->RemoveItem(ProductAsset, 2);

// アイテムの数量を確認
int32 Quantity = Inventory->GetItemQuantity(ProductAsset);
```

**Blueprint:**
- `Add Item` ノードを使用してアイテムを追加
- `Remove Item` ノードを使用してアイテムを削除
- `Get Item Quantity` ノードを使用して数量を取得

### 通貨管理

**C++:**
```cpp
// 通貨を追加
Inventory->AddCurrency(1000.0f);

// 通貨を削除
Inventory->RemoveCurrency(500.0f);

// 通貨を確認
float CurrentCurrency = Inventory->GetCurrency();

// 十分な通貨があるか確認
bool bHasEnough = Inventory->HasEnoughCurrency(250.0f);
```

## ショップシステム

### ショップマネージャーの取得

**C++:**
```cpp
UShopManager* ShopManager = GetGameInstance()->GetSubsystem<UShopManager>();
```

**Blueprint:**
- `Get Game Instance` → `Get Subsystem` → `Shop Manager` を選択

### 商品をショップに追加

**C++:**
```cpp
// 在庫10個で商品を追加
ShopManager->AddProductToShop(ProductAsset, 10, false);

// 無制限在庫で商品を追加
ShopManager->AddProductToShop(ProductAsset, 0, true);
```

**Blueprint:**
- `Add Product To Shop` ノードを使用

### 商品の購入

**C++:**
```cpp
UInventoryComponent* PlayerInventory = /* プレイヤーのインベントリ */;

// 購入可能か確認
if (ShopManager->CanBuyProduct(PlayerInventory, ProductAsset, 1))
{
    // 購入を実行
    bool bSuccess = ShopManager->BuyProduct(PlayerInventory, ProductAsset, 1);
}
```

**Blueprint:**
1. `Can Buy Product` で購入可能か確認
2. `Buy Product` で購入を実行
3. `On Transaction Complete` イベントで結果を処理

### 商品の売却

**C++:**
```cpp
// 売却可能か確認
if (ShopManager->CanSellProduct(PlayerInventory, ProductAsset, 1))
{
    // 売却を実行
    bool bSuccess = ShopManager->SellProduct(PlayerInventory, ProductAsset, 1);
}
```

## 価格変動システム

### 価格変動の設定

**C++:**
```cpp
FPriceFluctuationConfig Config;
Config.FluctuationMode = EPriceFluctuationMode::SupplyDemand;
Config.MinPriceMultiplier = 0.7f;
Config.MaxPriceMultiplier = 1.5f;
Config.bAffectsByDemand = true;
Config.DemandInfluence = 0.4f;

ShopManager->SetPriceFluctuationConfig(Config);
```

**Blueprint:**
1. `Make PriceFluctuationConfig` 構造体を作成
2. パラメータを設定
3. `Set Price Fluctuation Config` ノードで適用

### 価格の更新

**C++:**
```cpp
// すべての商品の価格を更新
ShopManager->UpdatePrices();

// 特定の商品の現在価格を取得
float CurrentPrice = ShopManager->GetCurrentProductPrice(ProductAsset);
```

### 価格変動モード

- **None**: 価格変動なし（基本価格のまま）
- **Random**: ランダムな価格変動
- **SupplyDemand**: 需要と供給に基づく変動
- **TimeBased**: 時間に基づく変動（日内変動など）
- **EventBased**: イベントベースの変動（カスタム実装）

## イベントハンドリング

### トランザクションイベント

**Blueprint:**
```
Shop Manager → On Transaction Complete イベントを追加

Success ブランチで:
- 購入/売却成功時の処理
- UIの更新
- サウンドの再生など
```

### インベントリ変更イベント

**Blueprint:**
```
Inventory Component → On Inventory Changed イベントを追加

イベントで:
- UIのアイテムリストを更新
- 通知を表示
```

## ベストプラクティス

1. **初期化**: Game Instanceの初期化時にショップの商品ラインナップを設定
2. **セーブ/ロード**: インベントリコンポーネントの`Items`と`Currency`はSaveGame対応
3. **エラーハンドリング**: トランザクションの結果を常に確認
4. **パフォーマンス**: 大量の商品を扱う場合は、価格更新の頻度を調整
5. **バリデーション**: カスタムロジックを追加する場合は、`CanBuyProduct`/`CanSellProduct`をオーバーライド
