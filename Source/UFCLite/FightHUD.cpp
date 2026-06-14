#include "FightHUD.h"
#include "FighterCharacter.h"
#include "HealthComponent.h"
#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"
#include "FightGameMode.h"

void AFightHUD::DrawHUD()
{
	Super::DrawHUD();

	AFightGameMode* GM = Cast<AFightGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GM) return;

	AFighterCharacter* F1 = GM->GetFighter1();
	AFighterCharacter* F2 = GM->GetFighter2();
	if (!F1 || !F2) return;

	float HealthWidth = Canvas->ClipX * 0.4f;
	float HealthHeight = 20.0f;
	float StaminaHeight = 12.0f;
	float Y1 = 30.0f;
	float Y2 = 60.0f;

	DrawFighterBars(F1, Canvas->ClipX * 0.05f, Y1, HealthWidth, StaminaHeight);
	DrawFighterBars(F2, Canvas->ClipX * 0.55f, Y1, HealthWidth, StaminaHeight);
}

void AFightHUD::DrawFighterBars(AFighterCharacter* Fighter, float X, float Y, float Width, float Height)
{
	if (!Fighter || !Fighter->HealthComponent) return;

	UHealthComponent* HC = Fighter->HealthComponent;

	float HealthPct = HC->GetHealth() / HC->MaxHealth;
	float StaminaPct = HC->GetStamina() / HC->MaxStamina;

	DrawRect(FLinearColor::Black, X - 1, Y - 1, Width + 2, Height + 2);
	DrawRect(FLinearColor(0.2f, 0.0f, 0.0f), X, Y, Width, Height);
	DrawRect(FLinearColor::Red, X, Y, Width * HealthPct, Height);

	float StaminaY = Y + Height + 3;
	DrawRect(FLinearColor::Black, X - 1, StaminaY - 1, Width + 2, 10 + 2);
	DrawRect(FLinearColor(0.0f, 0.0f, 0.2f), X, StaminaY, Width, 10);
	DrawRect(FLinearColor::Blue, X, StaminaY, Width * StaminaPct, 10);
}
