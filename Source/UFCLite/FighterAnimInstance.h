#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FighterAnimInstance.generated.h"

UCLASS()
class UFCLITE_API UFighterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly)
	float Speed;

	UPROPERTY(BlueprintReadOnly)
	float Direction;

	UPROPERTY(BlueprintReadOnly)
	bool bIsBlocking;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;
};
