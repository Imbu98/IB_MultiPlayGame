#include "InventoryComponent.h"
#include "IB_MultiPlayGame/IB_Framework/FunctionLibrary/IB_BlueprintFunctionLibrary.h"
#include "IB_MultiPlayGame/Inventory/ItemTypesToTables.h"
#include "../WidgetController/InventoryWidgetController.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../Interfaces/RPGAbilitySystemInterface.h"
#include "../Character/IB_MainChar.h"
#include "../IB_Framework/IB_GAS/IB_RPGAbilitySystemComponent.h"
#include "../IB_Framework/IB_GameInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "BlueprintGameplayTagLibrary.h"


bool FPackagedInventory::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	SafeNetSerializeTArray_WithNetSerialize<100>(Ar, ItemTags, Map);
	SafeNetSerializeTArray_Default<100>(Ar, ItemQuantities);
	SafeNetSerializeTArray_WithNetSerialize<100>(Ar, ItemDefinitions,Map);

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

	if (IsFirstStart==true)
	{
		FGameplayTag NoneTag = FGameplayTag::RequestGameplayTag(TEXT("Item.None"));
		CachedInventory.Initialize(Inventorysize, NoneTag, 0);
		ReConstructInventoryMap(CachedInventory);
		IsFirstStart = false;
	}

}

bool UInventoryComponent::AddItem(const FGameplayTag& ItemTag, int32 NumItems, const FMasterItemDefinition& ItemDefinition)
{
	AActor* Owner = GetOwner();
	if (Owner == nullptr)
	{
		return false;
	}

	// 이 컴포넌트를 가지고 있는 캐릭터가 클라이언트면 
	if (!Owner->HasAuthority())
	{
		ServerAddItem(ItemTag, NumItems,ItemDefinition);
		return false;
	}
	// QuestItem Add 처리
	if (AIB_RPGPlayerController* PlayerController = Cast<AIB_RPGPlayerController>(Owner))
	{
		if(AIB_MainChar* IB_MainChar = Cast<AIB_MainChar>(PlayerController->GetPawn()))
		{
			FString ItemTagString = ItemTag.ToString();
			if (IB_MainChar->OnObjectiveIdCalledDelegate.IsBound())
			{
				IB_MainChar->OnObjectiveIdCalledDelegate.Broadcast(ItemTagString, NumItems);
			}
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Collect %s, : %d"), *ItemTagString, NumItems));
		}
	}

	//PackageInventory(CachedInventory);

	// 나중에 얘도 데이터 테이블에서 data가져와서 stackable같은걸로 처리를 해줄 수 있으면 해주자
	return DefinitionItemAdd(ItemTag,NumItems,ItemDefinition);
}

void UInventoryComponent::ServerAddItem_Implementation(const FGameplayTag& ItemTag, int32 NumItems, const FMasterItemDefinition& ItemDefinition)
{
	AddItem(ItemTag, NumItems, ItemDefinition);
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
		else
		{
			FGameplayTag NoneTag = FGameplayTag::RequestGameplayTag(TEXT("Item.None"));
			if (NoneTag.IsValid())
			{
				OutInventory.ItemTags.Add(NoneTag);
				OutInventory.ItemQuantities.Add(0);
			}
		}
	}
}

void UInventoryComponent::SwapItemsInPackagedInventory(FPackagedInventory& CachedInventoryRef, int32 IndexA, int32 IndexB)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerSwapItem(IndexA,IndexB);
		return;
	}
	if (!CachedInventoryRef.ItemTags.IsValidIndex(IndexA) || !CachedInventoryRef.ItemTags.IsValidIndex(IndexB))
	{
		return;
	}

	CachedInventoryRef.ItemTags.Swap(IndexA, IndexB);
	
	CachedInventoryRef.ItemQuantities.Swap(IndexA, IndexB);

	InventoryPackageDelegate.Broadcast(CachedInventoryRef);

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Swapping %d <--> %d"), IndexA, IndexB));
}

void UInventoryComponent::ServerSwapItem_Implementation(int32 IndexA, int32 IndexB)
{
	SwapItemsInPackagedInventory(CachedInventory, IndexA, IndexB);
}

