#include "FighterAnimInstance.h"
#include "FighterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetAnimationLibrary.h"

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
		Direction = UKismetAnimationLibrary::CalculateDirection(MoveComp->Velocity, Character->GetActorRotation());
	}

	bIsBlocking = Character->bIsBlocking;
	bIsDead = Character->bIsDead;
}
