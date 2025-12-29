#include "ControlSettings.h"

UControlSettings::UControlSettings()
{
	ResetToDefault();
}

void UControlSettings::ResetToDefault()
{
	MouseSensitivity = 1.0f;
	bInvertMouseY = false;
	ControllerSensitivity = 1.0f;
	bInvertControllerY = false;
	bAimAssist = true;
	bVibration = true;

	KeyBindings.Empty();
	InitializeDefaultKeyBindings();
}

void UControlSettings::InitializeDefaultKeyBindings()
{
	// デフォルトのキーバインディングを設定
	// Set up default key bindings
	KeyBindings.Add(FKeyBinding(FName("MoveForward"), EKeys::W));
	KeyBindings.Add(FKeyBinding(FName("MoveBackward"), EKeys::S));
	KeyBindings.Add(FKeyBinding(FName("MoveLeft"), EKeys::A));
	KeyBindings.Add(FKeyBinding(FName("MoveRight"), EKeys::D));
	KeyBindings.Add(FKeyBinding(FName("Jump"), EKeys::SpaceBar));
	KeyBindings.Add(FKeyBinding(FName("Crouch"), EKeys::LeftControl));
	KeyBindings.Add(FKeyBinding(FName("Sprint"), EKeys::LeftShift));
	KeyBindings.Add(FKeyBinding(FName("Interact"), EKeys::E));
	KeyBindings.Add(FKeyBinding(FName("Attack"), EKeys::LeftMouseButton));
	KeyBindings.Add(FKeyBinding(FName("SecondaryAttack"), EKeys::RightMouseButton));
	KeyBindings.Add(FKeyBinding(FName("Reload"), EKeys::R));
	KeyBindings.Add(FKeyBinding(FName("UseItem"), EKeys::F));
	KeyBindings.Add(FKeyBinding(FName("Inventory"), EKeys::Tab));
	KeyBindings.Add(FKeyBinding(FName("Pause"), EKeys::Escape));
}

void UControlSettings::SetKeyBinding(FName ActionName, FKey Key)
{
	for (FKeyBinding& Binding : KeyBindings)
	{
		if (Binding.ActionName == ActionName)
		{
			Binding.Key = Key;
			return;
		}
	}

	// 存在しない場合は新規追加
	KeyBindings.Add(FKeyBinding(ActionName, Key));
}

FKey UControlSettings::GetKeyBinding(FName ActionName) const
{
	for (const FKeyBinding& Binding : KeyBindings)
	{
		if (Binding.ActionName == ActionName)
		{
			return Binding.Key;
		}
	}

	return EKeys::Invalid;
}

void UControlSettings::ApplySettings()
{
	// ここで実際の入力システムに設定を適用
	// Apply settings to the actual input system
	// Note: 実装はプロジェクトの入力システムに依存します
	// Implementation depends on the project's input system
}

UControlSettings* UControlSettings::CreateCopy() const
{
	UControlSettings* Copy = NewObject<UControlSettings>();
	Copy->MouseSensitivity = MouseSensitivity;
	Copy->bInvertMouseY = bInvertMouseY;
	Copy->ControllerSensitivity = ControllerSensitivity;
	Copy->bInvertControllerY = bInvertControllerY;
	Copy->bAimAssist = bAimAssist;
	Copy->bVibration = bVibration;
	Copy->KeyBindings = KeyBindings;
	return Copy;
}
