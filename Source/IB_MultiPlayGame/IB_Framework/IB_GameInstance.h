#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "IB_GameInstance.generated.h"

class UInventoryComponent;

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	RequestCreateDungeonSession(const FString& DugeonName);


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

	FOnlineSessionSettings SessionSettings;
	

private:
	FOnlineSessionSearch SessionSearch;

	TArray<FOnlineSessionSearchResult> SessionSearchResults;

	int32 LobbySessionCounter;
};
