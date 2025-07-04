#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
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
	
	void TryAutoConnect();

	UFUNCTION(BlueprintCallable, Category = "Dungeon")
	void RequestDungeonEntry(const FString& InDungeonID, APlayerController* RequestingPC);

	// 서버 측: 던전 인스턴스 찾거나 생성 (클라이언트 요청 처리)
	// 이 함수는 서버에서만 호출되어야 합니다.
	void Server_FindOrCreateDungeonInstance(const FString& InDungeonID, APlayerController* RequestingPC);

	// 서버 측: 새로운 던전 세션 생성
	// 이 함수는 서버에서만 호출되어야 합니다.
	void Server_CreateDungeonSession(const FString& InDungeonID, APlayerController* RequestingPC);


protected:
	// Online Subsystem 콜백
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful); // 세션 파괴 콜백 추가

	// 서버 측: 10초 타이머 시작
	void Server_StartSessionCloseTimer(FName SessionName);

	// 서버 측: 10초 타이머 만료 또는 4명 입장 시 호출
	void Server_CloseSessionEntry(FName SessionName);

	// 서버 측: 세션 이름 생성
	FString GenerateSessionName(const FString& InDungeonID);

	// 서버 측: 플레이어 접속 시 호출 (GameMode에서 호출될 예정)
	void Server_OnPlayerConnectedToDungeon(APlayerController* NewPlayer, const FName SessionName);

	// 서버 측: 플레이어 접속 해제 시 호출 (GameMode에서 호출될 예정)
	void Server_OnPlayerDisconnectedFromDungeon(APlayerController* Player, const FName SessionName);

private:
	IOnlineSessionPtr SessionInterface;

	// 클라이언트 측에서 요청할 때 사용되는 임시 변수
	FString ClientRequest_DungeonID;
	TWeakObjectPtr<APlayerController> ClientRequest_Requestor;

	// 서버 측에서 관리하는 활성 던전 인스턴스 맵
	// Key: SessionName (고유 ID), Value: FDungeonInstanceInfo
	TMap<FName, FDungeonInstanceInfo> ActiveDungeonInstances;

	// 서버 측: 세션 검색 결과 저장 (서버가 세션 검색할 때 사용)
	TSharedPtr<FOnlineSessionSearch> ServerSessionSearch;
};
