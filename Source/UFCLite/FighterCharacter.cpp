#include "FighterCharacter.h"
#include "HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputModifiers.h"

AFighterCharacter::AFighterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

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

		static ConstructorHelpers::FObjectFinder<UMaterialInterface> BodyMat(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
		if (BodyMat.Succeeded())
		{
			VisualMesh->SetMaterial(0, BodyMat.Object);
		}
	}

	SetupEnhancedInput();
}

void AFighterCharacter::SetupEnhancedInput()
{
	InputMapping = NewObject<UInputMappingContext>(this);

	auto NewAction = [this](EInputActionValueType Type)
	{
		UInputAction* Action = NewObject<UInputAction>(this);
		Action->ValueType = Type;
		return Action;
	};

	MoveForwardAction = NewAction(EInputActionValueType::Axis1D);
	MoveRightAction = NewAction(EInputActionValueType::Axis1D);
	JabAction = NewAction(EInputActionValueType::Boolean);
	CrossAction = NewAction(EInputActionValueType::Boolean);
	HookAction = NewAction(EInputActionValueType::Boolean);
	UppercutAction = NewAction(EInputActionValueType::Boolean);
	LowKickAction = NewAction(EInputActionValueType::Boolean);
	MidKickAction = NewAction(EInputActionValueType::Boolean);
	HighKickAction = NewAction(EInputActionValueType::Boolean);
	BlockAction = NewAction(EInputActionValueType::Boolean);

	auto Map = [this](UInputAction* Action, FKey Key) { return InputMapping->MapKey(Action, Key); };
	auto Negate = [this]() { return NewObject<UInputModifierNegate>(); };

	Map(MoveForwardAction, EKeys::W);
	Map(MoveForwardAction, EKeys::S).Modifiers.Add(Negate());
	Map(MoveForwardAction, EKeys::Gamepad_LeftY).Modifiers.Add(Negate());
	Map(MoveRightAction, EKeys::A).Modifiers.Add(Negate());
	Map(MoveRightAction, EKeys::D);
	Map(MoveRightAction, EKeys::Gamepad_LeftX);

	Map(JabAction, EKeys::X);
	Map(JabAction, EKeys::Gamepad_FaceButton_Left);
	Map(CrossAction, EKeys::Y);
	Map(CrossAction, EKeys::Gamepad_FaceButton_Top);
	Map(HookAction, EKeys::B);
	Map(HookAction, EKeys::Gamepad_FaceButton_Right);
	Map(UppercutAction, EKeys::A);
	Map(UppercutAction, EKeys::Gamepad_FaceButton_Bottom);

	Map(LowKickAction, EKeys::Q);
	Map(LowKickAction, EKeys::Gamepad_LeftShoulder);
	Map(MidKickAction, EKeys::E);
	Map(MidKickAction, EKeys::Gamepad_RightShoulder);
	Map(HighKickAction, EKeys::R);

	Map(BlockAction, EKeys::LeftShift);
	Map(BlockAction, EKeys::Gamepad_LeftTrigger);
}

void AFighterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &AFighterCharacter::OnDeath);
	}

	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
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

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC) return;

	EIC->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AFighterCharacter::MoveForward);
	EIC->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AFighterCharacter::MoveRight);

	EIC->BindAction(JabAction, ETriggerEvent::Started, this, &AFighterCharacter::JabPunch);
	EIC->BindAction(CrossAction, ETriggerEvent::Started, this, &AFighterCharacter::CrossPunch);
	EIC->BindAction(HookAction, ETriggerEvent::Started, this, &AFighterCharacter::HookPunch);
	EIC->BindAction(UppercutAction, ETriggerEvent::Started, this, &AFighterCharacter::UppercutPunch);

	EIC->BindAction(LowKickAction, ETriggerEvent::Started, this, &AFighterCharacter::LowKick);
	EIC->BindAction(MidKickAction, ETriggerEvent::Started, this, &AFighterCharacter::MidKick);
	EIC->BindAction(HighKickAction, ETriggerEvent::Started, this, &AFighterCharacter::HighKick);

	EIC->BindAction(BlockAction, ETriggerEvent::Started, this, &AFighterCharacter::StartBlock);
	EIC->BindAction(BlockAction, ETriggerEvent::Completed, this, &AFighterCharacter::StopBlock);
}

void AFighterCharacter::MoveForward(const FInputActionValue& Value)
{
	if (!Controller) return;
	const FRotator YawRot(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::X), Value.Get<float>());
}

void AFighterCharacter::MoveRight(const FInputActionValue& Value)
{
	if (!Controller) return;
	const FRotator YawRot(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y), Value.Get<float>());
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
