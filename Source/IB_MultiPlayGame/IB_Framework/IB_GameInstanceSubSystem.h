#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/SubsystemCollection.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "IB_GameInstanceSubSystem.generated.h"

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

UCLASS()
class IB_MULTIPLAYGAME_API UIB_GameInstanceSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	int32 LaunchNewDungeonServer(const FString& BaseMapName);
	FString RequestDungeonURL(const FString& BaseMapName);
	void RemoveDungeonInstance(int32 Port);

private:
	void LockInstance(int32 InstanceID);
	int32 GetAvailablePort();
	

	UPROPERTY()
	TArray<FDungeonInstanceData> ActiveInstances;

	int32 NextInstanceID = 1;
	int32 StartPort = 8000;

	UPROPERTY(EditDefaultsOnly)
	FString ServerExecutableName = TEXT("IB_MultiPlayGameServer.exe");

	
	
};
