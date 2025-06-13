#include "IB_GameInstanceSubSystem.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UIB_GameInstanceSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UIB_GameInstanceSubSystem::Deinitialize()
{
	Super::Deinitialize();
}

int32 UIB_GameInstanceSubSystem::LaunchNewDungeonServer(const FString& BaseMapName)
{

	/*if (!IsRunningDedicatedServer())
	{
		UE_LOG(LogTemp, Warning, TEXT("LaunchNewDungeonServer called from non-dedicated server"));
		return -1;
	}*/

	int32 NewPort = GetAvailablePort();

	FDungeonInstanceData NewInstance;
	NewInstance.InstanceID = NextInstanceID++;
	NewInstance.Port = NewPort;
	NewInstance.CurrentPlayers = 1;
	NewInstance.bLocked = false;

	FTimerDelegate LockDelegate;
	LockDelegate.BindUObject(this, &UIB_GameInstanceSubSystem::LockInstance, NewInstance.InstanceID);
	GetWorld()->GetTimerManager().SetTimer(NewInstance.LockTimerHandle, LockDelegate, 10.0f, false);

	FString Params = FString::Printf(TEXT("%s -server  -log -port=%d -nosteam"), *BaseMapName, NewPort);
	FString ServerPath = TEXT("D:\\IB_MultiplayGame\\Package\\WindowsServer\\IB_MultiPlayGameServer.exe");
	NewInstance.DungeonServerHandle = FPlatformProcess::CreateProc(*ServerPath, *Params, true, false, false, nullptr, 0, nullptr, nullptr);

	ActiveInstances.Add(NewInstance);

	return NewPort;
}

FString UIB_GameInstanceSubSystem::RequestDungeonURL(const FString& BaseMapName)
{
	for (FDungeonInstanceData& Instance : ActiveInstances)
	{
		if (!Instance.bLocked && Instance.CurrentPlayers < 4)
		{
			++Instance.CurrentPlayers;
			return FString::Printf(TEXT("127.0.0.1:%d"), Instance.Port);
		}
	}

	int32 Port = LaunchNewDungeonServer(BaseMapName);
	return FString::Printf(TEXT("127.0.0.1:%d"), Port);
}

void UIB_GameInstanceSubSystem::LockInstance(int32 InstanceID)
{
	for (FDungeonInstanceData& Instance : ActiveInstances)
	{
		if (Instance.InstanceID == InstanceID)
		{
			Instance.bLocked = true;
			break;
		}
	}
}

int32 UIB_GameInstanceSubSystem::GetAvailablePort()
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

void UIB_GameInstanceSubSystem::RemoveDungeonInstance(int32 Port)
{
	UE_LOG(LogTemp, Log, TEXT("TryRemovePort: %d"), Port);
	for (int32 i = 0; i < ActiveInstances.Num(); ++i)
	{
		UE_LOG(LogTemp, Log, TEXT("Checking ActiveInstance[%d].Port = %d"), i, ActiveInstances[i].Port);
		if (ActiveInstances[i].Port == Port)
		{
			UE_LOG(LogTemp, Log, TEXT("PortSearched: %d"), Port);
			if (ActiveInstances[i].DungeonServerHandle.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("PortRemoved: %d"), Port);
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
