# 使用例

## 基本的な使用例

### 1. シンプルなクラフトシステム

```cpp
// MyCharacter.h
UCLASS()
class AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyCharacter();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crafting")
    UCraftingManager* CraftingManager;

    UFUNCTION()
    void OnCraftingCompleted(FName ItemID, int32 Amount);

    UFUNCTION()
    void OnCraftingFailed(FName RecipeID);
};

// MyCharacter.cpp
AMyCharacter::AMyCharacter()
{
    // CraftingManagerコンポーネントを作成
    CraftingManager = CreateDefaultSubobject<UCraftingManager>(TEXT("CraftingManager"));
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // イベントをバインド
    CraftingManager->OnCraftingCompleted.AddDynamic(this, &AMyCharacter::OnCraftingCompleted);
    CraftingManager->OnCraftingFailed.AddDynamic(this, &AMyCharacter::OnCraftingFailed);

    // レシピを登録（レシピアセットは別途作成）
    // CraftingManager->RegisterRecipe(SwordRecipe);
}

void AMyCharacter::OnCraftingCompleted(FName ItemID, int32 Amount)
{
    UE_LOG(LogTemp, Log, TEXT("Successfully crafted %d of %s"), Amount, *ItemID.ToString());
}

void AMyCharacter::OnCraftingFailed(FName RecipeID)
{
    UE_LOG(LogTemp, Warning, TEXT("Failed to craft %s"), *RecipeID.ToString());
}
```

### 2. 素材の管理

```cpp
// 素材を拾った時
void AMyCharacter::PickupMaterial(FName MaterialID, int32 Amount)
{
    CraftingManager->AddMaterial(MaterialID, Amount);
    UE_LOG(LogTemp, Log, TEXT("Picked up %d of %s"), Amount, *MaterialID.ToString());
}

// 所持している素材を確認
void AMyCharacter::CheckMaterials()
{
    TMap<FName, int32> AllMaterials = CraftingManager->GetAllMaterials();
    for (const auto& Pair : AllMaterials)
    {
        UE_LOG(LogTemp, Log, TEXT("Material: %s, Amount: %d"),
               *Pair.Key.ToString(), Pair.Value);
    }
}
```

### 3. クラフトの実行

```cpp
void AMyCharacter::TryCraftItem(FName RecipeID)
{
    // クラフト可能かチェック
    if (!CraftingManager->CanCraftRecipe(RecipeID))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot craft %s - insufficient materials"),
               *RecipeID.ToString());
        return;
    }

    // クラフト開始
    if (CraftingManager->StartCrafting(RecipeID))
    {
        UE_LOG(LogTemp, Log, TEXT("Started crafting %s"), *RecipeID.ToString());
    }
}

// クラフトの進捗を表示
void AMyCharacter::DisplayCraftingProgress()
{
    if (CraftingManager->IsCrafting())
    {
        float Progress = CraftingManager->GetCraftingProgress();
        UE_LOG(LogTemp, Log, TEXT("Crafting progress: %.2f%%"), Progress * 100.0f);
    }
}
```

## Blueprint での使用例

### レシピの作成

1. Content Browserで右クリック
2. Miscellaneous → Data Asset → CraftingRecipe
3. レシピの設定：
   - Recipe ID: `Sword`
   - Recipe Name: `鉄の剣`
   - Required Materials:
     - MaterialID: `Iron`, RequiredAmount: 5
     - MaterialID: `Wood`, RequiredAmount: 2
   - Result:
     - ItemID: `IronSword`, Amount: 1
   - Crafting Time: 3.0

### Blueprintでのクラフト処理

```
Event BeginPlay
  → Register Recipe (Recipe: SwordRecipe)

Event OnPickupMaterial
  → Add Material (MaterialID, Amount)

Event OnCraftButtonPressed
  → Can Craft Recipe? (RecipeID)
    → Branch
      True → Start Crafting (RecipeID)
      False → Display "Insufficient Materials"

Event Tick
  → Is Crafting?
    → Branch
      True → Get Crafting Progress
           → Update Progress Bar
```

## 高度な使用例

### レシピのアンロックシステム

```cpp
void AMyCharacter::UnlockRecipe(FName RecipeID)
{
    UCraftingRecipe* Recipe = CraftingManager->GetRecipe(RecipeID);
    if (Recipe)
    {
        Recipe->bIsUnlocked = true;
        UE_LOG(LogTemp, Log, TEXT("Unlocked recipe: %s"), *RecipeID.ToString());
    }
}

void AMyCharacter::ShowAvailableRecipes()
{
    TArray<UCraftingRecipe*> UnlockedRecipes = CraftingManager->GetUnlockedRecipes();
    for (UCraftingRecipe* Recipe : UnlockedRecipes)
    {
        if (CraftingManager->CanCraftRecipe(Recipe->RecipeID))
        {
            UE_LOG(LogTemp, Log, TEXT("Can craft: %s"), *Recipe->RecipeName.ToString());
        }
    }
}
```

### 不足素材の表示

```cpp
void AMyCharacter::ShowMissingMaterials(FName RecipeID)
{
    UCraftingRecipe* Recipe = CraftingManager->GetRecipe(RecipeID);
    if (!Recipe)
    {
        return;
    }

    TMap<FName, int32> MissingMaterials;
    Recipe->GetMissingMaterials(CraftingManager->GetAllMaterials(), MissingMaterials);

    if (MissingMaterials.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Missing materials for %s:"), *RecipeID.ToString());
        for (const auto& Pair : MissingMaterials)
        {
            UE_LOG(LogTemp, Warning, TEXT("  - %s: %d more needed"),
                   *Pair.Key.ToString(), Pair.Value);
        }
    }
}
```

### クラフトのキャンセル

```cpp
void AMyCharacter::CancelCurrentCrafting(bool bRefundMaterials)
{
    if (CraftingManager->IsCrafting())
    {
        UCraftingRecipe* CurrentRecipe = CraftingManager->GetCurrentCraftingRecipe();
        if (CurrentRecipe)
        {
            UE_LOG(LogTemp, Log, TEXT("Cancelling crafting of %s"),
                   *CurrentRecipe->RecipeName.ToString());
        }

        CraftingManager->CancelCrafting(bRefundMaterials);
    }
}
```

## ゲームセーブへの統合例

```cpp
// セーブデータ構造
USTRUCT()
struct FCraftingSaveData
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<FName, int32> Materials;

    UPROPERTY()
    TArray<FName> UnlockedRecipes;
};

// セーブ
void AMyCharacter::SaveCraftingData(FCraftingSaveData& SaveData)
{
    // 素材を保存
    SaveData.Materials = CraftingManager->GetAllMaterials();

    // アンロック済みレシピを保存
    TArray<UCraftingRecipe*> AllRecipes = CraftingManager->GetAllRecipes();
    for (UCraftingRecipe* Recipe : AllRecipes)
    {
        if (Recipe && Recipe->bIsUnlocked)
        {
            SaveData.UnlockedRecipes.Add(Recipe->RecipeID);
        }
    }
}

// ロード
void AMyCharacter::LoadCraftingData(const FCraftingSaveData& SaveData)
{
    // 素材を復元
    for (const auto& Pair : SaveData.Materials)
    {
        CraftingManager->AddMaterial(Pair.Key, Pair.Value);
    }

    // レシピのアンロック状態を復元
    for (const FName& RecipeID : SaveData.UnlockedRecipes)
    {
        UCraftingRecipe* Recipe = CraftingManager->GetRecipe(RecipeID);
        if (Recipe)
        {
            Recipe->bIsUnlocked = true;
        }
    }
}
```
