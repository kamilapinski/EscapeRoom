// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupItem.h"
#include "Components/StaticMeshComponent.h"
#include "ERPlayerPawn.h"


// Sets default values
APickupItem::APickupItem()
{
	PrimaryActorTick.bCanEverTick = true;

	UE_LOG(LogTemp, Warning, TEXT("ItemMesh initialized"));
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;
	ItemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void APickupItem::BeginPlay()
{
	Super::BeginPlay();

	if (ItemMesh)
	{
		InitialRelativeScale = ItemMesh->GetRelativeScale3D();
		InitialRelativeRotation = ItemMesh->GetRelativeRotation();
        
		UE_LOG(LogTemp, Warning, TEXT("Startowa skala przedmiotu: %s"), *InitialRelativeScale.ToString());
	}
}

void APickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupItem::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("Interact Implementation called"));
	if (!Interactor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interactor is NULL"));
		return;
	}

	AERPlayerPawn* PlayerPawn = Cast<AERPlayerPawn>(Interactor);

	if (PlayerPawn && PlayerPawn->GetHoldLocation())
	{
		PlayerPawn->DropItem();
		UE_LOG(LogTemp, Warning, TEXT("Got PlayerPawn's hold location"));
		if (ItemMesh)
		{
			ItemMesh->SetSimulatePhysics(false);
			ItemMesh->PutRigidBodyToSleep();
			ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			
			ItemMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
			ItemMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		}
		else 
		{
			UE_LOG(LogTemp, Error, TEXT("ItemMesh jest NULL w %s!"), *GetName());
			return;
		}
		SetActorEnableCollision(false);
	
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, 
			EAttachmentRule::SnapToTarget, 
			EAttachmentRule::KeepWorld, 
			false
			);
		AttachToComponent(PlayerPawn->GetHoldLocation(), AttachmentRules);
		
		FVector Offset(150.0f, 30.0f, -30.0f);
		this->SetActorRelativeLocation(Offset);

		this->SetActorRelativeScale3D(InitialRelativeScale);

		FVector RelLoc = GetRootComponent()->GetRelativeLocation();
		UE_LOG(LogTemp, Warning, TEXT("Po doczepieniu Relative Location to: %s"), *RelLoc.ToString());

		this->SetActorRelativeLocation(FVector::ZeroVector);
		this->SetActorRelativeRotation(FRotator::ZeroRotator);
		UE_LOG(LogTemp, Error, TEXT("Przedmiot %s przyczepiony do HoldLocation!"), *GetName());

		PlayerPawn->SetHeldItem(this);

		ItemMesh->SetOwnerNoSee(false);
		ItemMesh->bCastHiddenShadow = true;
		this->SetActorHiddenInGame(false);
	}
	
}

void APickupItem::OnDropped()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	SetActorEnableCollision(true);

	this->SetActorRelativeScale3D(InitialRelativeScale);
	this->SetActorRelativeRotation(InitialRelativeRotation);

	if (ItemMesh)
	{
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Block);
        
		ItemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

		ItemMesh->SetSimulatePhysics(true);
        
		ItemMesh->WakeRigidBody();
	}

	UE_LOG(LogTemp, Warning, TEXT("Przedmiot %s został upuszczony i fizyka przywrócona."), *GetName());
}


bool APickupItem::IsAKey_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Called IsAKey in Pickup Item"))
	UE_LOG(LogTemp, Warning, TEXT("Key status in this item: %d"), key);
	return key;
}