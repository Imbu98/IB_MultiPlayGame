#pragma once

#include "CoreMinimal.h"
#include "IB_RPGAbilityTypes.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;
class AProjectileBase;
//class AWeaponBase;

USTRUCT()
struct FWeaponParams
{
	GENERATED_BODY()

	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<AWeaponBase> WeaponClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMesh> WeaponSkeletalMesh;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMesh> WeaponMesh;

	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UAnimMontage>> WeaponAttackMontageArray;

};

USTRUCT()
struct FProjectileParams
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMesh> ProjectileMesh;

	UPROPERTY(EditDefaultsOnly)
	float InitialSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly)
	float GravityScale = 1.f;

	UPROPERTY(EditDefaultsOnly)
	bool bShouldBounce = false;

	UPROPERTY(EditDefaultsOnly)
	float Bounciness = 0.6f;

};

USTRUCT(BlueprintType)
struct FDamageEffectInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> AvatarActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffect = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;
};
