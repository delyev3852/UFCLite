#include "FightGameMode.h"
#include "FighterCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

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

	int32 SpawnIndex = 0;
	for (AActor* Start : PlayerStarts)
	{
		if (SpawnIndex >= 2) break;

		APlayerStart* PlayerStart = Cast<APlayerStart>(Start);
		if (!PlayerStart) continue;

		FActorSpawnParameters SpawnParams;
		AFighterCharacter* Fighter = GetWorld()->SpawnActor<AFighterCharacter>(FighterClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParams);

		if (Fighter)
		{
			SetupFighterInput(Fighter, SpawnIndex);

			if (SpawnIndex == 0) Fighter1 = Fighter;
			else Fighter2 = Fighter;
		}

		SpawnIndex++;
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
