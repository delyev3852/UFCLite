#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChanged, float, NewStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UFCLITE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaminaRegenRate = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaminaRegenDelay = 1.0f;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnStaminaChanged OnStaminaChanged;

	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UFUNCTION(BlueprintCallable)
	void TakeDamage(float Damage);

	UFUNCTION(BlueprintCallable)
	bool ConsumeStamina(float Amount);

	UFUNCTION(BlueprintPure)
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure)
	float GetStamina() const { return Stamina; }

	UFUNCTION(BlueprintCallable)
	void ResetHealth();

	void TickRegen(float DeltaTime);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float Health;

	UPROPERTY(ReplicatedUsing = OnRep_Stamina)
	float Stamina;

	float TimeSinceLastStaminaUse;

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_Stamina();

	bool bIsDead;
};
