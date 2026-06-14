#include "FighterAnimInstance.h"
#include "FighterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UFighterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UFighterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	AFighterCharacter* Character = Cast<AFighterCharacter>(TryGetPawnOwner());
	if (!Character) return;

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (MoveComp)
	{
		Speed = MoveComp->Velocity.Size2D();
		if (!MoveComp->Velocity.IsZero())
		{
			FVector RotatedVel = Character->GetActorRotation().UnrotateVector(MoveComp->Velocity);
			Direction = FMath::Atan2(RotatedVel.Y, RotatedVel.X) * (180.0f / UE_PI);
		}
		else
		{
			Direction = 0.0f;
		}
	}

	bIsBlocking = Character->bIsBlocking;
	bIsDead = Character->bIsDead;
}
