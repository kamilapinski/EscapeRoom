// Fill out your copyright notice in the Description page of Project Settings.


#include "DocumentActor.h"
#include "ERPlayerPawn.h"

ADocumentActor::ADocumentActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ADocumentActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADocumentActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADocumentActor::Interact_Implementation(AActor* Interactor)
{
	if (!Interactor) return;
	
	AERPlayerPawn* Player = Cast<AERPlayerPawn>(Interactor);
	if (Player && DocumentData)
	{
		Player->OpenDocument(DocumentData);
	}
}

bool ADocumentActor::IsAKey_Implementation()
{
	return false;
}