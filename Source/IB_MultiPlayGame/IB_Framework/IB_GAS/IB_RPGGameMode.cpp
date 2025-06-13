#include "IB_RPGGameMode.h"
#include "../IB_GameInstanceSubSystem.h"
#include "IB_RPGPlayerController.h"


void AIB_RPGGameMode::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	// PIE 환경에서는 URL.Port 사용
	GameModePortNumber = GetWorld()->URL.Port;
	UE_LOG(LogTemp, Log, TEXT("PIE Dungeon started on port: %d"), GameModePortNumber);
#else
	// 패키징된 서버에서는 커맨드라인 인자 파싱
	FString PortStr;
	if (FParse::Value(FCommandLine::Get(), TEXT("port="), PortStr))
	{
		GameModePortNumber = FCString::Atoi(*PortStr);
		UE_LOG(LogTemp, Log, TEXT("Standalone Dungeon started on port: %d"), GameModePortNumber);
	}
#endif
}

UIB_CharacterClassInfo* AIB_RPGGameMode::GetCharacterClassDefaultInfo() const
{
	return ClassDefaults;
}

UProjectile_Info* AIB_RPGGameMode::GetProjectileInfo() const
{
	return ProjectileInfo;
}

UWeapon_Info* AIB_RPGGameMode::GetWeaponInfo() const
{
	return WeaponInfo;
}

void AIB_RPGGameMode::HandleDungeonClear()
{
	if (UIB_GameInstanceSubSystem* Subsystem = GetGameInstance()->GetSubsystem<UIB_GameInstanceSubSystem>())
	{
		Subsystem->RemoveDungeonInstance(GameModePortNumber); // 미리 저장한 InstanceID 필요
	}
}

void AIB_RPGGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerController* PC = Cast<APlayerController>(Exiting);
	if (PC)
	{
		ActiveControllers.Remove(PC);
		UE_LOG(LogTemp, Log, TEXT("Removed controller. Remaining: %d"), ActiveControllers.Num());
		UE_LOG(LogTemp, Log, TEXT("To RemovePort: %d"), GameModePortNumber);

		if (ActiveControllers.Num() == 0)
		{
			HandleDungeonClear();
		}
	}
}

void AIB_RPGGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	APlayerController* PC = Cast<APlayerController>(NewPlayer);
	if (PC)
	{
		ActiveControllers.Add(NewPlayer);
	}
}
