#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../../IB_Framework/IB_GAS/AbilitySystem/IB_RPGAbilityTypes.h"
#include "../../../Inventory/ItemTypes.h"

#include "GameplayEffectTypes.h"
#include "IB_MultiPlayGame/Interfaces/InteractInterface.h"
#include "WeaponBase.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:
	void SetWeaponParams(const FWeaponParams& Params);

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UAnimMontage>> WeaponAttackMontageArray;
	UFUNCTION()
	void SetCharacterAttack(float AttackPower);
	UFUNCTION()
	void SetItemDefinition(const FMasterItemDefinition& InItemDefinition);
	UFUNCTION()
	FMasterItemDefinition GetItemDefinition();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Custom Values | GamePlayEffect")
	TSubclassOf<UGameplayEffect> WeaponAttackEffect;


	

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USceneComponent> RootSceneComponent;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> WeaponSkeletalMesh;
	UPROPERTY()
	float WeaponAttackPower;
	UPROPERTY()
	FActiveGameplayEffectHandle ActiveGEHandle;
	UPROPERTY(Replicated)
	FMasterItemDefinition ItemDefinition;
private:

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetWeaponMesh(USkeletalMesh* InMesh);

	


	

	
};
