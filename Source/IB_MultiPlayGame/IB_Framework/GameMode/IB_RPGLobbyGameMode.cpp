#include "IB_RPGLobbyGameMode.h"
#include "GameFramework/PlayerState.h"


void AIB_RPGLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

UIB_CharacterClassInfo* AIB_RPGLobbyGameMode::GetCharacterClassDefaultInfo() const
{
	return ClassDefaults;
}

UProjectile_Info* AIB_RPGLobbyGameMode::GetProjectileInfo() const
{
	return ProjectileInfo;
}

UWeapon_Info* AIB_RPGLobbyGameMode::GetWeaponInfo() const
{
	return WeaponInfo;
}

UArmorInfo* AIB_RPGLobbyGameMode::GetArmorInfo() const
{
	return ArmorInfo;
}

void AIB_RPGLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		if (APlayerState* PlayerState= NewPlayer->GetPlayerState<APlayerState>())
		{
			PlayerState->bOnlyRelevantToOwner = true;
		}
	}
}
