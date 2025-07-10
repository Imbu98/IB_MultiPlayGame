#include "IB_GameInstance.h"
#include "IB_GAS/IB_RPGPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Online/OnlineSessionNames.h"
// 네트워크 관련 핵심 헤더
#include "Net/UnrealNetwork.h" // UNetDriver, FURL 등을 위해 필요
#include "Sockets.h" // FInternetAddr, FIPv4Address 등을 위해 필요 (빌드 파일에 Sockets 모듈 포함 필수)
#include "Interfaces/IPv4/IPv4Address.h" // FIPv4Address를 위해 필요


void UIB_GameInstance::Init()
{
	Super::Init();
	
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	if (OSS)
	{
		SessionInterface = OSS->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			// 모든 콜백은 클라이언트/서버 모두에서 바인딩됩니다.
			// 실제 로직은 내부에서 IsRunningDedicatedServer()로 분기합니다.
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UIB_GameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UIB_GameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UIB_GameInstance::OnJoinSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UIB_GameInstance::OnDestroySessionComplete); // 추가

			if (GEngine)
			{
	
				GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,FString::Printf(TEXT("%s"),*OSS->GetSubsystemName().ToString()));
			}
			if (IsRunningDedicatedServer())
			{
				CreateLobbySession();
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UIB_GameInstance: Failed to get Online Session Interface."));
		}
	}
}

void UIB_GameInstance::Shutdown()
{
	Super::Shutdown();

	
	//if (IsRunningDedicatedServer())
	//{
	//	for (auto& Pair : ActiveDungeonInstances)
	//	{
	//		FDungeonInstanceInfo& InstanceInfo = Pair.Value;
	//		GetWorld()->GetTimerManager().ClearTimer(InstanceInfo.SessionTimeoutTimerHandle);
	//		if (SessionInterface.IsValid() && InstanceInfo.bIsAdvertised)
	//		{
	//			SessionInterface->DestroySession(InstanceInfo.SessionName); // 세션 파괴
	//		}
	//	}
	//	ActiveDungeonInstances.Empty();
	//}
}

void UIB_GameInstance::CreateLobbySession_Implementation()

{
	UE_LOG(LogTemp, Error, TEXT("TryToCreateLobbySession"));
	
	// FString SessionName = FString::Printf(TEXT("LobbySession_%d"), LobbySessionCounter);
	//
	//  SessionSettings =MakeShareable(new FOnlineSessionSettings());
	// if (SessionSettings)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("TryToCreateLobbySession"));
	// 	
	// 	SessionSettings->bIsLANMatch = false;
	// 	SessionSettings->NumPublicConnections = 100;
	// 	SessionSettings->bShouldAdvertise = true;
	// 	SessionSettings->bAllowJoinInProgress = true;
	// 	SessionSettings->bUseLobbiesIfAvailable = false;
	// 	SessionSettings->bUsesPresence = false;
	// 	SessionSettings->bIsDedicated=IsRunningDedicatedServer();
	//
	// 	SessionSettings->Set(FName("SESSION_TYPE"),FString(TEXT("LobbySession")), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	// 	SessionSettings->Set(FName("MAP_NAME"),FString(TEXT("L_Lobby")), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	//
	// 	SessionInterface->CreateSession(0, FName(*SessionName),*SessionSettings);
	// }
}


void UIB_GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session %s created successfully"), *SessionName.ToString());

		// 현재 세션 관리 인터페이스 가져오기
		if (SessionInterface.IsValid())
		{
			// 생성된 세션 객체 얻기
			FNamedOnlineSession* CreatedSession = SessionInterface->GetNamedSession(SessionName);
			if (CreatedSession)
			{

				// MAP_NAME 로그 출력
				FString MapName;
				if (CreatedSession->SessionSettings.Get(TEXT("MAPNAME"), MapName))
				{
					UE_LOG(LogTemp, Log, TEXT("MAPNAME = %s"), *MapName);
				}

				// 예: 모든 키를 순회하며 출력 (디버그용)
				for (auto& SettingPair : CreatedSession->SessionSettings.Settings)
				{
					UE_LOG(LogTemp, Log, TEXT("Key: %s | Value: %s"),
						*SettingPair.Key.ToString(),
						*SettingPair.Value.Data.ToString());
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create session %s"), *SessionName.ToString());
	}
}

void UIB_GameInstance::FindLobbySession()
{
	if (!SessionInterface.IsValid()) return;
	
	SessionSearch.MaxSearchResults = 200000;
	SessionSearch.QuerySettings.Set(FName("MAPNAME"), FString(TEXT("L_Lobby")), EOnlineComparisonOp::Equals);
	
	FString MapNameValue;
	if (SessionSearch.QuerySettings.Get(FName("MAPNAME"), MapNameValue))
	{
		UE_LOG(LogTemp, Warning, TEXT("MAPNAME 검색 조건: %s"), *MapNameValue);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MAPNAME 검색 조건이 설정되지 않았습니다."));
	}

	SessionInterface->FindSessions(0, MakeShareable(new FOnlineSessionSearch(SessionSearch)));
}

UIB_GameInstance::RequestCreateDungeonSession_Implementation(const FString& DungeonName)
{


	UE_LOG(LogTemp, Error, TEXT("TryToCreateDungeonSession"));
}

void UIB_GameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		SessionSearchResults=SessionSearch.SearchResults;
	
		for (const FOnlineSessionSearchResult& Result : SessionSearch.SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("== Session Info =="));
			for (auto& Setting : Result.Session.SessionSettings.Settings)
			{
				FString Key = Setting.Key.ToString();
				FString Value = Setting.Value.Data.ToString();
				UE_LOG(LogTemp, Warning, TEXT("Setting: %s = %s"), *Key, *Value);
			}
			
			FString MapName;
			if (Result.Session.SessionSettings.Get(FName("MAPNAME"), MapName))
			{
				if (MapName == "L_Lobby")
				{
					SessionInterface->JoinSession(0, NAME_GameSession, Result);
					return;
				}
			}
		}
	}
}

void UIB_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;
	
	FString ConnectString;
	
	if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			PC->ClientTravel(ConnectString, TRAVEL_Absolute);
		}
	}
}

void UIB_GameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	
}

