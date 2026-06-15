#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/StaticMesh.h"
#include "FightGameMode.generated.h"

class AFighterCharacter;
class ACameraActor;

UCLASS()
class UFCLITE_API AFightGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFightGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AFighterCharacter> FighterClass;

	UFUNCTION(BlueprintCallable)
	void EndFight(AFighterCharacter* Winner);

#if WITH_EDITOR
	void ImportCharactersFromFbx();
#endif

	UFUNCTION(BlueprintPure)
	AFighterCharacter* GetFighter1() const { return Fighter1; }

	UFUNCTION(BlueprintPure)
	AFighterCharacter* GetFighter2() const { return Fighter2; }

	bool bFightOver = false;
	AFighterCharacter* Winner = nullptr;

protected:
	void SpawnArenaFloor();
	void SpawnArenaCamera();
	void SpawnFighters();
	void SetupFighterInput(AFighterCharacter* Fighter, int32 PlayerIndex);

	UPROPERTY()
	UStaticMesh* FloorMesh;

	UPROPERTY()
	AFighterCharacter* Fighter1;

	UPROPERTY()
	AFighterCharacter* Fighter2;

	UPROPERTY()
	ACameraActor* ArenaCamera;
};
