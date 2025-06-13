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

protected:
	virtual void BeginPlay() override;

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
	UPROPERTY()
	int32 GameModePortNumber;

	void HandleDungeonClear();

public:

	UPROPERTY()
	TSet<APlayerController*> ActiveControllers;

	virtual void Logout(AController* Exiting) override;
	void PostLogin(APlayerController* NewPlayer) override;
	
	
	
	
};
