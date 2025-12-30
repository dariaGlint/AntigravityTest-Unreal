# Crafting System Plugin

UE5用の柔軟なクラフティングシステムプラグインです。

## 概要

このプラグインは、以下の機能を提供します：
- レシピ管理
- 素材の組み合わせロジック
- アイテム生成
- 必要素材チェック

## 主要コンポーネント

### UCraftingRecipe
レシピのデータアセットクラスです。レシピの情報（必要素材、生成アイテム、クラフト時間など）を定義します。

### UCraftingManager
クラフティングシステムを管理するActorComponentです。プレイヤーやNPCにアタッチして使用します。

## 使用方法

### 1. レシピの作成

Content Browserで右クリック → Miscellaneous → Data Asset → CraftingRecipeを選択してレシピを作成します。

### 2. CraftingManagerの追加

Blueprintまたはアクターに`CraftingManager`コンポーネントを追加します。

### 3. レシピの登録

```cpp
// C++の場合
CraftingManager->RegisterRecipe(RecipeAsset);
```

Blueprintの場合は`Register Recipe`ノードを使用します。

### 4. 素材の追加

```cpp
// 素材を追加
CraftingManager->AddMaterial(FName("Wood"), 10);
CraftingManager->AddMaterial(FName("Stone"), 5);
```

### 5. クラフトの実行

```cpp
// クラフト可能かチェック
if (CraftingManager->CanCraftRecipe(FName("Sword_Recipe")))
{
    // クラフト開始
    CraftingManager->StartCrafting(FName("Sword_Recipe"));
}
```

### 6. クラフト完了イベントの処理

```cpp
// イベントをバインド
CraftingManager->OnCraftingCompleted.AddDynamic(this, &AMyActor::OnCraftingCompleted);

void AMyActor::OnCraftingCompleted(FName ItemID, int32 Amount)
{
    UE_LOG(LogTemp, Log, TEXT("Crafted %d of %s"), Amount, *ItemID.ToString());
}
```

## API リファレンス

詳細なAPIドキュメントは`Documentation/API.md`を参照してください。

## サンプル

使用例は`Documentation/Examples.md`を参照してください。
