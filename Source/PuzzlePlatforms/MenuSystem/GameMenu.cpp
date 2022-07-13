// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMenu.h"
#include "Components/Button.h"

bool UGameMenu::Initialize()
{
    bool Success = Super::Initialize();
    if (!Success) return false;

    CancelButton = Cast<UButton>(GetWidgetFromName(FName("CancelButton")));
    QuitButton = Cast<UButton>(GetWidgetFromName(FName("QuitButton")));

    if (!QuitButton || !CancelButton) return false;

    CancelButton->OnClicked.AddDynamic(this, &UGameMenu::CancelGameMenu);
    QuitButton->OnClicked.AddDynamic(this, &UGameMenu::QuitToMainMenu);

    return true;
}

void UGameMenu::CancelGameMenu()
{
    Teardown();
}

void UGameMenu::QuitToMainMenu()
{
    if (MenuInterface != nullptr)
    {
        Teardown();
        MenuInterface->LoadMainMenu();
    }
}
