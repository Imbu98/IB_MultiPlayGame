#include "InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "IB_MultiPlayGame/IB_Framework/FunctionLibrary/IB_BlueprintFunctionLibrary.h"
#include "IB_MultiPlayGame/Inventory/ItemTypesToTables.h"
#include "Net/UnrealNetwork.h"
#include "../WidgetController/InventoryWidgetController.h"
#include "Kismet/GameplayStatics.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"


bool FPackagedInventory::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	SafeNetSerializeTArray_WithNetSerialize<100>(Ar, ItemTags, Map);
	SafeNetSerializeTArray_Default<100>(Ar, ItemQuantities);

	bOutSuccess = true;
	return true;
}

UInventoryComponent::UInventoryComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, CachedInventory);
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::AddItem(const FGameplayTag& ItemTag, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (Owner == nullptr)
	{
		return;
	}

	// 이 컴포넌트를 가지고 있는 캐릭터가 클라이언트면 
	if (!Owner->HasAuthority())
	{
		ServerAddItem(ItemTag, NumItems);
		return;
	}

	if (InventoryTagMap.Contains(ItemTag))
	{
		InventoryTagMap[ItemTag] += NumItems;
	}
	else
	{
		InventoryTagMap.Emplace(ItemTag, NumItems);
	}

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Sever Item Added To Inventory %s, qty:%d"), *ItemTag.ToString(), NumItems));

	PackageInventory(CachedInventory);

	InventoryPackageDelegate.Broadcast(CachedInventory);
}

void UInventoryComponent::ServerAddItem_Implementation(const FGameplayTag& ItemTag, int32 NumItems)
{
	AddItem(ItemTag, NumItems);
}

void UInventoryComponent::PackageInventory(FPackagedInventory& OutInventory)
{
	OutInventory.ItemTags.Empty();
	OutInventory.ItemQuantities.Empty();

	for (const auto& Pair : InventoryTagMap)
	{
		if (Pair.Value > 0)
		{
			OutInventory.ItemTags.Add(Pair.Key);
			OutInventory.ItemQuantities.Add(Pair.Value);
		}
	}
}

void UInventoryComponent::SwapItemsInPackagedInventory(FPackagedInventory& CachedInventoryRef, int32 IndexA, int32 IndexB)
{
	if (!CachedInventoryRef.ItemTags.IsValidIndex(IndexA) || !CachedInventoryRef.ItemTags.IsValidIndex(IndexB))
	{
		return;
	}

	// 태그 스왑
	CachedInventoryRef.ItemTags.Swap(IndexA, IndexB);
	// 수량 스왑
	CachedInventoryRef.ItemQuantities.Swap(IndexA, IndexB);

	InventoryPackageDelegate.Broadcast(CachedInventoryRef);

	AIB_RPGPlayerController* IB_RPGPC = Cast< AIB_RPGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IB_RPGPC)
	{

		UInventoryWidgetController* InventoryWidgetController = IB_RPGPC->GetInventoryWidgetController();
		if (InventoryWidgetController)
		{
			InventoryWidgetController->UpdateInventory(CachedInventoryRef);
		}
	}


	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Swapping %d <--> %d"), IndexA, IndexB));
}

void UInventoryComponent::ReConstructInventoryMap(const FPackagedInventory& Inventory)
{
	InventoryTagMap.Empty();

	for (int32 i = 0; i < Inventory.ItemTags.Num();++i)
	{
		InventoryTagMap.Emplace(Inventory.ItemTags[i], Inventory.ItemQuantities[i]);

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::Printf(TEXT("Tag Added: %s // Quantity Added :%d"), *Inventory.ItemTags[i].ToString(), Inventory.ItemQuantities[i]));
	}
}

TMap<FGameplayTag, int32> UInventoryComponent::GetInventoryTagMap()
{
	return InventoryTagMap;
}

FPackagedInventory& UInventoryComponent::GetCachedInventory()
{
	return CachedInventory;
}

void UInventoryComponent::OnRep_CachedInventory()
{
	if (bOwnerLocallyControlled)
	{
		//ReConstructInventoryMap(CachedInventory);
		InventoryPackageDelegate.Broadcast(CachedInventory);
	}

}

void UInventoryComponent::UseItem(const FGameplayTag& ItemTag, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (Owner == nullptr)
	{
		return;
	}

	if (!Owner->HasAuthority())
	{
		ServerUseItem(ItemTag, NumItems);
		return;
	}
	const FMasterItemDefinition Item = GetItemDefinitionByTag(ItemTag);

	if (UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
	{
		if (IsValid(Item.ConsumableProps.ItemEffectClass))
		{
			const FGameplayEffectContextHandle ContextHandle = OwnerAsc->MakeEffectContext();
			const FGameplayEffectSpecHandle SpecHandle = OwnerAsc->MakeOutgoingSpec(Item.ConsumableProps.ItemEffectClass,
				Item.ConsumableProps.ItemEffectLevel, ContextHandle);
			OwnerAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());


			// 여기를 고치자
			AddItem(ItemTag, -1);

		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Magenta, FString::Printf(TEXT("Server Item Used : %s"), *ItemTag.ToString()));
}

void UInventoryComponent::ServerUseItem_Implementation(const FGameplayTag& ItemTag, int32 NumItems)
{
	if (InventoryTagMap.Contains(ItemTag))
	{
		UseItem(ItemTag, NumItems);
	}

}


FMasterItemDefinition UInventoryComponent::GetItemDefinitionByTag(const FGameplayTag& ItemTag)const
{
	checkf(InventoryDefinition, TEXT("No Inventory Definitions Inside Components %s"), *GetNameSafe(this));

	for (const auto& Pair : InventoryDefinition->TagsToTables)
	{
		if (ItemTag.MatchesTag(Pair.Key))
		{
			return *UIB_BlueprintFunctionLibrary::GetDataTableRowByTag<FMasterItemDefinition>(Pair.Value, ItemTag);
		}
	}

	return FMasterItemDefinition();
}