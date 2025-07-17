#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "../Inventory/ItemTypes.h"
#include "CombatComponent.generated.h"

class AWeaponBase;
class AArmorBase;
class AEquippableBase;

UENUM(BlueprintType)
enum class EWeaponHand : uint8
{
	Main,
	Sub,
};

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
	UPROPERTY(Replicated)
	int32 AttackCount=0;

	UPROPERTY()
	bool IsAttachedWeapon= false;

	UPROPERTY(Replicated)
	bool CanContinueAttack=true;
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedItemsDefinition)
	TArray<FMasterItemDefinition> EquippedItemsDefinition;

	UPROPERTY()
	TObjectPtr<AWeaponBase> MainHandWeapon;
	UPROPERTY()
	TObjectPtr<AWeaponBase> SubHandWeapon;

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
	UFUNCTION(Server, Reliable)
	void ServerSetCanContinueAttack();
	UFUNCTION()
	void SetEquippedItem(AActor* SpawnedItem,const uint8& DefinitionMainNumber);
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
	UFUNCTION()
	void SetWeapon(AWeaponBase* InWeapon, const uint8& DefinitionMainNumber);
	UFUNCTION(Server,Reliable)
	void ServerEnableCollisionTrace(const uint8& DefinitionMainNumber);
	UFUNCTION(Server, Reliable)
	void ServerDisableCollisionTrace();
	
};
