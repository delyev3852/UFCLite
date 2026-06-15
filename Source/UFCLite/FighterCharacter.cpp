#include "FighterCharacter.h"
#include "FightGameMode.h"
#include "FighterAnimInstance.h"
#include "HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputModifiers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AFighterCharacter::AFighterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	bIsBlocking = false;
	bReplicates = true;

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->AnimClass = UFighterAnimInstance::StaticClass();

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MannequinMesh(TEXT("/Engine/EngineMeshes/SkeletalMannequin.SkeletalMannequin"));
	if (MannequinMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MannequinMesh.Object);
	}

	UStaticMeshComponent* VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualBody"));
	VisualMesh->SetupAttachment(RootComponent);
	VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
	VisualMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 2.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (BodyMesh.Succeeded())
	{
		VisualMesh->SetStaticMesh(BodyMesh.Object);
		VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	if (PlayerIndex == 0)
	{
		Map(MoveForwardAction, EKeys::W);
		Map(MoveForwardAction, EKeys::S).Modifiers.Add(NewObject<UInputModifierNegate>());
		Map(MoveRightAction, EKeys::A).Modifiers.Add(NewObject<UInputModifierNegate>());
		Map(MoveRightAction, EKeys::D);

		Map(FaceLeftAction, EKeys::Z);
		Map(FaceTopAction, EKeys::X);
		Map(FaceRightAction, EKeys::C);
		Map(FaceBottomAction, EKeys::V);

		Map(KickModifierAction, EKeys::Tab);
		Map(BlockAction, EKeys::LeftShift);
	}
	else
	{
		Map(MoveForwardAction, EKeys::Up);
		Map(MoveForwardAction, EKeys::Down).Modifiers.Add(NewObject<UInputModifierNegate>());
		Map(MoveRightAction, EKeys::Left).Modifiers.Add(NewObject<UInputModifierNegate>());
		Map(MoveRightAction, EKeys::Right);

		Map(FaceLeftAction, EKeys::I);
		Map(FaceTopAction, EKeys::O);
		Map(FaceRightAction, EKeys::P);
		Map(FaceBottomAction, EKeys::K);

		Map(KickModifierAction, EKeys::L);
		Map(BlockAction, EKeys::RightControl);
	}

	Map(MoveForwardAction, EKeys::Gamepad_LeftY).Modifiers.Add(NewObject<UInputModifierNegate>());
	Map(MoveRightAction, EKeys::Gamepad_LeftX);
	Map(FaceLeftAction, EKeys::Gamepad_FaceButton_Left);
	Map(FaceTopAction, EKeys::Gamepad_FaceButton_Top);
	Map(FaceRightAction, EKeys::Gamepad_FaceButton_Right);
	Map(FaceBottomAction, EKeys::Gamepad_FaceButton_Bottom);
	Map(KickModifierAction, EKeys::Gamepad_RightTrigger);
	Map(BlockAction, EKeys::Gamepad_RightShoulder);
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

	if (PlayerIndex == 1)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC && PC->PlayerInput)
		{
			float Up    = PC->PlayerInput->GetKeyValue(EKeys::Up);
			float Down  = PC->PlayerInput->GetKeyValue(EKeys::Down);
			float Left  = PC->PlayerInput->GetKeyValue(EKeys::Left);
			float Right = PC->PlayerInput->GetKeyValue(EKeys::Right);
			AddMovementInput(FVector(0.f, -1.f, 0.f), Up - Down);
			AddMovementInput(FVector(1.f, 0.f, 0.f), Right - Left);

			bool bKick = PC->PlayerInput->GetKeyValue(EKeys::L) > 0.f;
			if (bKick)
			{
				if (PC->WasInputKeyJustPressed(EKeys::I)) RearKickLow();
				if (PC->WasInputKeyJustPressed(EKeys::O)) RearKickMid();
				if (PC->WasInputKeyJustPressed(EKeys::P)) RearKickHigh();
				if (PC->WasInputKeyJustPressed(EKeys::K)) RearKickBody();
			}
			else
			{
				if (PC->WasInputKeyJustPressed(EKeys::I)) LeadJab();
				if (PC->WasInputKeyJustPressed(EKeys::O)) LeadCross();
				if (PC->WasInputKeyJustPressed(EKeys::P)) LeadHook();
				if (PC->WasInputKeyJustPressed(EKeys::K)) LeadUppercut();
			}

			if (PC->PlayerInput->GetKeyValue(EKeys::RightControl) > 0.f)
			{
				if (!bIsBlocking) StartBlock();
			}
			else
			{
				if (bIsBlocking) StopBlock();
			}
		}
	}

	if (GEngine && (GetWorld() && GetWorld()->TimeSeconds - LastDebugTime > 0.5f))
	{
		LastDebugTime = GetWorld()->TimeSeconds;
		FColor C = GetMesh()->GetSkeletalMeshAsset() ? FColor::Green : FColor::Yellow;
		GEngine->AddOnScreenDebugMessage(1, 5.f, C, C == FColor::Green ? TEXT("Mannequin") : TEXT("Cylinder"));
	}
}

void AFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	SetupEnhancedInput();

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

	PlayerInputComponent->BindAxisKey(EKeys::Gamepad_LeftX, this, &AFighterCharacter::MoveRightLegacy);
	PlayerInputComponent->BindAxisKey(EKeys::Gamepad_LeftY, this, &AFighterCharacter::MoveForwardLegacy);

	PlayerInputComponent->BindAction("Jab", IE_Pressed, this, &AFighterCharacter::LeadJab);
	PlayerInputComponent->BindAction("Cross", IE_Pressed, this, &AFighterCharacter::LeadCross);
	PlayerInputComponent->BindAction("Hook", IE_Pressed, this, &AFighterCharacter::LeadHook);
	PlayerInputComponent->BindAction("Uppercut", IE_Pressed, this, &AFighterCharacter::LeadUppercut);
	PlayerInputComponent->BindAction("LowKick", IE_Pressed, this, &AFighterCharacter::RearKickLow);
	PlayerInputComponent->BindAction("MidKick", IE_Pressed, this, &AFighterCharacter::RearKickMid);
	PlayerInputComponent->BindAction("HighKick", IE_Pressed, this, &AFighterCharacter::RearKickHigh);
	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &AFighterCharacter::StartBlock);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &AFighterCharacter::StopBlock);
}

void AFighterCharacter::MoveForward(const FInputActionValue& Value)
{
	AddMovementInput(FVector(0.0f, -1.0f, 0.0f), Value.Get<float>());
}

void AFighterCharacter::MoveRight(const FInputActionValue& Value)
{
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value.Get<float>());
}

void AFighterCharacter::MoveForwardLegacy(float Value)
{
	AddMovementInput(FVector(0.0f, -1.0f, 0.0f), Value);
}

void AFighterCharacter::MoveRightLegacy(float Value)
{
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void AFighterCharacter::LeadJab()
{
	if (bKickModifierHeld) { RearKickLow(); return; }
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostLight)) return;
	PerformAttackTrace(JabDamage);
	PlayAnimMontage(JabMontage);
}

void AFighterCharacter::LeadCross()
{
	if (bKickModifierHeld) { RearKickMid(); return; }
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostLight)) return;
	PerformAttackTrace(CrossDamage);
	PlayAnimMontage(CrossMontage);
}

void AFighterCharacter::LeadHook()
{
	if (bKickModifierHeld) { RearKickHigh(); return; }
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostHeavy)) return;
	PerformAttackTrace(HookDamage);
	PlayAnimMontage(HookMontage);
}

void AFighterCharacter::LeadUppercut()
{
	if (bKickModifierHeld) { RearKickBody(); return; }
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostHeavy)) return;
	PerformAttackTrace(UppercutDamage);
	PlayAnimMontage(UppercutMontage);
}

void AFighterCharacter::RearKickLow()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostLight)) return;
	PerformAttackTrace(LowKickDamage);
	PlayAnimMontage(LowKickMontage);
}

void AFighterCharacter::RearKickMid()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostLight)) return;
	PerformAttackTrace(MidKickDamage);
	PlayAnimMontage(MidKickMontage);
}

void AFighterCharacter::RearKickHigh()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostHeavy)) return;
	PerformAttackTrace(HighKickDamage);
	PlayAnimMontage(HighKickMontage);
}

void AFighterCharacter::RearKickBody()
{
	if (!HealthComponent || !HealthComponent->ConsumeStamina(StaminaCostHeavy)) return;
	PerformAttackTrace(BodyKickDamage);
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

void AFighterCharacter::HitReact()
{
	if (HitReactMontage)
	{
		PlayAnimMontage(HitReactMontage);
	}
}

void AFighterCharacter::OnDeath()
{
	bIsDead = true;
	GetCharacterMovement()->DisableMovement();
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	SpawnBloodEffect(GetActorLocation());

	AFightGameMode* GM = Cast<AFightGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->EndFight(this);
	}

	SetLifeSpan(3.0f);
}

bool AFighterCharacter::IsOpponent(AFighterCharacter* Other) const
{
	return Other && Other != this && !Other->bIsDead;
}

bool AFighterCharacter::PerformAttackTrace(float Damage)
{
	if (!GetWorld()) return false;

	FVector Start = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	FVector End = Start + Forward * AttackRange;

	TArray<AActor*> Ignored;
	Ignored.Add(this);

	FHitResult Hit;
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		GetWorld(),
		Start, End,
		AttackRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false,
		Ignored,
		EDrawDebugTrace::ForDuration,
		Hit,
		true
	);

	if (bHit && Hit.GetActor())
	{
		AFighterCharacter* Victim = Cast<AFighterCharacter>(Hit.GetActor());
		if (IsOpponent(Victim) && Victim->HealthComponent)
		{
			float FinalDamage = Damage;
			if (Victim->bIsBlocking)
			{
				FinalDamage *= BlockDamageMultiplier;
			}

			Victim->HealthComponent->TakeDamage(FinalDamage);
			Victim->HitReact();
			SpawnBloodEffect(Hit.Location);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Orange,
					FString::Printf(TEXT("%.0f dmg"), FinalDamage),
					true, FVector2D(4.f, 4.f));
			}

			FVector KnockDir = (Victim->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			KnockDir.Z = 0.2f;
			Victim->LaunchCharacter(KnockDir * KnockbackForce, false, false);

			return true;
		}
	}
	return false;
}

void AFighterCharacter::SpawnBloodEffect(const FVector& Location)
{
	if (!GetWorld()) return;

	DrawDebugSphere(GetWorld(), Location, 15.f, 8, FColor::Red, false, 0.4f);

	for (int32 i = 0; i < 5; i++)
	{
		FVector Dir = FMath::VRand();
		Dir.Z = FMath::Abs(Dir.Z);
		FVector End = Location + Dir * FMath::RandRange(20.f, 60.f);
		DrawDebugLine(GetWorld(), Location, End, FColor::Red, false, 0.3f);
	}
}
