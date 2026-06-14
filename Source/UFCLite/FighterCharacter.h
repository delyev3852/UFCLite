#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FighterCharacter.generated.h"

class UHealthComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class UFCLITE_API AFighterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFighterCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName FighterName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* FighterPortrait;

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);

	void JabPunch();
	void CrossPunch();
	void HookPunch();
	void UppercutPunch();

	void LowKick();
	void MidKick();
	void HighKick();

	void StartBlock();
	void StopBlock();

	void LightAttack();
	void HeavyAttack();

	UFUNCTION()
	void OnDeath();

	UPROPERTY(EditAnywhere)
	float StaminaCostLight = 5.0f;

	UPROPERTY(EditAnywhere)
	float StaminaCostHeavy = 10.0f;

	UPROPERTY(EditAnywhere)
	UAnimMontage* JabMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* CrossMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* HookMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* UppercutMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* LowKickMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* MidKickMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* HighKickMontage;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(BlueprintReadOnly)
	bool bIsBlocking;
};
