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

	float BarW = 300.0f;
	float BarH = 20.0f;
	float Pad = 30.0f;
	float Top = 20.0f;

	DrawFighterBars(F1, Pad, Top, BarW, BarH);
	DrawFighterBars(F2, Canvas->SizeX - Pad - BarW, Top, BarW, BarH);

	if (GM->bFightOver && GM->Winner)
	{
		FString KoText = GM->Winner->PlayerIndex == 0 ? TEXT("PLAYER 1 WINS!") : TEXT("PLAYER 2 WINS!");
		float TW, TH;
		GetTextSize(KoText, TW, TH, nullptr, 3.f);
		FLinearColor KoColor = GM->Winner->PlayerIndex == 0 ? FLinearColor::Red : FLinearColor::Blue;
		DrawText(KoText, KoColor, (Canvas->SizeX - TW) * 0.5f, (Canvas->SizeY - TH) * 0.5f, nullptr, 3.f);
	}
}

void AFightHUD::DrawFighterBars(AFighterCharacter* Fighter, float X, float Y, float Width, float Height)
{
	if (!Fighter || !Fighter->HealthComponent) return;

	UHealthComponent* HC = Fighter->HealthComponent;
	float HP = HC->GetHealth() / HC->MaxHealth;
	float SP = HC->GetStamina() / HC->MaxStamina;

	DrawRect(FLinearColor::Black, X - 2, Y - 2, Width + 4, Height + 4);
	DrawRect(FLinearColor(0.3f, 0.0f, 0.0f), X, Y, Width, Height);
	DrawRect(FLinearColor::Red, X, Y, Width * HP, Height);

	float SY = Y + Height + 4;
	DrawRect(FLinearColor::Black, X - 2, SY - 2, Width + 4, 12 + 4);
	DrawRect(FLinearColor(0.0f, 0.0f, 0.2f), X, SY, Width, 12);
	DrawRect(FLinearColor::Blue, X, SY, Width * SP, 12);
}
