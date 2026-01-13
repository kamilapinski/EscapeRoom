// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionItem.h"
#include "ERPlayerPawn.h"


AActionItem::AActionItem()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));

	HelpPivot = CreateDefaultSubobject<USceneComponent>(TEXT("HelpPivot"));
	HelpPivot->SetupAttachment(RootComponent);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(HelpPivot);
}

void AActionItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AActionItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HelpPivot)
	{
		FVector CurrentLoc = HelpPivot->GetRelativeLocation();
		FRotator CurrentRot = HelpPivot->GetRelativeRotation();

		FVector TargetLoc = bIsLocked ? FVector::ZeroVector : TargetLocationOffset;
		FRotator TargetRot = bIsLocked ? FRotator::ZeroRotator : TargetRotationOffset;

		FVector NewLoc = FMath::VInterpTo(CurrentLoc, TargetLoc, DeltaTime, 5.0f);
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, 5.0f);

		FHitResult SweepHitResult;
		HelpPivot->SetRelativeLocationAndRotation(NewLoc, NewRot, false);
	}
}

void AActionItem::Interact_Implementation(AActor* Interactor)
{
	bIsLocked = !bIsLocked;

	UE_LOG(LogTemp, Warning, TEXT("Item state changed. Is Locked: %s"), bIsLocked ? TEXT("True") : TEXT("False"));
}

bool AActionItem::IsAKey_Implementation()
{
	return false;
}