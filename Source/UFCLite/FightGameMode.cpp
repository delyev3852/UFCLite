#include "FightGameMode.h"
#include "FighterCharacter.h"
#include "FightHUD.h"
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
	AStaticMeshActor* Floor = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(0.0f, 0.0f, -60.0f), FRotator(0.0f, 0.0f, 0.0f), SpawnParams);
	if (Floor)
	{
		Floor->SetMobility(EComponentMobility::Movable);
		Floor->GetStaticMeshComponent()->SetStaticMesh(FloorMesh);
		Floor->SetActorScale3D(FVector(500.0f, 50.0f, 0.5f));
		Floor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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

void AFightGameMode::SetupFighterInput(AFighterCharacter* Fighter, int32 PlayerIndex)
{
	if (!Fighter) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, PlayerIndex);
	if (PC)
	{
		PC->Possess(Fighter);
	}
}

void AFightGameMode::EndFight(AFighterCharacter* Winner)
{
	if (!Winner) return;
}
