#include "IB_GameInstance.h"
#include "IB_GAS/IB_RPGPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "OnlineSessionSettings.h"
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

	// 데디케이티드 서버인 경우, 종료 시 모든 활성 세션 정리
	if (IsRunningDedicatedServer())
	{
		if (SessionInterface.IsValid())
		{
			// 로비 서버 종료 시 로비 세션 파괴
			SessionInterface->DestroySession(LobbySessionName);
			//활성던전 인스턴스 세션들도 모두 파괴
			for (auto& Pair : ActiveDungeonInstances)
			{
				FDungeonInstanceInfo& InstanceInfo = Pair.Value;
				if (SessionInterface.IsValid() && InstanceInfo.bIsAdvertised)
				{
					SessionInterface->DestroySession(InstanceInfo.SessionName); // 세션 파괴
				}
			}
			ActiveDungeonInstances.Empty();
		}
	}
}

	void UIB_GameInstance::CreateLobbySession()
	{
		UE_LOG(LogTemp, Error, TEXT("UIB_GameInstance: Try to createLobbySession"));
		
		TSharedPtr<FOnlineSessionSettings> LobbySessionSettings = MakeShareable(new FOnlineSessionSettings());
	if (LobbySessionSettings)
	{
		LobbySessionSettings->bIsLANMatch = false;
		LobbySessionSettings->NumPublicConnections = 100;
		LobbySessionSettings->bShouldAdvertise = true;
		LobbySessionSettings->bAllowJoinInProgress = true;
		LobbySessionSettings->bUseLobbiesIfAvailable = false;
		LobbySessionSettings->bUsesPresence = false;
		LobbySessionSettings->bIsDedicated=IsRunningDedicatedServer();
		
		LobbySessionSettings->Set(TEXT("SESSION_TYPE"), FString("LobbySession"), EOnlineDataAdvertisementType::ViaOnlineService);

		SessionInterface->CreateSession(0, LobbySessionName , *LobbySessionSettings);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UIB_GameInstance:LobbySessionSettings is not valid"));
	}
		
	}

void UIB_GameInstance::Client_FindLobbySession()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 10;
	SessionSearch->QuerySettings.Set(TEXT("SESSION_TYPE"), FString("LobbySession"), EOnlineComparisonOp::Equals);

	UE_LOG(LogTemp, Warning, TEXT("Client: Searching for LobbySession..."));

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UIB_GameInstance::RequestDungeonEntry(const FString& InDungeonID, APlayerController* RequestingPC)
{
	
}

void UIB_GameInstance::Server_FindOrCreateDungeonInstance(const FString& InDungeonID, APlayerController* RequestingPC)
{
	if (!SessionInterface.IsValid() || !RequestingPC) return;

    // if (IsRunningDedicatedServer())
    // {
    // }
        ClientRequest_DungeonID = InDungeonID; // 서버에서 처리 중인 던전 ID
        ClientRequest_Requestor = RequestingPC; // 서버에서 처리 중인 요청자

        FDungeonInstanceInfo* FoundInstance = nullptr;

        // 1. 현재 서버가 관리하는 활성 인스턴스 중 참여 가능한 던전 인스턴스 검색
        for (auto& Pair : ActiveDungeonInstances)
        {
            FDungeonInstanceInfo& InstanceInfo = Pair.Value;
            if (InstanceInfo.DungeonID == InDungeonID &&
                InstanceInfo.bIsAdvertised &&
                !InstanceInfo.bIsEntryClosed &&
                InstanceInfo.CurrentPlayers < InstanceInfo.MaxPlayers)
            {
                FoundInstance = &InstanceInfo;
                break;
            }
        }

        if (FoundInstance)
        {
            // 2. 기존 인스턴스에 참여
            UE_LOG(LogTemp, Warning, TEXT("Server: Found existing dungeon instance '%s' for '%s'. Joining."), *FoundInstance->SessionName.ToString(), *InDungeonID);
            
            FoundInstance->CurrentPlayers++; 

            // 클라이언트에게 연결 문자열 전송 (PlayerController의 Client RPC 호출)
            AIB_RPGPlayerController* MyPC = Cast<AIB_RPGPlayerController>(RequestingPC);
            if (MyPC)
            {
                MyPC->Client_TravelToDungeonInstance(FoundInstance->ConnectString);
            }
            else
            {
                 UE_LOG(LogTemp, Error, TEXT("Server: Failed to cast RequestingPC to AMyPlayerController for Client_TravelToDungeonInstance."));
            }

            // 만약 4명이 모두 찼다면 세션 광고 중지 및 입장 제한
            if (FoundInstance->CurrentPlayers >= FoundInstance->MaxPlayers)
            {
                UE_LOG(LogTemp, Warning, TEXT("Server: Dungeon instance '%s' reached max players. Closing entry."), *FoundInstance->SessionName.ToString());
                Server_CloseSessionEntry(FoundInstance->SessionName);
            }
        }
        else
        {
            // 3. 새로운 던전 인스턴스 생성
            UE_LOG(LogTemp, Warning, TEXT("Server: No available instance for '%s'. Creating new one."), *InDungeonID);
            Server_CreateDungeonSession(InDungeonID, RequestingPC);
        }
}

