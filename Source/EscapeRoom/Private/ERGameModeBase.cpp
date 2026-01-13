// Fill out your copyright notice in the Description page of Project Settings.


#include "ERGameModeBase.h"
#include "ERGameState.h"
#include "ERPlayerPawn.h"

void AERGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	AERGameState* GS = GetGameState<AERGameState>();
	if (GS)
	{
		GS->SecondsRemaining = StartingSeconds;
        
		GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AERGameModeBase::UpdateTimer, 1.0f, true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Brak GameState. Sprawdz ustawienia GameMode."));
	}
}

void AERGameModeBase::UpdateTimer()
{
	AERGameState* GS = GetGameState<AERGameState>();
	if (GS)
	{
		GS->SecondsRemaining--;

		if (GS->SecondsRemaining <= 0)
		{
			GetWorldTimerManager().ClearTimer(GameTimerHandle);
			OnTimerEnd();
		}
	}
}

void AERGameModeBase::OnTimerEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("Czas minął"));
	EndGame(false);
}

void AERGameModeBase::EndGame(bool bPlayerWon)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	AERPlayerPawn* PlayerPawn = Cast<AERPlayerPawn>(PC->GetPawn());

	PC->SetPause(true); 
	PC->bShowMouseCursor = true;
    
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);

	if (PlayerPawn)
	{
		if (bPlayerWon) {
			UE_LOG(LogTemp, Warning, TEXT("Gracz wygrał"))
			PlayerPawn->ToggleWinSummaryWidget(); 
		} else {
			UE_LOG(LogTemp, Warning, TEXT("Gracz przegrał"));
			PlayerPawn->ToggleDefeatSummaryWidget(); 
		}
	}
}