#include "IB_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"


void UIB_GameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	if (OSS)
	{
		SessionInterface = OSS->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UIB_GameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UIB_GameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UIB_GameInstance::OnJoinSessionComplete);
		}
	}
	
}

void UIB_GameInstance::FindOrCreateDungeonSession(const FString& DungeonID, APlayerController* Requestor)
{
	if (!SessionInterface.IsValid() || !Requestor) return;

	CurrentDungeonID = DungeonID;
	CurrentRequestor = Requestor;
	bIsSessionEntryClosed = false;

	SessionSearch = MakeShareable(new FOnlineSessionSearch()) ;
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UIB_GameInstance::CreateDungeonSession(const FString& DungeonID, APlayerController* Requestor)
{
	if (!SessionInterface.IsValid() || !Requestor) return;

	FOnlineSessionSettings Settings;
	Settings.bIsLANMatch = false;
	Settings.bUsesPresence = true;
	Settings.NumPublicConnections = 4;
	Settings.bShouldAdvertise = true;
	Settings.bAllowJoinInProgress = true;

	// 세션에 던전ID 정보 저장
	Settings.Set(FName("DungeonID"), DungeonID, EOnlineDataAdvertisementType::ViaOnlineService);
	Settings.Set(FName("bIsEntryClosed"), false, EOnlineDataAdvertisementType::ViaOnlineService);

	FString NewSessionName = GenerateSessionName(DungeonID);

	SessionInterface->CreateSession(0, FName(*NewSessionName), Settings);
}

void UIB_GameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	if (!SessionSearch.IsValid() || !SessionInterface.IsValid())
		return;

	if (!bWasSuccessful || SessionSearch->SearchResults.Num() == 0)
	{
		// 세션 없으면 생성
		CreateDungeonSession(CurrentDungeonID, CurrentRequestor.Get());
		return;
	}

	for (const auto& Result : SessionSearch->SearchResults)
	{
		FString FoundDungeonID;
		Result.Session.SessionSettings.Get(FName("DungeonID"), FoundDungeonID);

		int32 NumPlayers = Result.Session.NumOpenPublicConnections;
		int32 MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;

		bool bIsEntryClosed = (Result.Session.SessionSettings.Get(FName("bIsEntryClosed"), bIsSessionEntryClosed) && bIsSessionEntryClosed);

		if (FoundDungeonID == CurrentDungeonID && NumPlayers > 0 && NumPlayers < MaxPlayers && !bIsEntryClosed)
		{
			// 빈 자리 있고 입장 가능 세션 발견 - 참가
			SessionInterface->JoinSession(0, NAME_GameSession, Result);
			return;
		}
	}

	// 빈 자리 세션 없으면 생성
	CreateDungeonSession(CurrentDungeonID, CurrentRequestor.Get());
}

void UIB_GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (!bWasSuccessful || !CurrentRequestor.IsValid())
		return;

	FString ConnectString;
	if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
	{
		// 세션 생성 성공 후 10초 뒤 입장 제한 시작
		StartSessionCloseTimer(SessionName);

		CurrentRequestor->ClientTravel(ConnectString, TRAVEL_Absolute);
	}
}

void UIB_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success || !CurrentRequestor.IsValid())
		return;

	FString ConnectString;
	if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
	{
		CurrentRequestor->ClientTravel(ConnectString, TRAVEL_Absolute);
	}
}


FString UIB_GameInstance::GenerateSessionName(const FString& DungeonID)
{
	return "";
}

void UIB_GameInstance::StartSessionCloseTimer(FName SessionName)
{
	ClosedSessionName = SessionName;
	bIsSessionEntryClosed = false;

	GetWorld()->GetTimerManager().SetTimer(SessionCloseTimerHandle, this, &UIB_GameInstance::CloseSessionEntry, 10.0f, false);
}

void UIB_GameInstance::CloseSessionEntry()
{
	if (!SessionInterface.IsValid()) return;

	bIsSessionEntryClosed = true;

	FOnlineSessionSettings Settings;
	if (SessionInterface->GetSessionSettings(ClosedSessionName))
	{
		Settings.Set(FName("bIsEntryClosed"), true, EOnlineDataAdvertisementType::ViaOnlineService);
		SessionInterface->UpdateSession(ClosedSessionName, Settings, true);
	}

	// 타이머 정리
	GetWorld()->GetTimerManager().ClearTimer(SessionCloseTimerHandle);
}
