#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	Stamina = MaxStamina;
	TimeSinceLastStaminaUse = 0.0f;
	bIsDead = false;
}

void UHealthComponent::TakeDamage(float Damage)
{
	if (bIsDead || Damage <= 0.0f) return;

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	OnRep_Health();
	OnHealthChanged.Broadcast(Health);

	if (Health <= 0.0f && !bIsDead)
	{
		bIsDead = true;
		OnDeath.Broadcast();
	}
}

bool UHealthComponent::ConsumeStamina(float Amount)
{
	if (Stamina >= Amount)
	{
		Stamina = FMath::Clamp(Stamina - Amount, 0.0f, MaxStamina);
		TimeSinceLastStaminaUse = 0.0f;
		OnRep_Stamina();
		OnStaminaChanged.Broadcast(Stamina);
		return true;
	}
	return false;
}

void UHealthComponent::ResetHealth()
{
	Health = MaxHealth;
	Stamina = MaxStamina;
	bIsDead = false;
	OnRep_Health();
	OnRep_Stamina();
}

void UHealthComponent::TickRegen(float DeltaTime)
{
	if (bIsDead) return;

	TimeSinceLastStaminaUse += DeltaTime;
	if (TimeSinceLastStaminaUse >= StaminaRegenDelay && Stamina < MaxStamina)
	{
		Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.0f, MaxStamina);
		OnRep_Stamina();
		OnStaminaChanged.Broadcast(Stamina);
	}
}

void UHealthComponent::OnRep_Health()
{
}

void UHealthComponent::OnRep_Stamina()
{
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, Health);
	DOREPLIFETIME(UHealthComponent, Stamina);
}
