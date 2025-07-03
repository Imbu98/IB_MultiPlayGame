#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "IB_GameInstance.generated.h"

class UInventoryComponent;

UCLASS()
class IB_MULTIPLAYGAME_API UIB_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	void FindOrCreateDungeonSession(const FString& DungeonID, APlayerController* Requestor);

	void CreateDungeonSession(const FString& DungeonID, APlayerController* Requestor);


protected:
	void OnFindSessionComplete(bool bWasSuccessful);

	// 세션 생성 완료
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	// 세션 참가 완료
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	
private:
	// 세션 인터페이스 참조
	IOnlineSessionPtr SessionInterface;

	// 세션 검색 객체
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	// 현재 던전 ID와 요청 PC 저장
	FString CurrentDungeonID;
	
	TWeakObjectPtr<APlayerController> CurrentRequestor;

	// 중복되지 않는 세션 이름 생성
	FString GenerateSessionName(const FString& DungeonID);
	
	// 10초 입장 제한 타이머 시작
	void StartSessionCloseTimer(FName SessionName);

	// 10초 후 입장 막기
	void CloseSessionEntry();

	// 타이머 핸들
	FTimerHandle SessionCloseTimerHandle;

	// 현재 닫힌 세션 이름 저장
	FName ClosedSessionName;

	// 입장 제한 플래그
	bool bIsSessionEntryClosed = false;
	
	
};
