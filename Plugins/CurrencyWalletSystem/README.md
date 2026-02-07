# Currency Wallet System

複数通貨対応のウォレット管理システムプラグインです。

## 概要

Currency Wallet Systemは、Unreal Engine 5向けの柔軟な経済システム実装プラグインです。複数の通貨を管理し、取引履歴の記録、上限管理、通貨間の変換などの機能を提供します。

## 主な機能

### 複数通貨対応
- 任意の数の通貨を登録・管理
- 各通貨に独自の表示名、説明、アイコン、上限値を設定可能
- ゲーム内で動的に通貨を追加・削除可能

### ウォレット管理
- 通貨の追加、削除、設定
- 残高照会
- 上限値の自動適用
- 残高変更時のイベント通知

### 取引履歴
- すべての取引を自動記録
- 取引タイプ（獲得、消費、変換、付与、削除）の区別
- タイムスタンプと理由の記録
- 履歴のフィルタリングと取得
- 履歴サイズの自動管理

### 通貨変換
- 通貨間の変換レート設定
- 変換計算と実行
- 変換時のイベント通知

## インストール

1. このプラグインを `Plugins/CurrencyWalletSystem` フォルダに配置
2. `.uproject` ファイルを右クリックし、「Generate Visual Studio project files」を選択
3. Unreal Editorでプロジェクトを開く
4. Edit > Plugins でプラグインが有効になっていることを確認

## 基本的な使用方法

### C++での使用

```cpp
#include "WalletManager.h"

// WalletManagerの取得
UWalletManager* WalletManager = GetGameInstance()->GetSubsystem<UWalletManager>();

// 通貨の登録
FCurrencyInfo GoldInfo(TEXT("Gold"), FText::FromString(TEXT("ゴールド")), 999999);
WalletManager->RegisterCurrency(GoldInfo);

FCurrencyInfo GemInfo(TEXT("Gem"), FText::FromString(TEXT("ジェム")), 9999);
WalletManager->RegisterCurrency(GemInfo);

// 通貨の追加
WalletManager->AddCurrency(TEXT("Gold"), 100, TEXT("クエスト報酬"));

// 通貨の消費
WalletManager->RemoveCurrency(TEXT("Gold"), 50, TEXT("アイテム購入"));

// 残高の確認
int64 GoldAmount = WalletManager->GetCurrencyAmount(TEXT("Gold"));

// 十分な残高があるか確認
bool bCanAfford = WalletManager->HasEnoughCurrency(TEXT("Gold"), 100);

// 通貨変換レートの設定
WalletManager->SetConversionRate(TEXT("Gold"), TEXT("Gem"), 0.1f); // 10ゴールド = 1ジェム

// 通貨の変換
WalletManager->ConvertCurrency(TEXT("Gold"), TEXT("Gem"), 100); // 100ゴールドを10ジェムに変換

// 取引履歴の取得
TArray<FTransactionRecord> History = WalletManager->GetTransactionHistory(TEXT("Gold"), 10);
```

### Blueprintでの使用

1. Get Game Instance ノードから Get Subsystem ノードを使用
2. Class に WalletManager を選択
3. Register Currency ノードで通貨を登録
4. Add Currency / Remove Currency ノードで残高を操作
5. Get Currency Amount ノードで残高を取得

## イベント

WalletManagerは以下のイベントを提供します：

- `OnCurrencyChanged`: 通貨の残高が変更された時
- `OnTransactionRecorded`: 取引が記録された時
- `OnCurrencyConverted`: 通貨が変換された時

これらのイベントをBindして、UIの更新やゲームロジックの実行に使用できます。

## 詳細ドキュメント

- [API リファレンス](Documentation/API.md)
- [使用例](Documentation/Examples.md)
- [ベストプラクティス](Documentation/Usage.md)

## ライセンス

このプラグインはプロジェクトの一部として自由に使用できます。
