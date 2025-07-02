#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Inventory/ItemTypes.h"
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
	
	EItemTypes InventoryType;

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

USTRUCT()
struct FUserInventory
{
	GENERATED_BODY()

	virtual ~FUserInventory() = default;

	// 필수 매크로
	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess);

	UPROPERTY()
	FPackagedInventory EquippableInventory;
	UPROPERTY()
	FPackagedInventory ConsumableInventory;
	UPROPERTY()
	FPackagedInventory ETCInventory;
};

template<>
struct TStructOpsTypeTraits<FUserInventory> : public TStructOpsTypeTraitsBase2<FUserInventory>
{
	enum
	{
		WithNetSerializer = true
	};
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FInventoryPackagedSignature, const FUserInventory&,const EItemTypes);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class IB_MULTIPLAYGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	FInventoryPackagedSignature UserInventoryPackageDelegate;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	bool AddItem(const FMasterItemDefinition& ItemDefinition=FMasterItemDefinition(),int32 NumItems = 1);

	UFUNCTION(BlueprintCallable)
	void UseItem(const FMasterItemDefinition& DynamicItemData,const int32& SlotIndex,int32 NumItems = 1);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(const FMasterItemDefinition& DynamicItemData,const int32& SlotIndex,int32 NumItems=1);

	UFUNCTION()
	FMasterItemDefinition GetItemDefinitionByTag(const FGameplayTag& ItemTag)const;

	FUserInventory& GetCachedUserInventory();

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
	TMap<FGameplayTag, int32> EquippableInventoryTagMap;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TMap<FGameplayTag, int32> ConsumableInventoryTagMap;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TMap<FGameplayTag, int32> ETCInventoryTagMap;
	
	UPROPERTY(ReplicatedUsing = OnRep_CachedUserInventory)
	FUserInventory CachedUserInventory;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UItemTypesToTables> InventoryDefinition;

	UPROPERTY()
	int32 Inventorysize=15;
	

	UFUNCTION(Server, Reliable)
	void ServerAddItem(int32 NumItems, const FMasterItemDefinition& ItemDefinition);

	UFUNCTION(Server, Reliable)
	void ServerUseItem(const FMasterItemDefinition& DynamicItemData,const int32& SlotIndex,int32 NumItems=1);

	UFUNCTION(Server, Reliable)
	void ServerSwapItem(FPackagedInventory CachedInventoryRef,int32 IndexA, int32 IndexB);
	
	UFUNCTION()
	void OnRep_CachedUserInventory();

	UFUNCTION(Client, Reliable)
	void ClientUpdateUserInventory(const FUserInventory& UserInventory, const EItemTypes InventoryType);

	UFUNCTION()
	void DefinitionItemUse(const FMasterItemDefinition& DynamicItemData,const int32& SlotIndex,int32 NumItems=1);
	UFUNCTION()
	bool DefinitionItemAdd(const FMasterItemDefinition& ItemDefinition,int32 NumItems=1);

	// 어떤 인벤토리인지 불러오기
	UFUNCTION()
	void DefinitionSetInventory(FPackagedInventory& Inventory);
	// 인벤토리 종류에 따라 넣기
	UFUNCTION()
	FPackagedInventory DefinitionGetInventory(const FGameplayTag& ItemTag);


};
