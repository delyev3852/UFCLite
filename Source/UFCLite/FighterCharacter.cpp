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

	bKickModifierHeld = false;
	bBlockHeld = false;
}

void AFighterCharacter::SetupEnhancedInput()
{
	auto NewA = [this](EInputActionValueType T, const TCHAR* N)
	{
		UInputAction* A = NewObject<UInputAction>(this, N);
		A->ValueType = T;
		return A;
	};

	InputMapping = NewObject<UInputMappingContext>(this, TEXT("IMC"));
	MoveForwardAction = NewA(EInputActionValueType::Axis1D, TEXT("MoveFwd"));
	MoveRightAction = NewA(EInputActionValueType::Axis1D, TEXT("MoveRight"));
	FaceLeftAction = NewA(EInputActionValueType::Boolean, TEXT("FaceL"));
	FaceTopAction = NewA(EInputActionValueType::Boolean, TEXT("FaceT"));
	FaceRightAction = NewA(EInputActionValueType::Boolean, TEXT("FaceR"));
	FaceBottomAction = NewA(EInputActionValueType::Boolean, TEXT("FaceB"));
	KickModifierAction = NewA(EInputActionValueType::Boolean, TEXT("KickMod"));
	BlockAction = NewA(EInputActionValueType::Boolean, TEXT("Block"));

	auto Map = [this](UInputAction* A, FKey K) -> FEnhancedActionKeyMapping& { return InputMapping->MapKey(A, K); };

	Map(MoveForwardAction, EKeys::W);
	Map(MoveForwardAction, EKeys::S).Modifiers.Add(NewObject<UInputModifierNegate>());
	Map(MoveForwardAction, EKeys::Gamepad_LeftY).Modifiers.Add(NewObject<UInputModifierNegate>());
	Map(MoveRightAction, EKeys::A).Modifiers.Add(NewObject<UInputModifierNegate>());
	Map(MoveRightAction, EKeys::D);
	Map(MoveRightAction, EKeys::Gamepad_LeftX);

	Map(FaceLeftAction, EKeys::X);
	Map(FaceLeftAction, EKeys::Gamepad_FaceButton_Left);
	Map(FaceTopAction, EKeys::Y);
	Map(FaceTopAction, EKeys::Gamepad_FaceButton_Top);
	Map(FaceRightAction, EKeys::B);
	Map(FaceRightAction, EKeys::Gamepad_FaceButton_Right);
	Map(FaceBottomAction, EKeys::A);
	Map(FaceBottomAction, EKeys::Gamepad_FaceButton_Bottom);

	Map(KickModifierAction, EKeys::Gamepad_RightTrigger);
	Map(BlockAction, EKeys::LeftShift);
	Map(BlockAction, EKeys::Gamepad_RightShoulder);
}

void AFighterCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupEnhancedInput();

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

	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		UEnhancedInputLocalPlayerSubsystem* Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (Sub)
		{
			Sub->AddMappingContext(InputMapping, 0);
		}
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC) return;

	EIC->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AFighterCharacter::MoveForward);
	EIC->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AFighterCharacter::MoveRight);

	EIC->BindAction(FaceLeftAction, ETriggerEvent::Started, this, &AFighterCharacter::LeadJab);
	EIC->BindAction(FaceTopAction, ETriggerEvent::Started, this, &AFighterCharacter::LeadCross);
	EIC->BindAction(FaceRightAction, ETriggerEvent::Started, this, &AFighterCharacter::LeadHook);
	EIC->BindAction(FaceBottomAction, ETriggerEvent::Started, this, &AFighterCharacter::LeadUppercut);

	EIC->BindAction(KickModifierAction, ETriggerEvent::Started, this, &AFighterCharacter::OnKickModifier);
	EIC->BindAction(KickModifierAction, ETriggerEvent::Completed, this, &AFighterCharacter::OnKickModifier);

	EIC->BindAction(BlockAction, ETriggerEvent::Started, this, &AFighterCharacter::StartBlock);
	EIC->BindAction(BlockAction, ETriggerEvent::Completed, this, &AFighterCharacter::StopBlock);
}

void AFighterCharacter::MoveForward(const FInputActionValue& Value)
{
	AddMovementInput(GetActorForwardVector(), Value.Get<float>());
}

void AFighterCharacter::MoveRight(const FInputActionValue& Value)
{
	AddMovementInput(GetActorRightVector(), Value.Get<float>());
}

void AFighterCharacter::LeadJab()
{
	if (bKickModifierHeld) { RearKickLow(); return; }
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostLight)) return;
	PlayAnimMontage(JabMontage);
}

void AFighterCharacter::LeadCross()
{
	if (bKickModifierHeld) { RearKickMid(); return; }
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostLight)) return;
	PlayAnimMontage(CrossMontage);
}

void AFighterCharacter::LeadHook()
{
	if (bKickModifierHeld) { RearKickHigh(); return; }
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostHeavy)) return;
	PlayAnimMontage(HookMontage);
}

void AFighterCharacter::LeadUppercut()
{
	if (bKickModifierHeld) { RearKickBody(); return; }
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostHeavy)) return;
	PlayAnimMontage(UppercutMontage);
}

void AFighterCharacter::RearKickLow()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostLight)) return;
	PlayAnimMontage(LowKickMontage);
}

void AFighterCharacter::RearKickMid()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostLight)) return;
	PlayAnimMontage(MidKickMontage);
}

void AFighterCharacter::RearKickHigh()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostHeavy)) return;
	PlayAnimMontage(HighKickMontage);
}

void AFighterCharacter::RearKickBody()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostHeavy)) return;
	PlayAnimMontage(JabMontage);
}

void AFighterCharacter::OnKickModifier(const FInputActionValue& Value)
{
	bKickModifierHeld = Value.Get<bool>();
}

void AFighterCharacter::OnBlockModifier(const FInputActionValue& Value)
{
	bBlockHeld = Value.Get<bool>();
	if (bBlockHeld) StartBlock();
	else StopBlock();
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
	LeadJab();
}

void AFighterCharacter::HeavyAttack()
{
	LeadHook();
}

void AFighterCharacter::OnDeath()
{
	GetCharacterMovement()->DisableMovement();
	SetLifeSpan(3.0f);
}
