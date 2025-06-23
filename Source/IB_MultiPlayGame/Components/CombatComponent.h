#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "../Inventory/ItemTypes.h"
#include "CombatComponent.generated.h"

class AWeaponBase;
class AArmorBase;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IB_MULTIPLAYGAME_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	UPROPERTY()
	int32 AttackCount;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeaponBase> EquippedWeaponBase;
	UPROPERTY()
	TObjectPtr<AArmorBase> EquippedHelmet;
	UPROPERTY()
	TObjectPtr<AArmorBase> EquippedChest;
	UPROPERTY()
	TObjectPtr<AArmorBase> EquippedPants;
	UPROPERTY()
	TObjectPtr<AArmorBase> EquippedGloves;
	UPROPERTY()
	TObjectPtr<AArmorBase> EquippedBoots;

public:
	UFUNCTION()
	void ResetAttack();
	UFUNCTION()
	void SetEquippedItem(AActor* SpawnedItem);
	UFUNCTION()
	void OnRep_EquippedWeapon();
		
	
};
