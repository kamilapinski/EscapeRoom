// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DocumentAsset.generated.h"

USTRUCT(BlueprintType)
struct FDocumentData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
    FText Content;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* BackgroundImage;
};

UCLASS()
class ESCAPEROOM_API UDocumentAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDocumentData DocumentData;
};
