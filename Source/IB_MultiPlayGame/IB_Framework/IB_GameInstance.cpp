#include "IB_GameInstance.h"
#include "IB_GAS/IB_RPGPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "Online/OnlineSessionNames.h"
// 네트워크 관련 핵심 헤더
#include "SocketSubsystem.h"
#include "Networking.h"
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

				int32 LocalPort = GetWorld()->URL.Port;

				if (LocalPort == 7777)
				{
					StartListeningForDungeonShutdown();
				}
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
					if (MapName == CurrentRequestDungeonMapName)
					{
						SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
						FTimerHandle LockSessionTimerHandle;
						GetWorld()->GetTimerManager().SetTimer(
							LockSessionTimerHandle,
							FTimerDelegate::CreateLambda([this,SessionName]()
								{
									if (SessionInterface.IsValid())
									{
										FOnlineSessionSettings* CurrentSettings = SessionInterface->GetSessionSettings(SessionName);
										if (CurrentSettings)
										{
											CurrentSettings->bAllowJoinInProgress = false;
											UE_LOG(LogTemp, Warning, TEXT("Locking session after 10 seconds"));

											SessionInterface->UpdateSession(SessionName, *CurrentSettings, true);
										}
									}
								}),
							10.0f,
							false
						);


					}
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
	
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch)
	{
		SessionSearch->MaxSearchResults = 200000;
		SessionSearch->QuerySettings.Set(FName("MAPNAME"), FString(TEXT("L_Lobby")), EOnlineComparisonOp::Equals);

		FString MapNameValue;
		if (SessionSearch->QuerySettings.Get(FName("MAPNAME"), MapNameValue))
		{
			UE_LOG(LogTemp, Warning, TEXT("MAPNAME 검색 조건: %s"), *MapNameValue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MAPNAME 검색 조건이 설정되지 않았습니다."));
		}

		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UIB_GameInstance::RequestFindOrCreateDungeonSession(const FString& DungeonName, AIB_RPGPlayerController* Player)
{
	if (!SessionInterface.IsValid()) return;
	if (!Player) return;

	CurrentRequestPlayer = Player;
	CurrentRequestDungeonMapName = DungeonName;

	/*AIB_RPGPlayerController* IB_RPGPlayerController = Cast<AIB_RPGPlayerController>(Player);
	if (IB_RPGPlayerController)
	{
		IB_RPGPlayerController->ClientLeaveLobbySession(DungeonName);
	}*/
	
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch)
	{
		SessionSearch->MaxSearchResults = 200000;
		SessionSearch->QuerySettings.Set(FName("MAPNAME"), DungeonName, EOnlineComparisonOp::Equals);

		FString MapNameValue;
		if (SessionSearch->QuerySettings.Get(FName("MAPNAME"), MapNameValue))
		{
			UE_LOG(LogTemp, Warning, TEXT("MAPNAME 검색 조건: %s"), *MapNameValue);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MAPNAME 검색 조건이 설정되지 않았습니다."));
		}

		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UIB_GameInstance::CreateDungeonSession_Implementation(const FString& DungeonName, const int32& GeneratedPort)
{

	UE_LOG(LogTemp, Error, TEXT("TryToCreateDungeonSession"));
}

void UIB_GameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Error, TEXT("Tryo On Find Session Complete"));

	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("Session Found"));

		if (!SessionSearch)
		{
			UE_LOG(LogTemp, Error, TEXT("No SharedPtr:Session Search "));
			return;
		}

		SessionSearchResults=SessionSearch->SearchResults;
	
		for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
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
					UE_LOG(LogTemp, Warning, TEXT("TryToJoinLobbySession"));
					SessionInterface->JoinSession(0, NAME_GameSession, Result);
					return;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("NoMapNameL_Lobby"));
				}
				if (MapName==CurrentRequestDungeonMapName)
				{
					UE_LOG(LogTemp, Warning, TEXT("TryToJoinDungeonSession"));
					SessionInterface->JoinSession(0, NAME_GameSession, Result);
					return;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("NoDungameMapName : %s"),*MapName);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("NoMapName"));
			}
		}
	}
	else if(!bWasSuccessful)
	{
		if (CurrentRequestDungeonMapName == TEXT("L_Lobby"))
		{
	
			UE_LOG(LogTemp, Warning, TEXT("No lobby session found. Handling lobby session failure."));
			
		}
		else
		{
			// 던전 세션을 찾지 못한 경우 새로 생성
			UE_LOG(LogTemp, Warning, TEXT("No dungeon session found for: %s. Creating new session."), *CurrentRequestDungeonMapName);

			if (IsRunningDedicatedServer())
			{
				int32 NewPort = GetAvailablePort();

				FDungeonInstanceData NewInstance;
				NewInstance.InstanceID = NextInstanceID++;
				NewInstance.Port = NewPort;
				NewInstance.CurrentPlayers = 1;
				NewInstance.bLocked = false;


				FString Params = FString::Printf(TEXT("%s -server  -log -port=%d "), *CurrentRequestDungeonMapName, NewPort);
				FString ServerPath = TEXT("D:\\IB_MultiplayGame\\Package\\WindowsServer\\IB_MultiPlayGameServer.exe");
				NewInstance.DungeonServerHandle = FPlatformProcess::CreateProc(*ServerPath, *Params, true, false, false, nullptr, 0, nullptr, nullptr);

				ActiveInstances.Add(NewInstance);

				CreateDungeonSession(CurrentRequestDungeonMapName,NewPort);
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

void UIB_GameInstance::DestroyDungeonSession(const FString& DungeonMapName)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub) return;

	if (!SessionInterface.IsValid()) return;


	FName SessionFName(*DungeonMapName);
	FNamedOnlineSession* Session = SessionInterface->GetNamedSession(SessionFName);
	if (Session)
	{
		int32 FoundPort = -1;
		if (Session->SessionSettings.Get(FName("PORT"), FoundPort))
		{
			ToDeletePort = FoundPort; // 여기서 포트 백업
		}
	}

	SessionInterface->DestroySession(SessionFName);
	
}

void UIB_GameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (IsRunningDedicatedServer())
	{
		FString Msg = FString::Printf(TEXT("%d"), ToDeletePort);
		FTCHARToUTF8 Convert(*Msg);
		const uint8* Data = (const uint8*)Convert.Get();

		FIPv4Address LobbyAddr;
		FIPv4Address::Parse(TEXT("43.201.223.63"), LobbyAddr); // Replace with actual lobby server IP

		TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		Addr->SetIp(LobbyAddr.Value);
		Addr->SetPort(6000);

		FSocket* SendSocket = FUdpSocketBuilder(TEXT("DungeonNotifySender"))
			.AsReusable()
			.WithBroadcast()
			.WithSendBufferSize(2 * 1024);

		int32 BytesSent = 0;
		SendSocket->SendTo(Data, Msg.Len(), BytesSent, *Addr);
		SendSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SendSocket);

		UE_LOG(LogTemp, Log, TEXT("Sent shutdown notification to lobby for port %d"), ToDeletePort);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Client LeaveSession: %s"), *SessionName.ToString());
	}
}



