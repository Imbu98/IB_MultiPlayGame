#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../../IB_Framework/IB_GAS/AbilitySystem/IB_RPGAbilityTypes.h"
#include "IB_MultiPlayGame/Interfaces/InteractInterface.h"
#include "WeaponBase.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();


public:
	void SetWeaponParams(const FWeaponParams& Params);

	UPROPERTY(BlueprintReadWrite)
	FDamageEffectInfo DamageEffectInfo;

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UAnimMontage>> WeaponAttackMontageArray;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> RootSceneComponent;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> WeaponSkeletalMesh;

	
};