void UIB_GameInstance::Server_CreateDungeonSession(const FString& InDungeonID, APlayerController* RequestingPC)
{
	 if (!SessionInterface.IsValid()) return;

    // if (IsRunningDedicatedServer())
    // {
    // }
	const TSharedPtr<FOnlineSessionSettings> DungeonSessionSettings = MakeShareable(new FOnlineSessionSettings());
        DungeonSessionSettings->bIsLANMatch = false;
        DungeonSessionSettings->bUsesPresence = true;
        DungeonSessionSettings->NumPublicConnections = 4;
        DungeonSessionSettings->bShouldAdvertise = true; 
        DungeonSessionSettings->bAllowJoinInProgress = true; 
        DungeonSessionSettings->bUsesStats = false; 

        DungeonSessionSettings->Set(FName("DungeonID"), InDungeonID, EOnlineDataAdvertisementType::ViaOnlineService);
	DungeonSessionSettings->Set(FName("bIsEntryClosed"), false, EOnlineDataAdvertisementType::ViaOnlineService);

        FName NewSessionName = FName(*GenerateSessionName(InDungeonID));

        FDungeonInstanceInfo NewInstance;
        NewInstance.SessionName = NewSessionName;
        NewInstance.DungeonID = InDungeonID;
        NewInstance.MaxPlayers = DungeonSessionSettings->NumPublicConnections;
        NewInstance.CurrentPlayers = 1; // 세션 생성 요청한 첫 플레이어 포함
        NewInstance.bIsAdvertised = true; 
        NewInstance.bIsEntryClosed = false;
	

        // ** 중요: 이 부분에서 해당 던전 인스턴스가 호스팅될 서버 IP와 포트를 정확히 설정해야 합니다. **
        // 데디케이티드 서버가 여러 맵을 동시에 호스팅하는 아키텍처라면,
        // 각 던전 인스턴스에 고유한 포트가 할당될 수 있습니다.
        // FString ServerIP = FString("YOUR_DEDICATED_SERVER_IP"); // 실제 서버 IP로 변경
        // int32 ServerPort = YOUR_DEDICATED_SERVER_BASE_PORT + INDEX_FOR_THIS_DUNGEON_INSTANCE; // 포트 관리 로직 필요
        
        // 현재 GameMode의 NetDriver 정보를 활용하여 IP/Port 가져오기 (가장 일반적인 방법)
        FString CurrentListenAddr;
        if (AGameModeBase* GM = GetWorld()->GetAuthGameMode())
        {
            if (UNetDriver* NetDriver = GM->GetNetDriver())
            {
                CurrentListenAddr = NetDriver->LowLevelGetNetworkNumber(); // IP:Port 형식
            }
        }
        if (CurrentListenAddr.IsEmpty())
        {
            UE_LOG(LogTemp, Error, TEXT("Server: Could not determine current listen address for new dungeon instance."));
            // Fallback or error handling
            CurrentListenAddr = TEXT("127.0.0.1:7777"); // 임시 기본값
        }

	// 맵 경로 문자열 구성
	NewInstance.MapName = FString::Printf(TEXT("/Game/Maps/%s"), *InDungeonID);

	// ConnectString 포맷: "IP:Port::MapPath"
	NewInstance.ConnectString = FString::Printf(TEXT("%s::%s"), *CurrentListenAddr, *NewInstance.MapName);
	
	
	ActiveDungeonInstances.Add(NewInstance.SessionName, NewInstance);

	UE_LOG(LogTemp, Warning, TEXT("Server: Attempting to create session '%s' for DungeonID '%s'."), *NewSessionName.ToString(), *InDungeonID);
	SessionInterface->CreateSession(0, NewSessionName, *DungeonSessionSettings);
}

