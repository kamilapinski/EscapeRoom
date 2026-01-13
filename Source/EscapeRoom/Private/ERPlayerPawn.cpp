// Fill out your copyright notice in the Description page of Project Settings.


#include "ERPlayerPawn.h"

#include "DocumentAsset.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "IInteractableInterface.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "PickupItem.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

AERPlayerPawn::AERPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	UCapsuleComponent* Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	Capsule->InitCapsuleSize(35.f, 90.f);
	Capsule->SetCollisionProfileName(TEXT("Pawn"));
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // WYMUSZENIE KOLIZJI
	Capsule->SetCollisionObjectType(ECC_Pawn);
	Capsule->SetCollisionResponseToAllChannels(ECR_Block);
	Capsule->SetSimulatePhysics(false);
	RootComponent = Capsule;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(RootComponent);
	PlayerCamera->SetRelativeLocation(FVector(0.f, 0.f, 70.f));
	PlayerCamera->bUsePawnControlRotation = true;
    
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	HoldLocation = CreateDefaultSubobject<USceneComponent>(TEXT("HoldLocation"));
	HoldLocation->SetupAttachment(PlayerCamera);
	HoldLocation->SetRelativeLocation(FVector(100.0f, 40.0f, -20.0f));

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->bConstrainToPlane = true;
	MovementComponent->SetPlaneConstraintNormal(FVector(0, 0, 1));
}

void AERPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AERPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GetController()) return;

	FVector EyeLocation;
	FRotator EyeRotation;
	GetController()->GetPlayerViewPoint(EyeLocation, EyeRotation);

	FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * InteractionRange);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, ECC_Visibility, Params);

	AActor* HitActor = HitResult.GetActor();
	if (bHit && HitActor)
	{
		if (HitActor->Implements<UIInteractableInterface>())
		{
			UPrimitiveComponent* Mesh = Cast<UPrimitiveComponent>(HitActor->GetComponentByClass(UPrimitiveComponent::StaticClass()));

			if (Mesh && Mesh->IsRegistered())
			{
				if (CurrentHighlightedComponent != Mesh)
				{
					if (CurrentHighlightedComponent)
						CurrentHighlightedComponent->SetRenderCustomDepth(false);

					Mesh->SetRenderCustomDepth(true);
					Mesh->SetCustomDepthStencilValue(1);
					
					UE_LOG(LogTemp, Warning, TEXT("Włączono podświetlenie"));
					
					CurrentHighlightedComponent = Mesh;
				}
				return;
			}
		}
	}
	if (CurrentHighlightedComponent)
	{
		CurrentHighlightedComponent->SetRenderCustomDepth(false);
		CurrentHighlightedComponent = nullptr;
	}
}

void AERPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Player Pawn does not have player controller"));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!EISubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("UEnhancedInputLocalPlayerSubsystem is null"));
		return;
	}

	if (!PlayerInputMappingContext)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInputMappingContext is null"));
		return;
	}

	if (!MovementInputAction)
	{
		UE_LOG(LogTemp, Error, TEXT("MovementInputAction is null"));
		return;
	}

	if (!CameraInputAction)
	{
		UE_LOG(LogTemp, Error, TEXT("CameraInputAction is null"));
		return;
	}

	EISubsystem->AddMappingContext(PlayerInputMappingContext, 0);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerInputComponent is not UEnhancedInputComponent type. Enable EnhancedInput subsystem in settings."));
		return;
	}

	EIC->BindAction(MovementInputAction, ETriggerEvent::Triggered, this, &AERPlayerPawn::HandleMovementInput);
	EIC->BindAction(CameraInputAction, ETriggerEvent::Triggered, this, &AERPlayerPawn::HandleCameraInput);
	EIC->BindAction(InteractInputAction, ETriggerEvent::Started, this, &AERPlayerPawn::Interact);
	EIC->BindAction(MenuInputAction, ETriggerEvent::Started, this, &AERPlayerPawn::ToggleMenu);
	EIC->BindAction(DropInputAction, ETriggerEvent::Started, this, &AERPlayerPawn::DropItem);
}


