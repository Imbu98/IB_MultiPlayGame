#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "IB_MultiPlayGame/Inventory/ItemTypes.h"
#include "InventoryComponent.generated.h"

USTRUCT()
struct FPackagedInventory
{
	GENERATED_BODY()

	virtual ~FPackagedInventory() = default;

	UPROPERTY()
	TArray<FGameplayTag> ItemTags;

	UPROPERTY()
	TArray<int32> ItemQuantities;

	UPROPERTY()
	TArray<FMasterItemDefinition> ItemDefinitions;

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

	void Initialize(int32 InventorySize, FGameplayTag DefaultTag, int32 DefaultQuantity = 0,FMasterItemDefinition ItemDefinition= FMasterItemDefinition())
	{
		ItemTags.SetNum(InventorySize);
		ItemQuantities.SetNum(InventorySize);
		ItemDefinitions.SetNum(InventorySize);

		for (int32 i = 0; i < InventorySize; ++i)
		{
			ItemTags[i] = DefaultTag;
			ItemQuantities[i] = DefaultQuantity;
			ItemDefinitions[i] = FMasterItemDefinition();
		}
	}
};

template<>
struct TStructOpsTypeTraits<FPackagedInventory> : TStructOpsTypeTraitsBase2<FPackagedInventory>
{
	enum
	{
		WithNetSerializer = true
	};
};




DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryPackagedSignature, const FPackagedInventory&);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class IB_MULTIPLAYGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	FInventoryPackagedSignature InventoryPackageDelegate;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	bool AddItem(const FMasterItemDefinition& ItemDefinition=FMasterItemDefinition(),int32 NumItems = 1);

	UFUNCTION(BlueprintCallable)
	void UseItem(const FMasterItemDefinition& DynamicItemData,const int32& SlotIndex,int32 NumItems = 1);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(const FMasterItemDefinition& DynamicItemData,const int32& SlotIndex,int32 NumItems=1);

	UFUNCTION()
	FMasterItemDefinition GetItemDefinitionByTag(const FGameplayTag& ItemTag)const;

	void ReConstructInventoryMap(const FPackagedInventory& Inventory);

	void PackageInventory(FPackagedInventory& OutInventory);

	TMap<FGameplayTag, int32> GetInventoryTagMap();

	FPackagedInventory& GetCachedInventory();

	int32 GetInventorySize();

	bool bOwnerLocallyControlled = false;

	void SwapItemsInPackagedInventory(FPackagedInventory& CachedInventory, int32 IndexA, int32 IndexB);

	UFUNCTION()
	int32 QueryInventory(const FString& ItemTagString);

	UPROPERTY()
	bool IsFirstStart = true;


protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TMap<FGameplayTag, int32> InventoryTagMap;

	UPROPERTY(ReplicatedUsing = OnRep_CachedInventory)
	FPackagedInventory CachedInventory;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UItemTypesToTables> InventoryDefinition;

	UPROPERTY()
	int32 Inventorysize=15;
	

	UFUNCTION(Server, Reliable)
	void ServerAddItem(int32 NumItems, const FMasterItemDefinition& ItemDefinition);

	UFUNCTION(Server, Reliable)
	void ServerUseItem(const FMasterItemDefinition& DynamicItemData,const int32& SlotIndex,int32 NumItems=1);

	UFUNCTION(Server, Reliable)
	void ServerSwapItem(int32 IndexA, int32 IndexB);

	UFUNCTION()
	void OnRep_CachedInventory();

	UFUNCTION()
	void DefinitionItemUse(const FMasterItemDefinition& DynamicItemData,const int32& SlotIndex,int32 NumItems=1);
	UFUNCTION()
	bool DefinitionItemAdd(const FMasterItemDefinition& ItemDefinition,int32 NumItems=1);


};
