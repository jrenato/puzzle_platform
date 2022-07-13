// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"


void UMenuWidget::Setup()
{
    this->AddToViewport();

    // Get the player controller
    APlayerController* PlayerController = GetPlayerController();
    if (!ensure(PlayerController != nullptr)) return;

    // Set the player controller
    FInputModeUIOnly InputModeData;
    InputModeData.SetWidgetToFocus(this->TakeWidget());
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    PlayerController->SetInputMode(InputModeData);

    PlayerController->bShowMouseCursor = true;
    // this->bIsFocusable = true;
}

void UMenuWidget::Teardown()
{
    this->RemoveFromViewport();

    // Get the player controller
    APlayerController* PlayerController = GetPlayerController();
    if (!ensure(PlayerController != nullptr)) return;
    
    // Set the player controller
    FInputModeGameOnly InputModeData;

    PlayerController->SetInputMode(InputModeData);

    PlayerController->bShowMouseCursor = false;
    // this->bIsFocusable = false;
}

void UMenuWidget::SetMenuInterface(IMenuInterface* NewMenuInterface)
{
    this->MenuInterface = NewMenuInterface;
}

APlayerController* UMenuWidget::GetPlayerController()
{
    UWorld* World = GetWorld();
    if (!ensure(World != nullptr)) return nullptr;
    
    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (!ensure(PlayerController != nullptr)) return nullptr;
    
    return PlayerController;
}
