#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FightHUD.generated.h"

class AFighterCharacter;

UCLASS()
class UFCLITE_API AFightHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

protected:
	void DrawFighterBars(AFighterCharacter* Fighter, float X, float Y, float Width, float Height);
};
