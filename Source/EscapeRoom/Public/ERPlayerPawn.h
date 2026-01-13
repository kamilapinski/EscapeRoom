// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "ERPlayerPawn.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

UCLASS()
class ESCAPEROOM_API AERPlayerPawn : public ADefaultPawn
{
	GENERATED_BODY()

public:
	AERPlayerPawn();

	USceneComponent* GetHoldLocation() const { return HoldLocation; }

protected:
	virtual void BeginPlay() override;
	
	void Interact();
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractionRange = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractInputAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* PlayerCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* HoldLocation;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	AActor* CurrentHeldItem;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> DocumentWidgetClass;
	
	UPROPERTY()
	class UUserWidget* ActiveDocumentWidget;

	UPROPERTY()
	class UPrimitiveComponent* CurrentHighlightedComponent;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> MenuWidgetClass;

	UPROPERTY()
	UUserWidget* MenuWidgetInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MenuInputAction;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> WinSummaryWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> DefeatSummaryWidgetClass;
	
	UPROPERTY()
	UUserWidget* WinSummaryWidgetInstance;
	
	UPROPERTY()
	UUserWidget* DefeatSummaryWidgetInstance;


public:	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideAllWidgets();
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void HandleMovementInput(const FInputActionValue& InputValue);
	void HandleCameraInput(const FInputActionValue& InputValue);

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AccelerationPerSecond = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MouseSensitivity = 0.5f;

	UPROPERTY(EditAnywhere, Category = "EI")
	TObjectPtr<UInputMappingContext> PlayerInputMappingContext = nullptr;

	UPROPERTY(EditAnywhere, Category = "EI")
	TObjectPtr<UInputAction> MovementInputAction;

	UPROPERTY(EditAnywhere, Category = "EI")
	TObjectPtr<UInputAction> CameraInputAction;
	
	UPROPERTY(EditAnywhere, Category = "EI")
	TObjectPtr<UInputAction> DropInputAction;

	void SetHeldItem(AActor* NewItem) { CurrentHeldItem = NewItem; }
	
	void OpenDocument(class UDocumentAsset* Data);
	
	void CloseDocument();

	void ToggleMenu();
	
	void ToggleWinSummaryWidget();
	
	void ToggleDefeatSummaryWidget();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool CanOpenDoor();
	
	void DropItem();
};
