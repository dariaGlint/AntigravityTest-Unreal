# Gimmick System Plugin

## 概要 / Overview

インタラクション可能オブジェクトの基底クラスとギミック管理システムを提供するUnreal Engine 5プラグインです。

A flexible gimmick and interactive object system for Unreal Engine 5 with support for doors, locks, puzzles, and state management.

## 主な機能 / Features

### 基底クラス / Base Classes

- **AInteractableBase** - すべてのインタラクション可能オブジェクトの基底クラス
  - インタラクション条件チェック
  - 範囲チェック
  - 使用回数制限
  - デリゲートによるイベント通知

- **UGimmickConditionBase** - インタラクション条件の基底クラス
  - カスタム条件の作成が可能
  - Blueprintで拡張可能

### ドアシステム / Door System

- **ADoorGimmick** - ドアギミッククラス
  - 状態管理（開/閉/施錠/開中/閉中）
  - 自動で閉じる機能
  - トグル/開くのみの動作モード
  - アニメーション連携（Blueprintイベント）

### パズルシステム / Puzzle System

- **APuzzleBase** - パズルの基底クラス
  - 解決状態の管理
  - リセット機能
  - カスタム検証ロジック

### 条件システム / Condition System

- **UKeyCondition** - 鍵所持チェック条件
  - Gameplay Tagを使用
  - インベントリインターフェース連携

- **UPuzzleCondition** - パズル解決チェック条件
  - 特定のパズルが解かれているかチェック

### 管理システム / Management System

- **UGimmickManager** - ギミック管理サブシステム
  - すべてのギミックを一元管理
  - ID検索
  - 最寄りのインタラクション可能オブジェクト検索
  - セーブ/ロード機能の準備

## 使用方法 / Usage

### 基本的なインタラクション

1. `AInteractableBase`を継承したクラスを作成
2. `OnInteract_Implementation`をオーバーライド
3. インタラクション処理を実装

### ドアの作成

```cpp
// C++での使用例
ADoorGimmick* Door = GetWorld()->SpawnActor<ADoorGimmick>();
Door->SetDoorState(EDoorState::Locked);

// 鍵条件を追加
UKeyCondition* KeyCondition = NewObject<UKeyCondition>(Door);
KeyCondition->RequiredKeyTag = FGameplayTag::RequestGameplayTag("Item.Key.RedKey");
Door->AddCondition(KeyCondition);
```

Blueprintでも完全に使用可能です。

### インベントリ連携

キャラクターやプレイヤーコントローラーに`IInventoryInterface`を実装することで、鍵条件システムと連携できます。

```cpp
class AMyCharacter : public ACharacter, public IInventoryInterface
{
    virtual bool HasItemWithTag_Implementation(const FGameplayTag& Tag) const override;
};
```

## アーキテクチャ / Architecture

```
AInteractableBase (基底)
├── ADoorGimmick (ドア)
├── APuzzleBase (パズル)
└── (カスタム実装)

UGimmickConditionBase (条件基底)
├── UKeyCondition (鍵チェック)
├── UPuzzleCondition (パズルチェック)
└── (カスタム条件)

UGimmickManager (管理システム)
└── WorldSubsystem
```

## Blueprint連携 / Blueprint Integration

すべてのクラスはBlueprintable/BlueprintNativeEventに対応しており、C++とBlueprintを組み合わせた開発が可能です。

- インタラクション処理
- アニメーション再生
- カスタム条件チェック
- UIとの連携

## 今後の拡張 / Future Extensions

- セーブシステムとの完全統合
- ネットワークレプリケーション対応
- より多くの条件タイプ（レベル、ステータス等）
- ギミックチェーンシステム（複数のギミックを連携）

## ライセンス / License

このプラグインはプロジェクトの一部として提供されます。
