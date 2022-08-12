// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "TimerManager.h"
#include "PuzzlePlatformsGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    NumberOfPlayers++;

    if (NumberOfPlayers == 2)
    {
        GetWorldTimerManager().SetTimer(TimerHandle, this, &ALobbyGameMode::StartGame, 5.0f);
    }
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    NumberOfPlayers--;

    Super::Logout(Exiting);
}

void ALobbyGameMode::StartGame()
{
    UPuzzlePlatformsGameInstance* GameInstance = Cast<UPuzzlePlatformsGameInstance>(GetGameInstance());
    if (GameInstance == nullptr) return;
    GameInstance->StartSession();

    UWorld* World = GetWorld();
    if (World != nullptr)
    {
        bUseSeamlessTravel = true;
        World->ServerTravel("/Game/PuzzlePlatform/Maps/Stage1?listen");
    }
}