void UIB_GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// if (IsRunningDedicatedServer())
	// {
	// }
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("Server: Failed to create session '%s'."), *SessionName.ToString());
		ActiveDungeonInstances.Remove(SessionName);
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Server: Session '%s' created successfully."), *SessionName.ToString());

	if (SessionName == FName("LobbySession"))
	{
		// ✅ 로비 세션 생성 완료 - 아무 작업도 안 함
		UE_LOG(LogTemp, Warning, TEXT("Server: Lobby session created."));
		return;
	}
	
	FDungeonInstanceInfo* InstanceInfo = ActiveDungeonInstances.Find(SessionName);
	if (InstanceInfo)
	{
		InstanceInfo->bIsAdvertised = true;
                
		// 10초 타이머 시작
		Server_StartSessionCloseTimer(SessionName);

		// ** 중요 변경: 서버는 ServerTravel 하지 않습니다. **
		// 서버는 해당 던전 맵을 이미 호스팅 중이거나,
		// 플레이어가 ClientTravel로 접속할 때 자동으로 맵이 로드되도록 설정되어 있을 수 있습니다.
		// 만약 서버가 현재 메인 로비 맵에 있다면, 던전 맵을 **동적으로 로드**하는 로직이 필요할 수 있습니다.
		// UGameplayStatics::LoadStreamLevel(this, FName(*InstanceInfo->MapName), true, false, LatentActionInfo); (Sublevel)
		// 또는 서버 시작 시 모든 던전 맵이 이미 로드되어 있는 복잡한 월드 구조일 수 있습니다.
		// 여기서는 서버가 해당 맵을 클라이언트 접속을 통해 호스팅할 수 있다고 가정합니다.

		// 클라이언트에게 연결 문자열 전송 (요청했던 클라이언트에게만)
		// ClientRequest_Requestor는 UIB_GameInstance::Server_FindOrCreateDungeonInstance에서 설정됨
		if (ClientRequest_Requestor.IsValid())
		{
			// join session으로 바꿔야될듯, 이미 로비세션 안에 있는거기 때문에
			AIB_RPGPlayerController* MyPC = Cast<AIB_RPGPlayerController>(ClientRequest_Requestor.Get());
			if (MyPC)
			{
				MyPC->Client_TravelToDungeonInstance(InstanceInfo->ConnectString);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Server: Failed to cast ClientRequest_Requester to AMyPlayerController for Client_TravelToDungeonInstance after session create."));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Server: No dungeon info found for session '%s'."), *SessionName.ToString());
	}
}

