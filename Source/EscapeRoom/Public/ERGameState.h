// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ERGameState.generated.h"


UCLASS()
class ESCAPEROOM_API AERGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Timer")
	int32 SecondsRemaining;

	UFUNCTION(BlueprintPure, Category = "Timer")
	FText GetFormattedTime() const;
	
};
