// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ERGameModeBase.generated.h"

UCLASS()
class ESCAPEROOM_API AERGameModeBase : public AGameModeBase
{
	GENERATED_BODY()protected:
	virtual void BeginPlay() override;

	FTimerHandle GameTimerHandle;

	void UpdateTimer();

	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 StartingSeconds = 600;

	void OnTimerEnd();
	
	UFUNCTION(BlueprintCallable, Category = "Game Control")
	void EndGame(bool bPlayerWon);
};
