// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerListItem.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UServerListItem : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HostUserName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerPlayers;

	UPROPERTY(BlueprintReadOnly)
	bool Selected = false;

	void Setup(class UMainMenu* ParentObject, uint32 IndexValue);

private:
	uint32 Index;
	
	UPROPERTY()
	class UMainMenu* Parent;

	UPROPERTY(meta = (BindWidget))
	class UButton* ServerButton;

	UFUNCTION()
	void OnClicked();
};
