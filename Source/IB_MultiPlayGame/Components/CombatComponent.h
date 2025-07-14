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
	bool IsAttachedWeapon= false;

	UPROPERTY()
	bool CanContinueAttack;
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedItemsDefinition)
	TArray<FMasterItemDefinition> EquippedItemsDefinition;

private:
	UPROPERTY()
	TMap<EItemParts,AEquippableBase*> EquippedInstancedItemMap;
	
	UPROPERTY()
	TMap<EItemParts,FMasterItemDefinition> EquippedItemMap;
	
	UFUNCTION(Client,Reliable)
	void ClientSetEquippedItemMap(const TArray<FMasterItemDefinition>& EquippedItemDefinitions);
	

public:
	UFUNCTION()
	void ResetAttack();
	UFUNCTION(Server,Reliable)
	void ServerResetAttack();
	UFUNCTION()
	void SetEquippedItem(AActor* SpawnedItem);
	UFUNCTION()
	void UnEquipItem(const FMasterItemDefinition& ItemInfo,const float& SlotIndex);
	UFUNCTION()
	UAnimMontage* GetWeaponAnimMontage();
	UFUNCTION()
	void OnRep_EquippedItemsDefinition();
	UFUNCTION()
	TMap<EItemParts,FMasterItemDefinition> GetEquippedItemMap();
	UFUNCTION()
	int32 DefinitionIndex(const FMasterItemDefinition& EquipItemDefinition);

		
	
};