void AERPlayerPawn::HandleMovementInput(const FInputActionValue& InputValue)
{
	const EInputActionValueType InputType = InputValue.GetValueType();

	switch (InputType)
	{
	case EInputActionValueType::Axis2D:
	{
			float DeltaTimeSeconds = GetWorld()->GetDeltaSeconds();
			FInputActionValue::Axis2D XYDelta = InputValue.Get<FInputActionValue::Axis2D>();
			XYDelta = XYDelta * DeltaTimeSeconds * AccelerationPerSecond;
			
			FRotator FullRotation = GetController()->GetControlRotation();
			FRotator YawRotation(0.f, FullRotation.Yaw, 0.f);
			
			FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			
			FVector RightVector = UKismetMathLibrary::GetRightVector(GetController()->GetControlRotation());
			RightVector.Normalize();
			
			ForwardVector *= XYDelta.X;
			RightVector *= XYDelta.Y;
			
			AddMovementInput(ForwardVector, 1.f, false);
			AddMovementInput(RightVector, 1.f, false);
			
			UE_LOG(LogTemp, Warning, TEXT("Dodaję ruch: Forward %s, Right %s"), *ForwardVector.ToString(), *RightVector.ToString())
			
			FVector CharacterDirection = ForwardVector + RightVector;
			CharacterDirection.Z = 0.f;
			CharacterDirection.Normalize();
			
			SetActorRotation(CharacterDirection.Rotation());
			break;
	}
	default:
		break;
	}
}


void AERPlayerPawn::HandleCameraInput(const FInputActionValue& InputValue)
{
	const EInputActionValueType InputType = InputValue.GetValueType();

	switch (InputType)
	{
	case EInputActionValueType::Axis2D:
	{
		FInputActionValue::Axis2D MouseXYDelta = InputValue.Get<FInputActionValue::Axis2D>();
		AddControllerYawInput(MouseXYDelta.X * MouseSensitivity);
		AddControllerPitchInput(-MouseXYDelta.Y * MouseSensitivity);
		break;
	}
	default:
		break;
	}
}

void AERPlayerPawn::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Interact function called"))
	
	if (ActiveDocumentWidget)
	{
		CloseDocument();
		return;
	}
	AActor* HeldBeforeInteract = CurrentHeldItem;

	FVector EyeLocation;
	FRotator EyeRotation;
	GetController()->GetPlayerViewPoint(EyeLocation, EyeRotation);

	FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * InteractionRange);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, ECC_Visibility, Params);

	if (!GetController()) return;

	bool executed = false;
	if (bHit && HitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Interact function hit: %s"), *HitResult.GetActor()->GetName());
		if (HitResult.GetActor()->Implements<UIInteractableInterface>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Executing interaction"));
			IIInteractableInterface::Execute_Interact(HitResult.GetActor(), this);
			executed = true;
		}
	}

	if (!executed && HeldBeforeInteract == CurrentHeldItem)
	{
		DropItem();
	}
}

void AERPlayerPawn::DropItem()
{
	if (!CurrentHeldItem) return;

	FVector EyeLocation;
	FRotator EyeRotation;
	GetController()->GetPlayerViewPoint(EyeLocation, EyeRotation);
	FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * InteractionRange);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(CurrentHeldItem);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, ECC_Visibility, Params);
	FVector FinalDropLocation = bHit ? HitResult.Location : TraceEnd;

	if (bHit && HitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());
	}

	FVector Diff = EyeLocation - FinalDropLocation;
	Diff.Normalize();
	FinalDropLocation += 7 * Diff;

	CurrentHeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	UE_LOG(LogTemp, Warning, TEXT("Final drop location: %s"), *FinalDropLocation.ToString());
	CurrentHeldItem->SetActorLocation(FinalDropLocation + FVector(0,0,10), true);
	CurrentHeldItem->SetActorRotation(FRotator::ZeroRotator);

	if (APickupItem* Pickup = Cast<APickupItem>(CurrentHeldItem))
	{
		Pickup->OnDropped();
	}

	CurrentHeldItem = nullptr;
}

void AERPlayerPawn::OpenDocument(class UDocumentAsset* Data)
{
	if (!DocumentWidgetClass || !Data) return;

	UE_LOG(LogTemp, Warning, TEXT("OpenDocument function called"))

	if (ActiveDocumentWidget)
	{
		CloseDocument();
		return;
	}

	ActiveDocumentWidget = CreateWidget<UUserWidget>(GetWorld(), DocumentWidgetClass);
	
	if (ActiveDocumentWidget)
	{
		UTextBlock* TitleWidget = Cast<UTextBlock>(ActiveDocumentWidget->GetWidgetFromName(TEXT("TextTitle")));
		if (TitleWidget)
		{
			TitleWidget->SetText(Data->DocumentData.Title);
		}

		UTextBlock* ContentWidget = Cast<UTextBlock>(ActiveDocumentWidget->GetWidgetFromName(TEXT("TextContent")));
		if (ContentWidget)
		{
			ContentWidget->SetText(Data->DocumentData.Content);
		}
		
		ActiveDocumentWidget->AddToViewport();
    
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(ActiveDocumentWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;

			if (MovementComponent)
			{
				MovementComponent->StopMovementImmediately();
			MovementComponent->SetUpdatedComponent(nullptr);
			}
		}
	}
}

