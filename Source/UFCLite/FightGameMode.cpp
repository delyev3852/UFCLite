#include "FightGameMode.h"
#include "FighterCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Engine/StaticMeshActor.h"
#include "UObject/ConstructorHelpers.h"

AFightGameMode::AFightGameMode()
{
	DefaultPawnClass = nullptr;

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
		Floor->SetMobility(EComponentMobility::Stationary);
		Floor->GetStaticMeshComponent()->SetStaticMesh(FloorMesh);
		Floor->SetActorScale3D(FVector(50.0f, 50.0f, 0.5f));
		Floor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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
		SpawnPos[0] = FVector(-200.0f, 0.0f, 100.0f);
		SpawnPos[1] = FVector(200.0f, 0.0f, 100.0f);
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

			APlayerController* PC = UGameplayStatics::GetPlayerController(this, i);
			if (PC)
			{
				PC->SetControlRotation(SpawnRot[i]);
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
