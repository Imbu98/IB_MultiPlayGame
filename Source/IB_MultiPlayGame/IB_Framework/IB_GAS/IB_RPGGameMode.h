#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "IB_RPGGameMode.generated.h"

class UIB_CharacterClassInfo;
class UProjectile_Info;
class UWeapon_Info;

UCLASS()
class IB_MULTIPLAYGAME_API AIB_RPGGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UIB_CharacterClassInfo* GetCharacterClassDefaultInfo() const;
	UProjectile_Info* GetProjectileInfo() const;
	UWeapon_Info* GetWeaponInfo() const;

private:

	UPROPERTY(EditDefaultsOnly,Category="Custom Values|Class Defaults")
	TObjectPtr<UIB_CharacterClassInfo> ClassDefaults;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Projectiles")
	TObjectPtr<UProjectile_Info> ProjectileInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|Weapons")
	TObjectPtr<UWeapon_Info> WeaponInfo;
	
	
	
};