void AERPlayerPawn::CloseDocument()
{
	UE_LOG(LogTemp, Warning, TEXT("CloseDocument function called"))
    
	if (ActiveDocumentWidget)
	{
		ActiveDocumentWidget->RemoveFromParent();
		ActiveDocumentWidget = nullptr; 
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
       
		if (MovementComponent)
		{
			MovementComponent->SetUpdatedComponent(RootComponent);
		}
	}
}

void AERPlayerPawn::ToggleMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("Próba przełączenia menu"));
	if (!MenuWidgetClass) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	if (!MenuWidgetInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tworzenie instancji widgetu menu"));
		MenuWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), MenuWidgetClass);
	}

	if (MenuWidgetInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Przełączenie widgetu menu"));
		if (!MenuWidgetInstance->IsInViewport())
		{
			MenuWidgetInstance->AddToViewport();
			PC->SetInputMode(FInputModeGameAndUI()); // Pozwala klikać w UI, ale postać może się ruszać (lub UIOnly dla pauzy)
			PC->bShowMouseCursor = true;
		}
		else
		{
			MenuWidgetInstance->RemoveFromParent();
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}
	}
}

void AERPlayerPawn::ToggleWinSummaryWidget()
{
	HideAllWidgets();
	UE_LOG(LogTemp, Warning, TEXT("Próba przełączenia win summary widget"));
	if (!WinSummaryWidgetClass) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	if (!WinSummaryWidgetInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tworzenie instancji widgetu win summary"));
		WinSummaryWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), WinSummaryWidgetClass);
	}	

	if (WinSummaryWidgetInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Przełączenie widgetu win summary"));
		if (!WinSummaryWidgetInstance->IsInViewport())
		{
			UE_LOG(LogTemp, Warning, TEXT("Otwieranie widgetu win summary"));
			WinSummaryWidgetInstance->AddToViewport();
			PC->SetInputMode(FInputModeGameAndUI());
			PC->bShowMouseCursor = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Zamykanie widgetu win summary"));
			WinSummaryWidgetInstance->RemoveFromParent();
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}
	}
}

void AERPlayerPawn::ToggleDefeatSummaryWidget()
{
	HideAllWidgets();
	UE_LOG(LogTemp, Warning, TEXT("Próba przełączenia defeat summary widget"));
	if (!DefeatSummaryWidgetClass) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	if (!DefeatSummaryWidgetInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tworzenie instancji widgetu defeat summary"));
		DefeatSummaryWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), DefeatSummaryWidgetClass);
	}	

	if (DefeatSummaryWidgetInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Przełączenie widgetu defeat summary"));
		if (!DefeatSummaryWidgetInstance->IsInViewport())
		{
			UE_LOG(LogTemp, Warning, TEXT("Otwieranie widgetu defeat summary"));
			DefeatSummaryWidgetInstance->AddToViewport();
			PC->SetInputMode(FInputModeGameAndUI());
			PC->bShowMouseCursor = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Zamykanie widgetu defeat summary"));
			DefeatSummaryWidgetInstance->RemoveFromParent();
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}
	}
}

void AERPlayerPawn::HideAllWidgets()
{
	
	if (MenuWidgetInstance && MenuWidgetInstance->IsInViewport())
	{
		UE_LOG(LogTemp, Warning, TEXT("Zamykanie widgetu menu"));
		MenuWidgetInstance->RemoveFromParent();
	}
    
	if (WinSummaryWidgetInstance && WinSummaryWidgetInstance->IsInViewport())
	{
		UE_LOG(LogTemp, Warning, TEXT("Zamykanie widgetu win summary"));
		WinSummaryWidgetInstance->RemoveFromParent();
	}
    
	if (DefeatSummaryWidgetInstance && DefeatSummaryWidgetInstance->IsInViewport())
	{
		UE_LOG(LogTemp, Warning, TEXT("Zamykanie widgetu defeat summary"));
		DefeatSummaryWidgetInstance->RemoveFromParent();
	}
	
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;
	PC->SetInputMode(FInputModeGameOnly());
	PC->bShowMouseCursor = false;
}

bool AERPlayerPawn::CanOpenDoor()
{
	if (CurrentHeldItem && CurrentHeldItem->Implements<UIInteractableInterface>())
	{
		return IIInteractableInterface::Execute_IsAKey(CurrentHeldItem);
	}
	UE_LOG(LogTemp, Warning, TEXT("No held item or held item doesn't implements interactable interface"));
	return false;
}