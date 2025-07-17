#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "IB_GameInstance.generated.h"

USTRUCT()
struct FDungeonInstanceData
{
	GENERATED_BODY()

	int32 InstanceID = -1;
	int32 Port = -1;
	int32 CurrentPlayers = 0;
	bool bLocked = false;
	FTimerHandle LockTimerHandle;
	FProcHandle DungeonServerHandle;
};

class UInventoryComponent;
class AIB_RPGPlayerController;

UCLASS()
class IB_MULTIPLAYGAME_API UIB_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CreateLobbySession();

	void FindLobbySession();

	void RequestFindOrCreateDungeonSession(const FString& DungeonName,AIB_RPGPlayerController* Player);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CreateDungeonSession(const FString& DungeonName,const int32& GeneratedPort);

	
	void DestroyDungeonSession(const FString& DungeonName);

	int32 GetAvailablePort();

	UFUNCTION()
	void ClientLeaveLobbySession(const FString& DungeonName);

	void StartListeningForDungeonShutdown();

	void PollSocket();

	void RemoveDungeonInstance(int32 Port);

protected:
	// Online Subsystem 콜백
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful); // 세션 파괴 콜백 추가


private:
	IOnlineSessionPtr SessionInterface;

	// 클라이언트 측에서 요청할 때 사용되는 임시 변수
	FString ClientRequest_DungeonID;
	TWeakObjectPtr<APlayerController> ClientRequest_Requestor;

	// 서버 측에서 관리하는 활성 던전 인스턴스 맵
	// Key: SessionName (고유 ID), Value: FDungeonInstanceInfo
	TMap<FName, FDungeonInstanceData> ActiveDungeonInstances;

	FOnlineSessionSettings SessionSettings;

	UPROPERTY()
	TArray<FDungeonInstanceData> ActiveInstances;

	FSocket* ListenerSocket;

	FTimerHandle ListenTimerHandle;

	

	

private:
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	TArray<FOnlineSessionSearchResult> SessionSearchResults;

	TSharedPtr<FOnlineSessionSearch> DungeonSessionSearch;

	TArray<FOnlineSessionSearchResult> DungeonSessionSearchResults;

	int32 LobbySessionCounter;

	FString CurrentRequestDungeonMapName;

	TObjectPtr<AIB_RPGPlayerController> CurrentRequestPlayer;

	int32 NextInstanceID = 1;

	int32 StartPort = 8000;

	int32 ToDeletePort = -1;
};
