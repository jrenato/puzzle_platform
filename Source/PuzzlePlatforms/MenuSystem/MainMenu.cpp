// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "Blueprint/UserWidget.h"
#include "ServerListItem.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"

UMainMenu::UMainMenu(const FObjectInitializer & ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UUserWidget> ServerListItemBPClass(TEXT("/Game/PuzzlePlatform/MenuSystem/WBP_ServerListItem"));
    if (!ensure(ServerListItemBPClass.Class != nullptr)) return;
    ServerListItemClass = ServerListItemBPClass.Class;
}

bool UMainMenu::Initialize()
{
    bool Success = Super::Initialize();
    if (!Success) return false;

    bIsFocusable = true;

    OpenJoinButton = Cast<UButton>(GetWidgetFromName(FName("OpenJoinButton")));
    OpenHostButton = Cast<UButton>(GetWidgetFromName(FName("OpenHostButton")));
    QuitButton = Cast<UButton>(GetWidgetFromName(FName("QuitButton")));
    
    ServerList = Cast<UScrollBox>(GetWidgetFromName(FName("ServerList")));
    ConfirmJoinButton = Cast<UButton>(GetWidgetFromName(FName("ConfirmJoinButton")));
    CancelJoinButton = Cast<UButton>(GetWidgetFromName(FName("CancelJoinButton")));

    HostNameTextBox = Cast<UEditableTextBox>(GetWidgetFromName(FName("HostNameTextBox")));
    ConfirmHostButton = Cast<UButton>(GetWidgetFromName(FName("ConfirmHostButton")));
    CancelHostButton = Cast<UButton>(GetWidgetFromName(FName("CancelHostButton")));

    OpenHostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);
    CancelHostButton->OnClicked.AddDynamic(this, &UMainMenu::CancelMenu);
    ConfirmHostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

    OpenJoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);
    CancelJoinButton->OnClicked.AddDynamic(this, &UMainMenu::CancelMenu);
    ConfirmJoinButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

    QuitButton->OnClicked.AddDynamic(this, &UMainMenu::Quit);

    return true;
}

void UMainMenu::CancelMenu()
{
    if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(MainMenu != nullptr)) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::OpenJoinMenu()
{
    if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;

    MenuSwitcher->SetActiveWidget(JoinMenu);

    if (MenuInterface != nullptr)
    {
        MenuInterface->RefreshServerList();
    }
}

void UMainMenu::JoinServer()
{
    if (MenuInterface != nullptr && SelectedIndex.IsSet())
	{
        MenuInterface->Join(SelectedIndex.GetValue());
	}
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No server selected"));
    }
}

void UMainMenu::OpenHostMenu()
{
    if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(HostMenu != nullptr)) return;

    MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::HostServer()
{
    if (MenuInterface != nullptr)
	{
        FString HostName = HostNameTextBox->GetText().ToString();
		MenuInterface->Host(HostName);
	}
}

void UMainMenu::SetServerList(TArray<FServerData> ServersAvailable)
{
    if (!ensure(ServerList != nullptr)) return;
    ServerList->ClearChildren();

    UWorld* World = this->GetWorld();
    if (!ensure(World != nullptr)) return;

    uint32 Index = 0;
    for (const FServerData& Server : ServersAvailable)
    {
        UServerListItem* ServerListItem = CreateWidget<UServerListItem>(World, ServerListItemClass);
        if (!ensure(ServerListItem != nullptr)) return;
        
        ServerListItem->ServerName->SetText(FText::FromString(Server.Name));
        ServerListItem->HostUserName->SetText(FText::FromString(Server.HostUserName));
        ServerListItem->ServerPlayers->SetText(FText::FromString(FString::FromInt(Server.CurrentPlayers) + "/" + FString::FromInt(Server.MaxPlayers)));
        ServerListItem->Setup(this, Index);

        ServerList->AddChild(ServerListItem);

        ++Index;
    }
}

void UMainMenu::SelectIndex(uint32 Index)
{
    SelectedIndex = Index;
    UpdateChildren();
}

void UMainMenu::Quit()
{
    APlayerController* PlayerController = GetPlayerController();
    if (!ensure(PlayerController != nullptr)) return;

    PlayerController->ConsoleCommand("quit");
}

void UMainMenu::UpdateChildren()
{
    for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
    {
        UServerListItem* ServerListItem = Cast<UServerListItem>(ServerList->GetChildAt(i));
        if (!ensure(ServerListItem != nullptr)) continue;

        ServerListItem->Selected = SelectedIndex.IsSet() && SelectedIndex.GetValue() == i;
    }
}
