#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FighterCharacter.generated.h"

class UHealthComponent;
class UInputMappingContext;
class UInputAction;

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

	UPROPERTY(BlueprintReadOnly)
	bool bIsBlocking;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead = false;

	int32 PlayerIndex = 0;

protected:
	void MoveForward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void MoveForwardLegacy(float Value);
	void MoveRightLegacy(float Value);

	void LeadJab();
	void LeadCross();
	void LeadHook();
	void LeadUppercut();
	void RearKickLow();
	void RearKickMid();
	void RearKickHigh();
	void RearKickBody();

	void StartBlock();
	void StopBlock();
	void OnKickModifier(const FInputActionValue& Value);
	void OnBlockModifier(const FInputActionValue& Value);

	void LightAttack();
	void HeavyAttack();

	UFUNCTION()
	void OnDeath();

	void HitReact();

	bool PerformAttackTrace(float Damage);
	bool IsOpponent(AFighterCharacter* Other) const;
	void SpawnBloodEffect(const FVector& Location);

	UPROPERTY(EditAnywhere)
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere)
	float AttackRadius = 30.0f;

	UPROPERTY(EditAnywhere)
	float KnockbackForce = 500.0f;

	UPROPERTY(EditAnywhere)
	float BlockDamageMultiplier = 0.3f;

	UPROPERTY(EditAnywhere)
	float JabDamage = 5.0f;

	UPROPERTY(EditAnywhere)
	float CrossDamage = 8.0f;

	UPROPERTY(EditAnywhere)
	float HookDamage = 12.0f;

	UPROPERTY(EditAnywhere)
	float UppercutDamage = 12.0f;

	UPROPERTY(EditAnywhere)
	float LowKickDamage = 5.0f;

	UPROPERTY(EditAnywhere)
	float MidKickDamage = 8.0f;

	UPROPERTY(EditAnywhere)
	float HighKickDamage = 12.0f;

	UPROPERTY(EditAnywhere)
	float BodyKickDamage = 10.0f;

	void SetupEnhancedInput(int32 InPlayerIndex = 0);

	UPROPERTY()
	UInputMappingContext* InputMapping;

	UPROPERTY()
	UInputAction* MoveForwardAction;

	UPROPERTY()
	UInputAction* MoveRightAction;

	UPROPERTY()
	UInputAction* FaceLeftAction;

	UPROPERTY()
	UInputAction* FaceTopAction;

	UPROPERTY()
	UInputAction* FaceRightAction;

	UPROPERTY()
	UInputAction* FaceBottomAction;

	UPROPERTY()
	UInputAction* KickModifierAction;

	UPROPERTY()
	UInputAction* BlockAction;

	bool bKickModifierHeld;
	bool bBlockHeld;

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

	UPROPERTY(EditAnywhere)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* DeathMontage;

	float LastDebugTime = 0.0f;
};
