#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FightGameMode.generated.h"

class AFighterCharacter;

UCLASS()
class UFCLITE_API AFightGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFightGameMode();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AFighterCharacter> FighterClass;

	UFUNCTION(BlueprintCallable)
	void EndFight(AFighterCharacter* Winner);

protected:
	void SpawnFighters();
	void SetupFighterInput(AFighterCharacter* Fighter, int32 PlayerIndex);

	UPROPERTY()
	AFighterCharacter* Fighter1;

	UPROPERTY()
	AFighterCharacter* Fighter2;
};
