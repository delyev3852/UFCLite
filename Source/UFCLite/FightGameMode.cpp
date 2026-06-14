#include "FightGameMode.h"
#include "FighterCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AFightGameMode::AFightGameMode()
{
	DefaultPawnClass = AFighterCharacter::StaticClass();
}

void AFightGameMode::BeginPlay()
{
	Super::BeginPlay();
	SpawnFighters();
}

void AFightGameMode::SpawnFighters()
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

	if (PlayerStarts.Num() >= 2)
	{
		for (int32 i = 0; i < 2; i++)
		{
			APlayerStart* PS = Cast<APlayerStart>(PlayerStarts[i]);
			if (!PS) continue;

			FActorSpawnParameters SpawnParams;
			AFighterCharacter* Fighter = GetWorld()->SpawnActor<AFighterCharacter>(FighterClass, PS->GetActorLocation(), PS->GetActorRotation(), SpawnParams);
			if (Fighter)
			{
				SetupFighterInput(Fighter, i);
				if (i == 0) Fighter1 = Fighter;
				else Fighter2 = Fighter;
			}
		}
	}
	else
	{
		FVector SpawnPositions[] = {
			FVector(-200.0f, 0.0f, 100.0f),
			FVector(200.0f, 0.0f, 100.0f)
		};
		FRotator SpawnRotations[] = {
			FRotator(0.0f, 0.0f, 0.0f),
			FRotator(0.0f, -180.0f, 0.0f)
		};

		for (int32 i = 0; i < 2; i++)
		{
			FActorSpawnParameters SpawnParams;
			AFighterCharacter* Fighter = GetWorld()->SpawnActor<AFighterCharacter>(FighterClass, SpawnPositions[i], SpawnRotations[i], SpawnParams);
			if (Fighter)
			{
				SetupFighterInput(Fighter, i);
				if (i == 0) Fighter1 = Fighter;
				else Fighter2 = Fighter;
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
