// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IInteractableInterface.h"
#include "GameFramework/Actor.h"
#include "PickupItem.generated.h"

UCLASS()
class ESCAPEROOM_API APickupItem : public AActor, public IIInteractableInterface
{
	GENERATED_BODY()

public:
	APickupItem();
	
	void OnDropped();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	FVector InitialRelativeScale;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	FRotator InitialRelativeRotation;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	bool key;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual bool IsAKey_Implementation() override;
};
