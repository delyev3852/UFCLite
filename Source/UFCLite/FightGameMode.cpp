#include "FightGameMode.h"
#include "FighterCharacter.h"
#include "FightHUD.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/StaticMeshActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Framework/Application/SlateApplication.h"

AFightGameMode::AFightGameMode()
{
	DefaultPawnClass = nullptr;
	HUDClass = AFightHUD::StaticClass();
	FighterClass = AFighterCharacter::StaticClass();

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FloorAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (FloorAsset.Succeeded())
	{
		FloorMesh = FloorAsset.Object;
	}
}

void AFightGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnArenaFloor();
	SpawnArenaCamera();

	TSharedPtr<GenericApplication> GenApp = FSlateApplication::Get().GetPlatformApplication();
	if (GenApp.IsValid())
	{
		bool bGamepad = GenApp->IsGamepadAttached();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(99, 10.f, bGamepad ? FColor::Green : FColor::Red,
				FString::Printf(TEXT("Gamepad attached: %s"), bGamepad ? TEXT("YES") : TEXT("NO")));
		}

	}

	SpawnFighters();
}

void AFightGameMode::SpawnArenaFloor()
{
	if (!FloorMesh || !GetWorld()) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AStaticMeshActor* Floor = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(0.f, 0.f, -60.f), FRotator::ZeroRotator, SpawnParams);
	if (Floor)
	{
		Floor->SetMobility(EComponentMobility::Movable);
		Floor->GetStaticMeshComponent()->SetStaticMesh(FloorMesh);
		Floor->SetActorScale3D(FVector(8.f, 8.f, 0.2f));
		Floor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	const float Radius = 400.f;
	const float PostHeight = 300.f;
	const float Deg22_5 = 22.5f * UE_PI / 180.f;
	const TCHAR* OctMatPath = TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial");

	for (int32 i = 0; i < 8; i++)
	{
		float Angle = Deg22_5 + i * UE_PI / 4.f;
		FVector PostPos(Radius * FMath::Cos(Angle), Radius * FMath::Sin(Angle), PostHeight * 0.5f - 50.f);
		AStaticMeshActor* Post = GetWorld()->SpawnActor<AStaticMeshActor>(PostPos, FRotator::ZeroRotator, SpawnParams);
		if (Post)
		{
			Post->SetMobility(EComponentMobility::Movable);
			Post->GetStaticMeshComponent()->SetStaticMesh(FloorMesh);
			Post->SetActorScale3D(FVector(0.3f, 0.3f, PostHeight * 0.01f));
			Post->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}

		float NextAngle = Deg22_5 + ((i + 1) % 8) * UE_PI / 4.f;
		FVector NextPos(Radius * FMath::Cos(NextAngle), Radius * FMath::Sin(NextAngle), 0.f);
		FVector Mid = (FVector(Radius * FMath::Cos(Angle), Radius * FMath::Sin(Angle), 0.f) + NextPos) * 0.5f;
		float SegLen = FVector::Dist(PostPos, FVector(Radius * FMath::Cos(NextAngle), Radius * FMath::Sin(NextAngle), PostHeight * 0.5f - 50.f));
		FVector SegDir = (NextPos - FVector(Radius * FMath::Cos(Angle), Radius * FMath::Sin(Angle), 0.f)).GetSafeNormal();

		for (int32 r = 0; r < 3; r++)
		{
			float H = 40.f + r * 90.f;
			FVector RailPos(Mid.X, Mid.Y, H);
			AStaticMeshActor* Rail = GetWorld()->SpawnActor<AStaticMeshActor>(RailPos, FRotator::ZeroRotator, SpawnParams);
			if (Rail)
			{
				Rail->SetMobility(EComponentMobility::Movable);
				Rail->GetStaticMeshComponent()->SetStaticMesh(FloorMesh);
				Rail->SetActorScale3D(FVector(SegLen * 0.01f, 0.15f, 0.15f));
				float Yaw = FMath::Atan2(SegDir.Y, SegDir.X) * 180.f / UE_PI;
				Rail->SetActorRotation(FRotator(0.f, Yaw, 0.f));
				Rail->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}
		}
	}
}

void AFightGameMode::SpawnArenaCamera()
{
	if (!GetWorld()) return;

	FActorSpawnParameters SpawnParams;
	FVector CamPos = FVector(0.0f, 500.0f, 200.0f);
	ArenaCamera = GetWorld()->SpawnActor<ACameraActor>(CamPos, FRotator::ZeroRotator, SpawnParams);
	if (ArenaCamera)
	{
		ArenaCamera->SetActorRotation((FVector::ZeroVector - CamPos).Rotation());
	}
}

void AFightGameMode::SpawnFighters()
{
	FVector SpawnPos[2];
	FRotator SpawnRot[2];

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

	if (PlayerStarts.Num() >= 2)
	{
		for (int32 i = 0; i < 2; i++)
		{
			APlayerStart* PS = Cast<APlayerStart>(PlayerStarts[i]);
			if (PS)
			{
				SpawnPos[i] = PS->GetActorLocation();
				SpawnRot[i] = PS->GetActorRotation();
			}
		}
	}
	else
	{
		SpawnPos[0] = FVector(-150.0f, 0.0f, 100.0f);
		SpawnPos[1] = FVector(150.0f, 0.0f, 100.0f);
		SpawnRot[0] = FRotator(0.0f, 0.0f, 0.0f);
		SpawnRot[1] = FRotator(0.0f, -180.0f, 0.0f);
	}

	for (int32 i = 0; i < 2; i++)
	{
		FActorSpawnParameters SpawnParams;
		AFighterCharacter* Fighter = GetWorld()->SpawnActor<AFighterCharacter>(FighterClass, SpawnPos[i], SpawnRot[i], SpawnParams);
		if (Fighter)
		{
			SetupFighterInput(Fighter, i);
			if (i == 0) Fighter1 = Fighter;
			else Fighter2 = Fighter;
		}
	}

	if (ArenaCamera)
	{
		for (int32 i = 0; i < 2; i++)
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(this, i);
			if (PC)
			{
				PC->SetViewTarget(ArenaCamera);
			}
		}
	}
}

void AFightGameMode::SetupFighterInput(AFighterCharacter* Fighter, int32 InPlayerIndex)
{
	if (!Fighter) return;

	Fighter->PlayerIndex = InPlayerIndex;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && InPlayerIndex == 0)
	{
		PC->Possess(Fighter);
	}
}

void AFightGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ArenaCamera && Fighter1 && Fighter2)
	{
		FVector MidPoint = (Fighter1->GetActorLocation() + Fighter2->GetActorLocation()) * 0.5f;
		float Dist = FVector::Dist(Fighter1->GetActorLocation(), Fighter2->GetActorLocation());
		Dist = FMath::Clamp(Dist, 300.f, 800.f);
		FVector CamPos = MidPoint + FVector(0.f, Dist * 0.8f, 200.f);
		ArenaCamera->SetActorLocation(CamPos);
		ArenaCamera->SetActorRotation((MidPoint - CamPos).Rotation());
	}
}

void AFightGameMode::EndFight(AFighterCharacter* Loser)
{
	if (!Loser || bFightOver) return;

	bFightOver = true;
	Winner = (Loser == Fighter1) ? Fighter2 : Fighter1;

	if (Winner)
	{
		Winner->GetCharacterMovement()->DisableMovement();
	}

	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, [this]()
	{
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}, 5.0f, false);
}
