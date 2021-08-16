// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "CustomGameInstance.generated.h"


USTRUCT(BlueprintType)
struct FMapInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString MapURL;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString MapName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString MapDescription;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UTexture2D* MapImage;
};

USTRUCT(BlueprintType)
struct FLobbyInfo {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
		struct FMapInfo MapInfo;

	UPROPERTY(BlueprintReadOnly)
		TArray<FString> Players;
};

UCLASS()
class TACTICALSHOOTER_API UCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()

		UCustomGameInstance();
};
