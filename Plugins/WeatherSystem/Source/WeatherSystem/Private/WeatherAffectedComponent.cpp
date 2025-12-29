#include "WeatherAffectedComponent.h"
#include "WeatherManager.h"
#include "Engine/World.h"

UWeatherAffectedComponent::UWeatherAffectedComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bAffectedByWeather = true;
	bAffectedByVisibility = true;
	bAffectedByMovementSpeed = true;

	CurrentVisibilityMultiplier = 1.0f;
	CurrentMovementSpeedMultiplier = 1.0f;
}

void UWeatherAffectedComponent::BeginPlay()
{
	Super::BeginPlay();

	// WeatherManagerからのイベントを購読
	if (UWorld* World = GetWorld())
	{
		if (UWeatherManager* WeatherManager = World->GetSubsystem<UWeatherManager>())
		{
			WeatherManager->OnWeatherParametersUpdated.AddDynamic(this, &UWeatherAffectedComponent::OnWeatherParametersUpdated);

			// 初期パラメータを取得
			CurrentWeatherParameters = WeatherManager->GetCurrentWeatherParameters();
			UpdateEffects();
		}
	}
}

void UWeatherAffectedComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// イベントの購読を解除
	if (UWorld* World = GetWorld())
	{
		if (UWeatherManager* WeatherManager = World->GetSubsystem<UWeatherManager>())
		{
			WeatherManager->OnWeatherParametersUpdated.RemoveDynamic(this, &UWeatherAffectedComponent::OnWeatherParametersUpdated);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UWeatherAffectedComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWeatherAffectedComponent::OnWeatherParametersUpdated(const FWeatherParameters& NewParameters)
{
	CurrentWeatherParameters = NewParameters;
	UpdateEffects();
}

void UWeatherAffectedComponent::UpdateEffects()
{
	if (!bAffectedByWeather)
	{
		CurrentVisibilityMultiplier = 1.0f;
		CurrentMovementSpeedMultiplier = 1.0f;
		return;
	}

	if (bAffectedByVisibility)
	{
		CurrentVisibilityMultiplier = CurrentWeatherParameters.VisibilityMultiplier;
	}
	else
	{
		CurrentVisibilityMultiplier = 1.0f;
	}

	if (bAffectedByMovementSpeed)
	{
		CurrentMovementSpeedMultiplier = CurrentWeatherParameters.MovementSpeedMultiplier;
	}
	else
	{
		CurrentMovementSpeedMultiplier = 1.0f;
	}
}
