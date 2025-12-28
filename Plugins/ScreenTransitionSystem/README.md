# Screen Transition System for Unreal Engine 5

UE5用の柔軟な画面遷移システムプラグインです。非同期ロード、トランジションエフェクト、画面スタック管理をサポートします。

## 特徴

- **簡単な画面遷移管理** - GameInstanceSubsystemベースの中央管理システム
- **画面スタック機能** - 前の画面に戻る機能をサポート
- **トランジションエフェクト** - フェード、スライドなど複数のエフェクト
- **Blueprint完全対応** - C++とBlueprintの両方で使用可能
- **ライフサイクル管理** - OnEnter、OnExit、OnPause、OnResumeイベント
- **カスタマイズ可能** - 独自のトランジションエフェクトを作成可能

## インストール

1. このプラグインフォルダをUE5プロジェクトの`Plugins`ディレクトリにコピー
2. UE5エディタを起動
3. Edit > Plugins から "Screen Transition System" を有効化
4. エディタを再起動

## クイックスタート

### 1. 画面Widgetの作成

Widget Blueprintを作成し、親クラスを`ScreenBase`に設定します。

**例: タイトル画面の作成**
```
Content/UI/WBP_TitleScreen
親クラス: ScreenBase
```

### 2. 画面遷移の実行

**Blueprint:**
```
Get Game Instance
  └─ Get Subsystem (Screen Transition Manager)
      └─ Transition To Screen
          ├─ Screen Class: WBP_TitleScreen
          ├─ Use Transition: true
          └─ Transition Effect Class: (Optional)
```

**C++:**
```cpp
UScreenTransitionManager* Manager = GetGameInstance()->GetSubsystem<UScreenTransitionManager>();
Manager->TransitionToScreen(UWBP_TitleScreen::StaticClass(), true);
```

### 3. 画面のライフサイクル

ScreenBaseを継承した画面では、以下のイベントをオーバーライドできます：

```
Event On Enter    // 画面が表示されたとき
Event On Exit     // 画面が非表示になるとき
Event On Pause    // 画面が一時停止されたとき（モーダル表示時など）
Event On Resume   // 画面が再開されたとき
```

## 主要機能

### 画面遷移

```cpp
// 画面を完全に切り替え（スタックをクリア）
TransitionToScreen(TSubclassOf<UScreenBase> ScreenClass, bool bUseTransition, TSubclassOf<UTransitionEffect> TransitionEffectClass);

// 画面をスタックにプッシュ
PushScreen(TSubclassOf<UScreenBase> ScreenClass, bool bAsModal, bool bUseTransition, TSubclassOf<UTransitionEffect> TransitionEffectClass);

// 前の画面に戻る
PopScreen(bool bUseTransition, TSubclassOf<UTransitionEffect> TransitionEffectClass);

// スタックをすべてクリア
ClearScreenStack();
```

### トランジションエフェクト

デフォルトで以下のエフェクトタイプをサポート：

- **Fade** - フェードイン/アウト
- **Slide** - スライド遷移（Left, Right, Up, Down）
- **Custom** - カスタムエフェクト（Blueprint/C++で実装）

### イベント

```cpp
// 画面が変更されたとき
OnScreenChanged(UScreenBase* OldScreen, UScreenBase* NewScreen);

// トランジション開始時
OnTransitionStarted(UScreenBase* TargetScreen);

// トランジション完了時
OnTransitionCompleted(UScreenBase* CurrentScreen);
```

## ディレクトリ構造

```
Plugins/ScreenTransitionSystem/
├── README.md                           # このファイル
├── ScreenTransitionSystem.uplugin      # プラグイン定義
├── Source/ScreenTransitionSystem/
│   ├── Public/
│   │   ├── ScreenTransitionManager.h   # メインマネージャー
│   │   ├── ScreenBase.h                # 画面基底クラス
│   │   └── TransitionEffect.h          # トランジションエフェクト
│   └── Private/
│       └── (実装ファイル)
├── Content/
│   ├── UI/                             # UIアセット用
│   └── Effects/                        # エフェクト用
└── Documentation/
    ├── Usage.md                        # 詳細な使用方法
    ├── API.md                          # APIリファレンス
    └── Examples.md                     # サンプルコード
```

## サンプル

詳細なサンプルは[Examples.md](Documentation/Examples.md)を参照してください。

### 基本的な画面遷移フロー

```
タイトル画面
  └─ ゲーム開始ボタン押下
      └─ Transition To Screen (WBP_MainGame)
          └─ メインゲーム画面表示
              └─ 設定ボタン押下
                  └─ Push Screen (WBP_Settings)
                      └─ 設定画面表示（モーダル）
                          └─ 閉じるボタン押下
                              └─ Pop Screen
                                  └─ メインゲーム画面に戻る
```

## 必要要件

- Unreal Engine 5.0 以上
- C++プロジェクト（またはソースコードをコンパイル可能な環境）

## ライセンス

このプラグインは自由に使用、改変、配布できます。

## サポート

問題が発生した場合は、Issueを作成してください。

## 関連ドキュメント

- [詳細な使用方法](Documentation/Usage.md)
- [APIリファレンス](Documentation/API.md)
- [サンプル集](Documentation/Examples.md)
