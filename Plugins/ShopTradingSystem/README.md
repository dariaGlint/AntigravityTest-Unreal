# Shop Trading System

商品ラインナップ管理とショップシステムを提供するUnreal Engine 5プラグインです。

## 機能

### 1. 商品ラインナップ管理
- データアセットベースの商品定義
- カテゴリとレアリティによる分類
- カスタマイズ可能な商品属性

### 2. 在庫制御
- ショップ在庫の管理
- プレイヤーインベントリシステム
- 無制限在庫オプション
- スタック可能なアイテム

### 3. 価格変動ロジック
- 複数の価格変動モード:
  - ランダム変動
  - 需要と供給
  - 時間ベース
  - イベントベース
- カスタマイズ可能な価格範囲
- 需要による価格影響

### 4. 買取/売却処理
- プレイヤーからの購入処理
- プレイヤーへの売却処理
- トランザクションイベント
- 価格計算とバリデーション

## セットアップ

1. プラグインをプロジェクトの`Plugins`フォルダにコピー
2. プロジェクトを開き、プラグインを有効化
3. エディタを再起動

## 基本的な使用方法

### 商品データアセットの作成

1. コンテンツブラウザで右クリック
2. `Miscellaneous > Data Asset` を選択
3. `ProductItem` を選択
4. 商品の詳細情報を設定

### インベントリコンポーネントの追加

プレイヤーキャラクターやポーンに `InventoryComponent` を追加します。

```cpp
// C++の例
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
UInventoryComponent* InventoryComponent;
```

### ショップの初期化

Game Instanceから `ShopManager` サブシステムにアクセスし、商品を追加します。

```cpp
// C++の例
UShopManager* ShopManager = GetGameInstance()->GetSubsystem<UShopManager>();
ShopManager->AddProductToShop(ProductAsset, 10, false);
```

### 商品の購入

```cpp
// C++の例
UShopManager* ShopManager = GetGameInstance()->GetSubsystem<UShopManager>();
bool bSuccess = ShopManager->BuyProduct(PlayerInventory, ProductAsset, 1);
```

### 商品の売却

```cpp
// C++の例
UShopManager* ShopManager = GetGameInstance()->GetSubsystem<UShopManager>();
bool bSuccess = ShopManager->SellProduct(PlayerInventory, ProductAsset, 1);
```

## 主要なクラス

### UProductItem
商品データを定義するデータアセット

### UInventoryComponent
プレイヤーやNPCのインベントリを管理するコンポーネント

### UShopManager
ショップ操作、在庫、トランザクションを管理するGame Instanceサブシステム

### UPriceFluctuationSystem
動的な価格変動を管理するシステム

## Blueprint対応

すべての主要な機能はBlueprintから利用可能です。

## イベント

### OnTransactionComplete
トランザクションが完了したときに発火

### OnInventoryChanged
インベントリが変更されたときに発火

### OnCurrencyChanged
通貨が変更されたときに発火

### OnShopInventoryChanged
ショップの在庫が変更されたときに発火

## 詳細なドキュメント

- [使用方法](Documentation/Usage.md)
- [API リファレンス](Documentation/API.md)
- [サンプル](Documentation/Examples.md)

## ライセンス

このプラグインはプロジェクト内で自由に使用できます。
