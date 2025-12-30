# 使用ガイド

## セットアップ

### 1. プラグインの有効化

1. Unreal Editorを起動
2. Edit → Plugins を開く
3. "Crafting System" を検索
4. チェックボックスをオンにして有効化
5. エディタを再起動

### 2. プロジェクトへの統合

#### C++プロジェクトの場合

Build.csファイルに以下を追加：

```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core",
    "CoreUObject",
    "Engine",
    "CraftingSystem"  // 追加
});
```

#### Blueprintプロジェクトの場合

特別な設定は不要です。そのまま使用できます。

## 基本的なワークフロー

### ステップ1: レシピの作成

1. Content Browserで右クリック
2. **Miscellaneous → Data Asset → CraftingRecipe** を選択
3. レシピに名前を付ける（例：`DA_Recipe_IronSword`）
4. レシピを開いて設定：

#### レシピの設定例

```
Recipe ID: IronSword
Recipe Name: 鉄の剣
Description: 鉄と木材から作られる基本的な剣

Required Materials:
  [0]
    Material ID: Iron
    Required Amount: 5
  [1]
    Material ID: Wood
    Required Amount: 2

Result:
  Item ID: IronSword
  Amount: 1

Crafting Time: 3.0
Is Unlocked: true
```

### ステップ2: CraftingManagerの追加

#### Blueprintの場合

1. Character Blueprint または Actor Blueprint を開く
2. **Components パネル** で "Add Component" をクリック
3. **CraftingManager** を検索して追加

#### C++の場合

```cpp
// ヘッダーファイル
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting")
UCraftingManager* CraftingManager;

// コンストラクタ
CraftingManager = CreateDefaultSubobject<UCraftingManager>(TEXT("CraftingManager"));
```

### ステップ3: レシピの登録

#### Blueprintの場合

```
Event BeginPlay
  → Register Recipe
      Recipe: DA_Recipe_IronSword (作成したレシピアセット)
```

#### C++の場合

```cpp
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (IronSwordRecipe)
    {
        CraftingManager->RegisterRecipe(IronSwordRecipe);
    }
}
```

### ステップ4: 素材の管理

#### 素材の追加

```
// Blueprint
Add Material
  Material ID: Iron
  Amount: 10
```

```cpp
// C++
CraftingManager->AddMaterial(FName("Iron"), 10);
```

#### 素材の確認

```
// Blueprint
Get Material Amount
  Material ID: Iron
  → Return Value (int32)
```

```cpp
// C++
int32 IronAmount = CraftingManager->GetMaterialAmount(FName("Iron"));
```

### ステップ5: クラフトの実行

#### クラフト可能かチェック

```
// Blueprint
Can Craft Recipe
  Recipe ID: IronSword
  → Return Value (bool)
```

#### クラフトの開始

```
// Blueprint
Start Crafting
  Recipe ID: IronSword
  → Return Value (bool) - 成功した場合true
```

### ステップ6: イベントの処理

#### クラフト完了イベント

```
// Blueprint
Event BeginPlay
  → Crafting Manager
    → Bind Event to On Crafting Completed
      → Custom Event: Handle Crafting Completed
          Input: Item ID (Name), Amount (int32)
```

```cpp
// C++
CraftingManager->OnCraftingCompleted.AddDynamic(this, &AMyCharacter::OnCraftingCompleted);

void AMyCharacter::OnCraftingCompleted(FName ItemID, int32 Amount)
{
    // 完成したアイテムを処理
}
```

## UI との統合

### クラフト進捗バーの作成

#### Widget Blueprint

1. Progress Bar を追加
2. Event Tick で以下を実行：

```
Event Tick
  → Get Crafting Manager (from Player Character)
    → Is Crafting?
      → Branch
        True:
          → Get Crafting Progress (0.0 - 1.0)
            → Set Percent (Progress Bar)
        False:
          → Set Percent: 0.0
```

### レシピリストの表示

```
// レシピリストを取得
Get All Recipes
  → For Each Loop
    → Create Widget (Recipe List Item)
      → Set Recipe Data
      → Add to Scroll Box
```

### 素材リストの表示

```
// 素材リストを取得
Get All Materials
  → For Each Loop (Map)
    → Create Widget (Material List Item)
      → Set Material ID: Key
      → Set Amount: Value
      → Add to Scroll Box
```

## よくあるパターン

### パターン1: クラフトボタン

```
On Button Clicked
  → Get Selected Recipe ID
    → Can Craft Recipe?
      → Branch
        True:
          → Start Crafting
            → Show Success Message
        False:
          → Get Recipe
            → Get Missing Materials
              → Show Missing Materials Dialog
```

### パターン2: 自動素材収集

```cpp
void AMyCharacter::OnOverlapMaterial(AActor* OtherActor)
{
    if (AMaterialPickup* Pickup = Cast<AMaterialPickup>(OtherActor))
    {
        CraftingManager->AddMaterial(Pickup->MaterialID, Pickup->Amount);
        Pickup->Destroy();
    }
}
```

### パターン3: レシピのアンロックシステム

```cpp
void AMyCharacter::OnLevelUp(int32 NewLevel)
{
    // レベルに応じてレシピをアンロック
    if (NewLevel >= 5)
    {
        UCraftingRecipe* Recipe = CraftingManager->GetRecipe(FName("SteelSword"));
        if (Recipe)
        {
            Recipe->bIsUnlocked = true;
        }
    }
}
```

## トラブルシューティング

### 問題: レシピが表示されない

**解決策:**
- レシピが正しく登録されているか確認
- `GetAllRecipes()` または `GetUnlockedRecipes()` を使用

### 問題: クラフトが開始できない

**チェック項目:**
1. 素材が十分にあるか: `CanCraftRecipe()` で確認
2. レシピがアンロックされているか: `Recipe->bIsUnlocked`
3. すでにクラフト中でないか: `IsCrafting()`

### 問題: クラフトが完了しない

**確認事項:**
- CraftingManagerの `Tick` が有効になっているか
- Crafting Time が適切に設定されているか
- `GetCraftingProgress()` で進捗を確認

## パフォーマンスの考慮事項

### レシピの数が多い場合

```cpp
// レシピをカテゴリ分けして管理
TMap<FName, TArray<UCraftingRecipe*>> RecipesByCategory;

void OrganizeRecipes()
{
    TArray<UCraftingRecipe*> AllRecipes = CraftingManager->GetAllRecipes();
    for (UCraftingRecipe* Recipe : AllRecipes)
    {
        // カテゴリに基づいて分類（独自のロジック）
    }
}
```

### 素材の数が多い場合

インベントリシステムと統合することを推奨します。

## 次のステップ

- [API リファレンス](API.md) で詳細な関数仕様を確認
- [使用例](Examples.md) で実装パターンを学習
- 独自のゲームロジックに合わせてカスタマイズ
