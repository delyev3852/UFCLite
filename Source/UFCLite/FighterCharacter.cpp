#include "FighterCharacter.h"
#include "HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AFighterCharacter::AFighterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->SetRelativeRotation(FRotator(-10.0f, 0.0f, 0.0f));
	SpringArm->bEnableCameraLag = false;
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	bIsBlocking = false;
	bReplicates = true;

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	UStaticMeshComponent* VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualBody"));
	VisualMesh->SetupAttachment(RootComponent);
	VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
	VisualMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 2.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (BodyMesh.Succeeded())
	{
		VisualMesh->SetStaticMesh(BodyMesh.Object);
		VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		VisualMesh->SetIsReplicated(true);
	}

void AFighterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &AFighterCharacter::OnDeath);
	}
}

void AFighterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HealthComponent)
	{
		HealthComponent->TickRegen(DeltaTime);
	}
}

void AFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFighterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFighterCharacter::MoveRight);

	PlayerInputComponent->BindAction("Jab", IE_Pressed, this, &AFighterCharacter::JabPunch);
	PlayerInputComponent->BindAction("Cross", IE_Pressed, this, &AFighterCharacter::CrossPunch);
	PlayerInputComponent->BindAction("Hook", IE_Pressed, this, &AFighterCharacter::HookPunch);
	PlayerInputComponent->BindAction("Uppercut", IE_Pressed, this, &AFighterCharacter::UppercutPunch);

	PlayerInputComponent->BindAction("LowKick", IE_Pressed, this, &AFighterCharacter::LowKick);
	PlayerInputComponent->BindAction("MidKick", IE_Pressed, this, &AFighterCharacter::MidKick);
	PlayerInputComponent->BindAction("HighKick", IE_Pressed, this, &AFighterCharacter::HighKick);

	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &AFighterCharacter::StartBlock);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &AFighterCharacter::StopBlock);
}

void AFighterCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AFighterCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AFighterCharacter::JabPunch()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostLight)) return;
	PlayAnimMontage(JabMontage);
}

void AFighterCharacter::CrossPunch()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostLight)) return;
	PlayAnimMontage(CrossMontage);
}

void AFighterCharacter::HookPunch()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostHeavy)) return;
	PlayAnimMontage(HookMontage);
}

void AFighterCharacter::UppercutPunch()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostHeavy)) return;
	PlayAnimMontage(UppercutMontage);
}

void AFighterCharacter::LowKick()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostLight)) return;
	PlayAnimMontage(LowKickMontage);
}

void AFighterCharacter::MidKick()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostLight)) return;
	PlayAnimMontage(MidKickMontage);
}

void AFighterCharacter::HighKick()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostHeavy)) return;
	PlayAnimMontage(HighKickMontage);
}

void AFighterCharacter::StartBlock()
{
	bIsBlocking = true;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed *= 0.5f;
	}
}

void AFighterCharacter::StopBlock()
{
	bIsBlocking = false;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed *= 2.0f;
	}
}

void AFighterCharacter::LightAttack()
{
	JabPunch();
}

void AFighterCharacter::HeavyAttack()
{
	HookPunch();
}

void AFighterCharacter::OnDeath()
{
	GetCharacterMovement()->DisableMovement();
	SetLifeSpan(3.0f);
}