int32 UIB_GameInstance::GetAvailablePort()
{
	if (ActiveInstances.Num() == 0)
	{
		// 아무 인스턴스도 없으면 시작 포트로
		return 8000;
	}
	if (ActiveInstances.Num() == 1)
	{
		// 인스턴스가 하나뿐일 때
		int32 FirstPort = ActiveInstances[0].Port;
		return (FirstPort == 8000) ? 8001 : 8000; // 8000부터 시작한다는 전제
	}

	for (int32 i = 0; i < ActiveInstances.Num() - 1; ++i)
	{
		int32 CurrentPort = ActiveInstances[i].Port;
		int32 NextPort = ActiveInstances[i + 1].Port;

		if (NextPort - CurrentPort > 1)
		{
			return CurrentPort + 1;
		}
	}
	// 공백이 없으면 마지막 포트 다음
	return ActiveInstances.Last().Port + 1;
}

void UIB_GameInstance::ClientLeaveLobbySession(const FString& DungeonName)
{
	if (!SessionInterface) return;

	UE_LOG(LogTemp, Log, TEXT("Client Try To Leave Dungeon : %s"), *DungeonName);

	FName DungeonSessionName(*DungeonName);
	SessionInterface->DestroySession(DungeonSessionName);
}

void UIB_GameInstance::StartListeningForDungeonShutdown()
{
	UE_LOG(LogTemp, Log, TEXT("StartListeningPort"));

	FIPv4Address Addr;
		FIPv4Address::Parse(TEXT("0.0.0.0"), Addr); // 모든 주소에서 받음
		FIPv4Endpoint Endpoint(Addr, 6000); // 수신 포트 (예: 6000)
	
		ListenerSocket = FUdpSocketBuilder(TEXT("DungeonShutdownListener"))
			.AsNonBlocking()
			.AsReusable()
			.BoundToEndpoint(Endpoint)
			.WithReceiveBufferSize(2 * 1024 * 1024); // (옵션) 버퍼 사이즈
	
		if (ListenerSocket)
		{
			UE_LOG(LogTemp, Log, TEXT("Listening for dungeon shutdown messages on port 6000"));
			GetWorld()->GetTimerManager().SetTimer(ListenTimerHandle, this, &UIB_GameInstance::PollSocket, 0.1f, true);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("No ListenerSocket"));
		}
}

void UIB_GameInstance::PollSocket()
{

	UE_LOG(LogTemp, Log, TEXT("Try To PollSocket"));

	if (!ListenerSocket) return;
	
		uint8 Buffer[1024];
		uint32 BytesRead = 0;
		TSharedRef<FInternetAddr> Sender = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	
		while (ListenerSocket->HasPendingData(BytesRead))
		{
			int32 Read = 0;
			ListenerSocket->RecvFrom(Buffer, sizeof(Buffer), Read, *Sender);
			FString Received = FString(UTF8_TO_TCHAR((const char*)Buffer)).Left(Read);
			int32 PortToRemove = FCString::Atoi(*Received);
			UE_LOG(LogTemp, Log, TEXT("Received shutdown message from port: %d"), PortToRemove);
			RemoveDungeonInstance(PortToRemove);
		}
}

void UIB_GameInstance::RemoveDungeonInstance(int32 Port)
{
	UE_LOG(LogTemp, Log, TEXT("TryRemovePort: %d"), ToDeletePort);
	UE_LOG(LogTemp, Log, TEXT(" ActiveInstance: %d"), ActiveInstances.Num());
	for (int32 i = 0; i < ActiveInstances.Num(); i++)
	{
		UE_LOG(LogTemp, Log, TEXT("Checking ActiveInstance[%d].Port = %d"), i, ActiveInstances[i].Port);
		if (ActiveInstances[i].Port == ToDeletePort)
		{
			UE_LOG(LogTemp, Log, TEXT("PortSearched: %d"), ToDeletePort);
			if (ActiveInstances[i].DungeonServerHandle.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("PortRemoved: %d"), ToDeletePort);
				FPlatformProcess::TerminateProc(ActiveInstances[i].DungeonServerHandle, true);
				FPlatformProcess::CloseProc(ActiveInstances[i].DungeonServerHandle);
			}
			// 타이머 클리어
			GetWorld()->GetTimerManager().ClearTimer(ActiveInstances[i].LockTimerHandle);
			ActiveInstances.RemoveAt(i);
			break;
		}
	}
}
