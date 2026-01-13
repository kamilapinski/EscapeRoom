// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IInteractableInterface.h"
#include "GameFramework/Actor.h"
#include "ActionItem.generated.h"

UCLASS()
class ESCAPEROOM_API AActionItem : public AActor, public IIInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AActionItem();
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	USceneComponent* HelpPivot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemMesh")
	UStaticMeshComponent* ItemMesh;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual bool IsAKey_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bIsLocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FRotator TargetRotationOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FVector TargetLocationOffset;
};