void UIB_GameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful || !SessionSearch.IsValid()) return;

	UE_LOG(LogTemp, Warning, TEXT("Client: FindSession completed. Total: %d"), SessionSearch->SearchResults.Num());

	for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
	{
		UE_LOG(LogTemp, Warning, TEXT("== Session Info =="));
		for (auto& Setting : Result.Session.SessionSettings.Settings)
		{
			FString Key = Setting.Key.ToString();
			FString Value = Setting.Value.Data.ToString();
			UE_LOG(LogTemp, Warning, TEXT("Setting: %s = %s"), *Key, *Value);
		}
		FString SessionKeyword;
		if (Result.Session.SessionSettings.Get(TEXT("SESSION_TYPE"), SessionKeyword))
		{
			UE_LOG(LogTemp, Warning, TEXT("Client: Found SessionKeyword == %s"), *SessionKeyword);
			
			if (SessionKeyword == "LobbySession")
			{
				UE_LOG(LogTemp, Warning, TEXT("Client: Found LobbySession, attempting to join..."));
				SessionInterface->JoinSession(0, FName("LobbySession"), Result);
				return;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Client: LobbySession not found."));
}

void UIB_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	// 이 콜백은 클라이언트에서만 처리 (서버는 직접 세션 조인 안 함)
	// if (IsRunningDedicatedServer())
	// {
	// }
		if (Result != EOnJoinSessionCompleteResult::Success || !ClientRequest_Requestor.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("Client: Failed to join session '%s'. Result: %d"), *SessionName.ToString(), (int32)Result);
			return;
		}

		FString ConnectString;
	
		if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
		{
			APlayerController* PC = GetFirstLocalPlayerController();
			if (PC)
			{
				UE_LOG(LogTemp, Warning, TEXT("Client: Joining session at %s"), *ConnectString);
				PC->ClientTravel(ConnectString, TRAVEL_Absolute);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete::NO PlayerController"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Client: Failed to get connect string for session '%s'."), *SessionName.ToString());
		}
}

void UIB_GameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	// 세션 파괴 완료 시 호출 (주로 서버에서 세션 정리 시 사용)
	// if (IsRunningDedicatedServer())
	// {
	// 	
	// }
		if (bWasSuccessful)
		{
			UE_LOG(LogTemp, Warning, TEXT("Server: Session '%s' destroyed successfully."), *SessionName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Server: Failed to destroy session '%s'."), *SessionName.ToString());
		}
		ActiveDungeonInstances.Remove(SessionName); // 맵에서 제거
	
}

void UIB_GameInstance::Server_StartSessionCloseTimer(FName SessionName)
{
	// if (IsRunningDedicatedServer())
	// {
	// }
		FDungeonInstanceInfo* InstanceInfo = ActiveDungeonInstances.Find(SessionName);
		if (InstanceInfo)
		{
			FTimerDelegate TimerDel;
			TimerDel.BindUObject(this, &UIB_GameInstance::Server_CloseSessionEntry, SessionName);
			GetWorld()->GetTimerManager().SetTimer(
				InstanceInfo->SessionTimeoutTimerHandle,
				TimerDel,
				10.0f, // 10초
				false // 반복 안함
			);
			UE_LOG(LogTemp, Warning, TEXT("Server: Started 10-second timer for session '%s'."), *SessionName.ToString());
		}
}

void UIB_GameInstance::Server_CloseSessionEntry(FName SessionName)
{
	if (!SessionInterface.IsValid()) return;

	// if (IsRunningDedicatedServer())
	// {
	// }
		FDungeonInstanceInfo* InstanceInfo = ActiveDungeonInstances.Find(SessionName);
		if (InstanceInfo)
		{
			InstanceInfo->bIsEntryClosed = true; // 입장 제한 설정
			InstanceInfo->bIsAdvertised = false; // 광고 중지

			FOnlineSessionSettings Settings;
			// 기존 세션 설정을 가져와서 업데이트해야 합니다.
			// GetSessionSettings는 FOnlineSessionSettings를 반환하지 않으므로,
			// 세션 인터페이스에서 직접 세션 속성을 업데이트하는 함수를 사용해야 합니다.
			// 또는 UpdateSession 함수에 FOnlineSessionSettings를 직접 전달해야 합니다.
			// 여기서는 GetSessionSettings가 반환하는 FOnlineSessionSettings를 사용한다고 가정합니다.
			// 실제로는 UpdateSession 함수가 FOnlineSessionSettings를 인자로 받으므로 새로 생성하여 전달합니다.
			Settings.NumPublicConnections = InstanceInfo->MaxPlayers;
			Settings.bIsLANMatch = false;
			Settings.bUsesPresence = true;
			Settings.bShouldAdvertise = false; // 광고 중지
			Settings.bAllowJoinInProgress = true; // 이미 들어온 플레이어는 유지
			Settings.Set(FName("DungeonID"), InstanceInfo->DungeonID, EOnlineDataAdvertisementType::ViaOnlineService);
			Settings.Set(FName("bIsEntryClosed"), true, EOnlineDataAdvertisementType::ViaOnlineService); // 입장 제한

			// UpdateSession 함수는 FOnlineSessionSettings를 인자로 받습니다.
			SessionInterface->UpdateSession(SessionName, Settings, true); // bOverrideExistingProperties = true

			UE_LOG(LogTemp, Warning, TEXT("Server: Session '%s' entry closed and advertisement ended."), *SessionName.ToString());

			// 타이머 정리 (만약 타이머 만료로 호출된 것이 아니라면)
			GetWorld()->GetTimerManager().ClearTimer(InstanceInfo->SessionTimeoutTimerHandle);
		}
}

