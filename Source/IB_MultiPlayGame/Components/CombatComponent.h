#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "../Inventory/ItemTypes.h"
#include "CombatComponent.generated.h"

class AWeaponBase;
class AArmorBase;
class AEquippableBase;


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

	UPROPERTY()
	TArray<AEquippableBase*> EquippedItems;

	UPROPERTY(ReplicatedUsing = OnRep_EquippbaleBase)
	TArray<FMasterItemDefinition> EquippedItemsDefinition;

public:
	UFUNCTION()
	void ResetAttack();
	UFUNCTION()
	void SetEquippedItem(AActor* SpawnedItem);
	UFUNCTION()
	void UnEquipItem(const FMasterItemDefinition& Iteminfo);
	UFUNCTION()
	void OnRep_EquippbaleBase();

		
	
};
