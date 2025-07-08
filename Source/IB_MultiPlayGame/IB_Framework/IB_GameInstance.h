#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "IB_GameInstance.generated.h"

class UInventoryComponent;

USTRUCT()
struct FDungeonInstanceInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FName SessionName; // 스팀 세션 이름 (고유 ID)

	UPROPERTY()
	FString DungeonID; // 던전 유형 ID (예: "Dungeon1")

	UPROPERTY()
	FString ConnectString; // 이 인스턴스로 접속할 IP:Port

	UPROPERTY()
	int32 CurrentPlayers; // 현재 인원

	UPROPERTY()
	int32 MaxPlayers; // 최대 인원

	UPROPERTY()
	FTimerHandle SessionTimeoutTimerHandle; // 10초 타이머 핸들

	UPROPERTY()
	bool bIsAdvertised; // 현재 스팀에 광고 중인지 여부

	UPROPERTY()
	bool bIsEntryClosed; // 입장 제한 여부
	UPROPERTY() // 이 부분 추가
	FString MapName; // 이 인스턴스가 사용하는 맵 이름 (예: Dungeon1_Map)

	FDungeonInstanceInfo()
		: SessionName(NAME_None)
		, DungeonID(TEXT(""))
		, ConnectString(TEXT(""))
		, CurrentPlayers(0)
		, MaxPlayers(4)
		, bIsAdvertised(false)
		, bIsEntryClosed(false)
		, MapName(TEXT("")) // 기본값 초기화
	{}
};

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
	TMap<FName, FDungeonInstanceInfo> ActiveDungeonInstances;

	TSharedPtr<FOnlineSessionSettings> SessionSettings;
	

private:
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	TArray<FOnlineSessionSearchResult> SessionSearchResults;

	int32 LobbySessionCounter;
};