FString UIB_GameInstance::GenerateSessionName(const FString& InDungeonID)
{
	// 던전 ID와 고유 GUID를 결합하여 세션 이름 생성
	return FString::Printf(TEXT("%s_%s"), *InDungeonID, *FGuid::NewGuid().ToString());
}

void UIB_GameInstance::Server_OnPlayerConnectedToDungeon(APlayerController* NewPlayer, const FName SessionName)
{
	// if (IsRunningDedicatedServer())
	// {
	// }
		FDungeonInstanceInfo* InstanceInfo = ActiveDungeonInstances.Find(SessionName);
		if (InstanceInfo)
		{
			// CurrentPlayers는 이미 RequestDungeonInstance에서 증가시켰으므로 여기서는 추가 증가 없음.
			// 하지만 실제 접속 완료 시점의 정확한 인원수를 반영하려면 여기서 증가시키는 것이 더 정확할 수 있습니다.
			// 여기서는 이미 RequestDungeonInstance에서 1 증가시켰다고 가정합니다.
			// InstanceInfo->CurrentPlayers++; // 필요시 여기서 증가

			UE_LOG(LogTemp, Warning, TEXT("Server: Player %s connected to dungeon instance '%s'. Current players: %d"),
				*NewPlayer->GetName(), *SessionName.ToString(), InstanceInfo->CurrentPlayers);

			// 4명 모두 찼다면 세션 입장 제한
			if (InstanceInfo->CurrentPlayers >= InstanceInfo->MaxPlayers && !InstanceInfo->bIsEntryClosed)
			{
				UE_LOG(LogTemp, Warning, TEXT("Server: Dungeon instance '%s' reached max players. Closing entry."), *SessionName.ToString());
				Server_CloseSessionEntry(SessionName);
			}
		}
	
}

void UIB_GameInstance::Server_OnPlayerDisconnectedFromDungeon(APlayerController* Player, const FName SessionName)
{
	// if (IsRunningDedicatedServer())
	// {
	// }
		FDungeonInstanceInfo* InstanceInfo = ActiveDungeonInstances.Find(SessionName);
		if (InstanceInfo)
		{
			InstanceInfo->CurrentPlayers--;
			UE_LOG(LogTemp, Warning, TEXT("Server: Player %s disconnected from dungeon instance '%s'. Current players: %d"),
				*Player->GetName(), *SessionName.ToString(), InstanceInfo->CurrentPlayers);

			if (InstanceInfo->CurrentPlayers <= 0)
			{
				// 모든 플레이어가 나가면 세션 파괴
				UE_LOG(LogTemp, Warning, TEXT("Server: Dungeon instance '%s' is empty. Destroying session."), *SessionName.ToString());
				GetWorld()->GetTimerManager().ClearTimer(InstanceInfo->SessionTimeoutTimerHandle);
				if (SessionInterface.IsValid())
				{
					SessionInterface->DestroySession(SessionName); // 세션 파괴
				}
				// OnDestroySessionComplete 콜백에서 ActiveDungeonInstances 맵에서 제거됩니다.
			}
			else if (InstanceInfo->CurrentPlayers < InstanceInfo->MaxPlayers && InstanceInfo->bIsEntryClosed)
			{
				// 플레이어가 나가서 다시 자리가 생겼고, 타이머가 만료되지 않았다면 다시 입장 허용 (선택 사항)
				// 현재 시나리오에서는 10초 또는 4명 이후 닫히면 다시 열지 않습니다.
				// 만약 다시 열고 싶다면 여기서 Server_OpenSessionEntry 같은 함수를 호출해야 합니다.
			}
		}
	
}
