// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"

// #include "Engine/Engine.h"
// #include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
// #include "OnlineSubsystemTypes.h" 

#include "PlatformTrigger.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/GameMenu.h"
#include "MenuSystem/MenuWidget.h"

const static FName SESSION_NAME = NAME_GameSession;
const static FName SESSION_NAME_SETTINGS_KEY = TEXT("HostName");

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer & ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/PuzzlePlatform/MenuSystem/WBP_MainMenu"));
    if (!ensure(MainMenuBPClass.Class != nullptr)) return;
    MainMenuClass = MainMenuBPClass.Class;

    ConstructorHelpers::FClassFinder<UUserWidget> GameMenuBPClass(TEXT("/Game/PuzzlePlatform/MenuSystem/WBP_GameMenu"));
    if (!ensure(GameMenuBPClass.Class != nullptr)) return;
    GameMenuClass = GameMenuBPClass.Class;
}

void UPuzzlePlatformsGameInstance::Init()
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();

    if (Subsystem != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Found Subsystem %s"), *Subsystem->GetSubsystemName().ToString());
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnCreateSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnDestroySessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnJoinSessionComplete);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Subsystem is null"));
    }

    if (GEngine != nullptr)
    {
        GEngine->OnNetworkFailure().AddUObject(this, &UPuzzlePlatformsGameInstance::OnNetworkFailure);
    }

}

void UPuzzlePlatformsGameInstance::LoadMenu()
{
    if (!ensure(MainMenuClass != nullptr)) return;
    MainMenu = CreateWidget<UMainMenu>(this, MainMenuClass);

    MainMenu->Setup();

    MainMenu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::LoadInGameMenu()
{
    if (!ensure(GameMenuClass != nullptr)) return;
    GameMenu = CreateWidget<UGameMenu>(this, GameMenuClass);

    GameMenu->Setup();

    GameMenu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::CreateSession()
{
    if (SessionInterface.IsValid())
    {
        FOnlineSessionSettings SessionSettings;

        SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";

        SessionSettings.NumPublicConnections = 5;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.bUseLobbiesIfAvailable = true;
        SessionSettings.Set(SESSION_NAME_SETTINGS_KEY, HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

        SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
    }
}

void UPuzzlePlatformsGameInstance::Host(FString HostNameValue)
{
    this->HostName = HostNameValue;

    if (SessionInterface.IsValid())
    {
        FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
        if (ExistingSession != nullptr)
        {
            SessionInterface->DestroySession(SESSION_NAME);
        }
        else
        {
            CreateSession();
        }
    }
}

void UPuzzlePlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
    if (!Success)
    {
        return;
    }

    if (MainMenu != nullptr)
    {
        MainMenu->Teardown();
    }

    if (GameMenu != nullptr)
    {
        GameMenu->Teardown();
    }

    UEngine* Engine = GetEngine();
    if (!ensure(Engine != nullptr)) return;

    UWorld* World = GetWorld();
    if (!ensure(World != nullptr)) return;

    Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, TEXT("Hosting"));
    World->ServerTravel("/Game/PuzzlePlatform/Maps/Lobby?listen");
}

void UPuzzlePlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
    if (Success)
    {
        CreateSession();
    }
}

void UPuzzlePlatformsGameInstance::OnFindSessionsComplete(bool Success)
{
    UE_LOG(LogTemp, Warning, TEXT("Session Search Complete"));

    TArray<FServerData> ServersAvailable;

    if (Success && SessionSearch.IsValid() && MainMenu != nullptr)
    {
        TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
        UE_LOG(LogTemp, Warning, TEXT("Found %d sessions"), SearchResults.Num());

        for (const FOnlineSessionSearchResult& SearchResult : SearchResults)
        {
            UE_LOG(LogTemp, Warning, TEXT("Found Session: %s"), *SearchResult.GetSessionIdStr());

            FServerData ServerData;

            // ServerData.Name = SearchResult.GetSessionIdStr();
            ServerData.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
            ServerData.CurrentPlayers = ServerData.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
            ServerData.HostUserName = SearchResult.Session.OwningUserName;

            FString ServerHostName;
            if (SearchResult.Session.SessionSettings.Get(SESSION_NAME_SETTINGS_KEY, ServerHostName))
            {
                ServerData.Name = ServerHostName;
            }
            else
            {
                ServerData.Name = "Unknown server name";
            }

            FString Data;
            if (SearchResult.Session.SessionSettings.Get(TEXT("HostName"), Data))
            {
                UE_LOG(LogTemp, Warning, TEXT("Found Data: %s"), *Data);
            }

            ServersAvailable.Add(ServerData);
        }
    }

    if (MainMenu != nullptr)
    {
        MainMenu->SetServerList(ServersAvailable);
    }
}

void UPuzzlePlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (!SessionInterface.IsValid() || Result != EOnJoinSessionCompleteResult::Success)
    {
        return;
    }

    UEngine* Engine = GetEngine();
    if (!ensure(Engine != nullptr)) return;

    FString Address;
    if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
    {
        Engine->AddOnScreenDebugMessage(0, 2, FColor::Red, TEXT("Unable to connect to server"));
        return;
    }
    
    if (Address.IsEmpty())
    {
        Engine->AddOnScreenDebugMessage(0, 2, FColor::Red, TEXT("The syntax is 'Join <IP>'"));
        return;
    }

    Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if (!ensure(PlayerController != nullptr)) return;

    PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UPuzzlePlatformsGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
    LoadMainMenu();
}

void UPuzzlePlatformsGameInstance::Join(uint32 Index)
{
    if (!SessionInterface.IsValid() || !SessionSearch.IsValid()) return;

    if (MainMenu != nullptr)
    {
        MainMenu->Teardown();
    }

    SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UPuzzlePlatformsGameInstance::RefreshServerList()
{
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    if (SessionSearch.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Starting Session Search"));

        // SessionSearch->bIsLanQuery = true;
        SessionSearch->MaxSearchResults = 100;
        SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }
}

void UPuzzlePlatformsGameInstance::LoadMainMenu()
{
    if (GameMenu != nullptr)
    {
        GameMenu->Teardown();
    }

    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if (!ensure(PlayerController != nullptr)) return;

    PlayerController->ClientTravel("/Game/PuzzlePlatform/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);

}

void UPuzzlePlatformsGameInstance::StartSession()
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->StartSession(SESSION_NAME);
    }
}
