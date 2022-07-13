// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUserName;
};

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()

public:
	UMainMenu(const FObjectInitializer & ObjectInitializer);

	void SetServerList(TArray<FServerData> ServerNames);

	void SelectIndex(uint32 Index);

protected:
	virtual bool Initialize();

private:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	// ** Menus ** //
	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;

	// ** Main Menu ** //
	UPROPERTY(meta = (BindWidget))
	class UButton* OpenJoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* OpenHostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	// ** Join Menu ** //
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ServerList;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelJoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmJoinButton;
	
	// ** Host Menu ** //
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* HostNameTextBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelHostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmHostButton;

	TSubclassOf<class UUserWidget> ServerListItemClass;

	UFUNCTION()
	void CancelMenu();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void OpenHostMenu();

	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void Quit();

	TOptional<uint32> SelectedIndex;

	void UpdateChildren();
};
