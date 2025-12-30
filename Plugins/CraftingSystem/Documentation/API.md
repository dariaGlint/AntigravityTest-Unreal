# API リファレンス

## UCraftingRecipe

レシピデータアセットクラス。

### プロパティ

| プロパティ | 型 | 説明 |
|----------|-----|------|
| RecipeID | FName | レシピの一意なID |
| RecipeName | FText | レシピの表示名 |
| Description | FText | レシピの説明 |
| RequiredMaterials | TArray<FCraftingMaterial> | 必要な素材のリスト |
| Result | FCraftingResult | クラフト結果のアイテム |
| CraftingTime | float | クラフトに必要な時間（秒） |
| bIsUnlocked | bool | レシピがアンロックされているか |

### メソッド

#### CanCraft
```cpp
bool CanCraft(const TMap<FName, int32>& AvailableMaterials) const
```
素材が十分にあるかチェックします。

**パラメータ:**
- `AvailableMaterials`: 利用可能な素材のマップ（MaterialID -> Amount）

**戻り値:** 素材が十分にある場合true

#### GetMissingMaterials
```cpp
void GetMissingMaterials(const TMap<FName, int32>& AvailableMaterials, TMap<FName, int32>& OutMissingMaterials) const
```
不足している素材のリストを取得します。

**パラメータ:**
- `AvailableMaterials`: 利用可能な素材のマップ
- `OutMissingMaterials`: 不足している素材のリスト（MaterialID -> 不足数）

## UCraftingManager

クラフティングシステムを管理するActorComponent。

### イベント

#### OnCraftingCompleted
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCraftingCompleted, FName, ItemID, int32, Amount)
```
クラフト完了時に発火されるイベント。

#### OnCraftingFailed
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCraftingFailed, FName, RecipeID)
```
クラフト失敗時に発火されるイベント。

### メソッド

#### RegisterRecipe
```cpp
void RegisterRecipe(UCraftingRecipe* Recipe)
```
レシピを登録します。

#### UnregisterRecipe
```cpp
void UnregisterRecipe(FName RecipeID)
```
レシピを削除します。

#### GetRecipe
```cpp
UCraftingRecipe* GetRecipe(FName RecipeID) const
```
レシピを取得します。

#### GetAllRecipes
```cpp
TArray<UCraftingRecipe*> GetAllRecipes() const
```
すべてのレシピを取得します。

#### GetUnlockedRecipes
```cpp
TArray<UCraftingRecipe*> GetUnlockedRecipes() const
```
アンロック済みのレシピのみを取得します。

#### AddMaterial
```cpp
void AddMaterial(FName MaterialID, int32 Amount)
```
素材を追加します。

#### RemoveMaterial
```cpp
int32 RemoveMaterial(FName MaterialID, int32 Amount)
```
素材を削除します。実際に削除された数量を返します。

#### GetMaterialAmount
```cpp
int32 GetMaterialAmount(FName MaterialID) const
```
素材の所持数を取得します。

#### GetAllMaterials
```cpp
TMap<FName, int32> GetAllMaterials() const
```
すべての素材を取得します。

#### StartCrafting
```cpp
bool StartCrafting(FName RecipeID)
```
クラフトを開始します。

**戻り値:** クラフトが開始された場合true

#### CanCraftRecipe
```cpp
bool CanCraftRecipe(FName RecipeID) const
```
レシピがクラフト可能かチェックします。

#### IsCrafting
```cpp
bool IsCrafting() const
```
現在クラフト中かチェックします。

#### GetCraftingProgress
```cpp
float GetCraftingProgress() const
```
クラフトの進捗を取得します（0.0 - 1.0）。

#### GetCurrentCraftingRecipe
```cpp
UCraftingRecipe* GetCurrentCraftingRecipe() const
```
現在クラフト中のレシピを取得します。

#### CancelCrafting
```cpp
void CancelCrafting(bool bRefundMaterials = true)
```
クラフトをキャンセルします。

**パラメータ:**
- `bRefundMaterials`: 素材を返却するか

## データ構造

### FCraftingMaterial
```cpp
struct FCraftingMaterial
{
    FName MaterialID;      // 素材のID
    int32 RequiredAmount;  // 必要な数量
}
```

### FCraftingResult
```cpp
struct FCraftingResult
{
    FName ItemID;  // 生成されるアイテムのID
    int32 Amount;  // 生成される数量
}
```
