// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IInteractableInterface.h"
#include "DocumentActor.generated.h"

UCLASS()
class ESCAPEROOM_API ADocumentActor : public AActor, public IIInteractableInterface
{
	GENERATED_BODY()
	
public:	
	ADocumentActor();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = "Document")
	class UDocumentAsset* DocumentData;
	
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual bool IsAKey_Implementation() override;
};
