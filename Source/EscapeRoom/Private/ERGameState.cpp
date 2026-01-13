// Fill out your copyright notice in the Description page of Project Settings.


#include "ERGameState.h"

FText AERGameState::GetFormattedTime() const
{
	int32 Minutes = SecondsRemaining / 60;
	int32 Seconds = SecondsRemaining % 60;
	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
}