void UInventoryComponent::ReConstructInventoryMap(const FPackagedInventory& Inventory)
{
	InventoryTagMap.Empty();

	for (int32 i = 0; i < Inventorysize;++i)
	{
		if (!Inventory.ItemTags.IsValidIndex(i) || !Inventory.ItemQuantities.IsValidIndex(i)) return;

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

int32 UInventoryComponent::GetInventorySize()
{
	return Inventorysize;
}

void UInventoryComponent::OnRep_CachedInventory()
{
	if (bOwnerLocallyControlled)
	{
		InventoryPackageDelegate.Broadcast(CachedInventory);

	}

}

int32 UInventoryComponent::QueryInventory(const FString& ItemTagString)
{
	if (ItemTagString.IsEmpty()) return 0;

	FString TagString = ItemTagString;
	FName TagName(*TagString);

	FGameplayTag Tag;
	if (UGameplayTagsManager::Get().RequestGameplayTag(TagName,false).IsValid())
	{
		Tag = UGameplayTagsManager::Get().RequestGameplayTag(TagName, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameplayTag '%s' is not registered."), *TagString);
	}

	// 단일 FPackagedInventory 안에서 인덱스 탐색 후 수량 return;
	for (int32 Index = 0; Index < CachedInventory.ItemTags.Num(); ++Index)
	{
		if (CachedInventory.ItemTags[Index] == Tag)
		{
			return CachedInventory.ItemQuantities[Index];
		}
	}
	return 0;
}


void UInventoryComponent::UseItem(const FGameplayTag& ItemTag, int32 NumItems, const FMasterItemDefinition& DynamicItemData)
{
	AActor* Owner = GetOwner();
	if (Owner == nullptr)
	{
		return;
	}

	if (!Owner->HasAuthority())
	{
		ServerUseItem(ItemTag, NumItems, DynamicItemData);
		return;
	}
	const FMasterItemDefinition Item = GetItemDefinitionByTag(ItemTag);

	DefinitionItemUse(Item, DynamicItemData);

}

void UInventoryComponent::ServerUseItem_Implementation(const FGameplayTag& ItemTag, int32 NumItems, const FMasterItemDefinition& DynamicItemData)
{
	if (CachedInventory.ItemTags.Contains(ItemTag))
	{
		UseItem(ItemTag, NumItems, DynamicItemData);
	}

}


FMasterItemDefinition UInventoryComponent::GetItemDefinitionByTag(const FGameplayTag& ItemTag)const
{
	checkf(InventoryDefinition, TEXT("No Inventory Definitions Inside Components %s"), *GetNameSafe(this));

	for (const auto& Pair : InventoryDefinition->TagsToTables)
	{
		if (Pair.Value)
		{
			if (ItemTag.MatchesTag(Pair.Key))
			{
				if (const FMasterItemDefinition* ItemDef = UIB_BlueprintFunctionLibrary::GetDataTableRowByTag<FMasterItemDefinition>(Pair.Value, ItemTag))
				{
					return *ItemDef;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("ItemTag '%s' not found in DataTable!"), *ItemTag.ToString());

					return FMasterItemDefinition();
				}
				
			}
		}

		
	}

	return FMasterItemDefinition();
}


void UInventoryComponent::DefinitionItemUse(const FMasterItemDefinition& StaticItemData, const FMasterItemDefinition& DynamicItemData)
{
	AActor* Owner = GetOwner();
	if (Owner == nullptr)
	{
		return;
	}

	const FGameplayTag ConsumableTag = FGameplayTag::RequestGameplayTag(FName("Item.Consumable"));
	const FGameplayTag EquippableTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable"));

	if (StaticItemData.ItemTag.MatchesTag(ConsumableTag))
	{
		if (UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
		{
			if (IsValid(StaticItemData.ConsumableProps.ItemEffectClass))
			{
				const FGameplayEffectContextHandle ContextHandle = OwnerAsc->MakeEffectContext();
				const FGameplayEffectSpecHandle SpecHandle = OwnerAsc->MakeOutgoingSpec(StaticItemData.ConsumableProps.ItemEffectClass,
					StaticItemData.ConsumableProps.ItemEffectLevel, ContextHandle);
				OwnerAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());



			}
		}
	}
	// 차라리 어빌리티 레벨을 바꾸자
	else if (DynamicItemData.ItemTag.MatchesTag(EquippableTag))
	{
		if (UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
		{
			if (UIB_RPGAbilitySystemComponent* IB_RPGAbilitySystemComponent = Cast<UIB_RPGAbilitySystemComponent>(OwnerAsc))
			{
				if (IB_RPGAbilitySystemComponent->Implements<URPGAbilitySystemInterface>())
				{
					IRPGAbilitySystemInterface::Execute_SetDynamicWeapon(IB_RPGAbilitySystemComponent, DynamicItemData.ItemTag, DynamicItemData.AbilityLevel);
				}
			}
		}
	}

	AddItem(DynamicItemData.ItemTag, -1, DynamicItemData);

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Magenta, FString::Printf(TEXT("Server Item Used : %s"), *DynamicItemData.ItemTag.ToString()));
}

bool UInventoryComponent::DefinitionItemAdd(const FGameplayTag& ItemTag,int32 NumItems,const FMasterItemDefinition& ItemDefinition)
{
	FGameplayTag NoneTag = FGameplayTag::RequestGameplayTag(TEXT("Item.None"));
	FGameplayTag EquippableTag = FGameplayTag::RequestGameplayTag(TEXT("Item.Equippable"));

	if (!CachedInventory.ItemTags.Contains(NoneTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory Is Full"));
		return false;
	}

	if (CachedInventory.ItemTags.Contains(ItemTag))
	{
		// EquippableTag (즉, 장비면 새로운 슬롯에 넣어주기)
		if (ItemTag.MatchesTag(EquippableTag))
		{
			if (NumItems > 0)
			{
				
				int32 NoneIndex = CachedInventory.ItemTags.IndexOfByKey(NoneTag);
				if (NoneIndex != INDEX_NONE && CachedInventory.ItemQuantities.IsValidIndex(NoneIndex))
				{
					CachedInventory.ItemTags[NoneIndex] = ItemTag;
					CachedInventory.ItemQuantities[NoneIndex] = NumItems;
					CachedInventory.ItemDefinitions[NoneIndex] = ItemDefinition;

					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Sever Item Added To Inventory %s, qty:%d"), *ItemTag.ToString(), NumItems));

					InventoryPackageDelegate.Broadcast(CachedInventory);
					return true;
				}
			}
			else
			{
				int32 FoundIndex = CachedInventory.ItemTags.IndexOfByKey(ItemTag);
				if (FoundIndex != INDEX_NONE && CachedInventory.ItemQuantities.IsValidIndex(FoundIndex))
				{
					CachedInventory.ItemQuantities[FoundIndex] += NumItems;
					if (CachedInventory.ItemQuantities[FoundIndex] <= 0)
					{
						CachedInventory.ItemTags[FoundIndex] = NoneTag;
						CachedInventory.ItemDefinitions[FoundIndex] = FMasterItemDefinition();

						GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Sever Item Added To Inventory %s, qty:%d"), *ItemTag.ToString(), NumItems));

						InventoryPackageDelegate.Broadcast(CachedInventory);
						return true;
					}
				}
			}
			
		}
		// 장비가 아니면 수량+
		else
		{
			int32 FoundIndex = CachedInventory.ItemTags.IndexOfByKey(ItemTag);
			if (FoundIndex != INDEX_NONE && CachedInventory.ItemQuantities.IsValidIndex(FoundIndex))
			{
				CachedInventory.ItemQuantities[FoundIndex] += NumItems;
				if (CachedInventory.ItemQuantities[FoundIndex] <= 0)
				{
					CachedInventory.ItemTags[FoundIndex] = NoneTag;

					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Sever Item Added To Inventory %s, qty:%d"), *ItemTag.ToString(), NumItems));

					InventoryPackageDelegate.Broadcast(CachedInventory);
					return true;
				}
			}
		}
		
	}
	else
	{
		int32 NoneIndex = CachedInventory.ItemTags.IndexOfByKey(NoneTag);
		if (NoneIndex != INDEX_NONE && CachedInventory.ItemQuantities.IsValidIndex(NoneIndex))
		{
			CachedInventory.ItemTags[NoneIndex] = ItemTag;
			CachedInventory.ItemQuantities[NoneIndex] = NumItems;
			CachedInventory.ItemDefinitions[NoneIndex] = ItemDefinition;

			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Sever Item Added To Inventory %s, qty:%d"), *ItemTag.ToString(), NumItems));
			InventoryPackageDelegate.Broadcast(CachedInventory);

			return true;
		}
		else
		{
			// None 슬롯도 없으면 추가 (혹시나 배열이 확장되는 구조일 경우)
			/*CachedInventory.ItemTags.Add(ItemTag);
			CachedInventory.ItemQuantities.Add(NumItems);*/
			return false;
		}
		

		// 만들어진 맵순서로 다시 CachedInventory 생성
		//PackageInventory(CachedInventory);

		// 그 CachedInventory순서로 맵을 만듦
		
	}

	
	return false;
}
