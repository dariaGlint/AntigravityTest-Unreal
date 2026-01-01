#pragma once

#include "CoreMinimal.h"
#include "ScreenTransitionTypes.generated.h"

/**
 * Error types for screen transition operations
 */
UENUM(BlueprintType)
enum class EScreenTransitionError : uint8
{
	None UMETA(DisplayName = "None"),
	InvalidScreenClass UMETA(DisplayName = "Invalid Screen Class"),
	StackOverflow UMETA(DisplayName = "Stack Overflow"),
	StackUnderflow UMETA(DisplayName = "Stack Underflow"),
	TransitionTimeout UMETA(DisplayName = "Transition Timeout"),
	ContextValidationFailed UMETA(DisplayName = "Context Validation Failed"),
	ScreenCreationFailed UMETA(DisplayName = "Screen Creation Failed")
};

/**
 * Context data structure for passing information between screens
 */
USTRUCT(BlueprintType)
struct SCREENTRANSITIONSYSTEM_API FScreenTransitionContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Screen Transition")
	TMap<FString, FString> StringData;

	UPROPERTY(BlueprintReadWrite, Category = "Screen Transition")
	TMap<FString, int32> IntData;

	UPROPERTY(BlueprintReadWrite, Category = "Screen Transition")
	TMap<FString, float> FloatData;

	UPROPERTY(BlueprintReadWrite, Category = "Screen Transition")
	TMap<FString, bool> BoolData;
};
