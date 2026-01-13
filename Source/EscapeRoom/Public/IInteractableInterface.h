// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInteractableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UIInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class ESCAPEROOM_API IIInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Interact(AActor* Interactor);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool IsAKey();
};
