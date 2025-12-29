# Settings Management System Plugin

設定管理システムプラグイン / Settings Management System Plugin

## 概要 / Overview

このプラグインは、Unreal Engine 5でのゲーム設定を包括的に管理するためのシステムを提供します。

This plugin provides a comprehensive system for managing game settings in Unreal Engine 5.

## 機能 / Features

### 1. オーディオ設定 / Audio Settings
- マスターボリューム / Master Volume
- BGMボリューム / Music Volume
- SEボリューム / SFX Volume
- ボイスボリューム / Voice Volume
- 環境音ボリューム / Ambient Volume
- ミュート設定 / Mute Settings

### 2. グラフィック設定 / Graphics Settings
- 品質プリセット（Low, Medium, High, Ultra, Custom）/ Quality Presets
- 解像度スケール / Resolution Scale
- 影の品質 / Shadow Quality
- アンチエイリアス品質 / Anti-Aliasing Quality
- テクスチャ品質 / Texture Quality
- エフェクト品質 / Effects Quality
- ポストプロセス品質 / Post-Process Quality
- 視野距離品質 / View Distance Quality
- 垂直同期 / VSync
- フレームレート制限 / Frame Rate Limit
- フルスクリーンモード / Fullscreen Mode

### 3. コントロール設定 / Control Settings
- マウス感度 / Mouse Sensitivity
- マウスY軸反転 / Invert Mouse Y
- コントローラー感度 / Controller Sensitivity
- コントローラーY軸反転 / Invert Controller Y
- エイムアシスト / Aim Assist
- バイブレーション / Vibration
- キーバインディング / Key Bindings

### 4. プリセット管理 / Preset Management
- カスタムプリセットの作成 / Create Custom Presets
- プリセットの保存と読み込み / Save and Load Presets
- プリセットの削除 / Delete Presets

### 5. 保存/読み込み機能 / Save/Load Functionality
- 自動的な設定の保存 / Automatic Settings Saving
- 起動時の設定読み込み / Load Settings on Startup
- デフォルト値への復元 / Reset to Default Values

## 使用方法 / Usage

### C++での使用 / Using in C++

```cpp
// SettingsManagerの取得
USettingsManager* SettingsManager = GetGameInstance()->GetSubsystem<USettingsManager>();

// オーディオ設定の変更
UAudioSettings* AudioSettings = SettingsManager->GetAudioSettings();
AudioSettings->MasterVolume = 0.8f;
AudioSettings->ApplySettings();

// グラフィック設定の変更
UGraphicsSettings* GraphicsSettings = SettingsManager->GetGraphicsSettings();
GraphicsSettings->ApplyPreset(EGraphicsQualityPreset::High);

// 設定の保存
SettingsManager->SaveSettings();

// 設定の読み込み
SettingsManager->LoadSettings();

// デフォルトに復元
SettingsManager->ResetAllToDefault();
```

### Blueprintでの使用 / Using in Blueprints

1. **Settings Managerの取得**
   - Get Game Instance -> Get Subsystem (Settings Manager)

2. **設定の変更**
   - Get Audio Settings -> Set Properties -> Apply Settings
   - Get Graphics Settings -> Set Properties -> Apply Settings
   - Get Control Settings -> Set Properties -> Apply Settings

3. **プリセットの管理**
   - Create Preset
   - Save Preset
   - Load Preset
   - Apply Preset

4. **設定の保存/読み込み**
   - Save Settings
   - Load Settings
   - Reset All To Default

## クラス構成 / Class Structure

### コアクラス / Core Classes

- **USettingsManager**: メイン管理クラス（GameInstanceSubsystem）/ Main management class
- **UAudioSettings**: オーディオ設定クラス / Audio settings class
- **UGraphicsSettings**: グラフィック設定クラス / Graphics settings class
- **UControlSettings**: コントロール設定クラス / Control settings class
- **USettingsPreset**: プリセット管理クラス / Preset management class
- **USettingsSaveGame**: セーブデータクラス / Save data class

## セットアップ / Setup

1. プラグインを `Plugins/SettingsManagement` フォルダに配置
2. プロジェクトを再生成（.uprojectを右クリック -> "Generate Visual Studio project files"）
3. プロジェクトをビルド
4. エディタでプラグインを有効化

## 技術仕様 / Technical Specifications

- **UE Version**: 5.7+
- **Module Type**: Runtime
- **Dependencies**: Core, CoreUObject, Engine, UMG, Slate, SlateCore, InputCore

## ライセンス / License

このプラグインはプロジェクトの一部として提供されます。

This plugin is provided as part of the project.

## 作成者 / Author

Antigravity

## バージョン / Version

1.0
