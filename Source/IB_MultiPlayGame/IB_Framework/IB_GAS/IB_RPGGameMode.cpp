#include "IB_RPGGameMode.h"


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
