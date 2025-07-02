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

bool FUserInventory::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	EquippableInventory.NetSerialize(Ar, Map, bOutSuccess);
	ConsumableInventory.NetSerialize(Ar, Map, bOutSuccess);
	ETCInventory.NetSerialize(Ar, Map, bOutSuccess);

	return true;
}

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
	
	DOREPLIFETIME(UInventoryComponent, CachedUserInventory);
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsFirstStart==true)
	{
		FGameplayTag NoneTag = FGameplayTag::RequestGameplayTag(TEXT("Item.None"));
		CachedUserInventory.ConsumableInventory.Initialize(Inventorysize, NoneTag, 0);
		CachedUserInventory.EquippableInventory.InventoryType = EItemTypes::Item_Equippable;
		
		CachedUserInventory.EquippableInventory.Initialize(Inventorysize, NoneTag, 0);
		CachedUserInventory.ConsumableInventory.InventoryType = EItemTypes::Item_Consumable;
		
		CachedUserInventory.ETCInventory.Initialize(Inventorysize, NoneTag, 0);
		CachedUserInventory.ETCInventory.InventoryType = EItemTypes::Item_ETC;
		IsFirstStart = false;
	}

}

bool UInventoryComponent::AddItem(const FMasterItemDefinition& ItemDefinition,int32 NumItems )
{
	AActor* Owner = GetOwner();
	if (Owner == nullptr)
	{
		return false;
	}

	// 이 컴포넌트를 가지고 있는 캐릭터가 클라이언트면 
	if (!Owner->HasAuthority())
	{
		ServerAddItem(NumItems,ItemDefinition);
		return false;
	}
	// QuestItem Add 처리
	if (AIB_RPGPlayerController* PlayerController = Cast<AIB_RPGPlayerController>(Owner))
	{
		if(AIB_MainChar* IB_MainChar = Cast<AIB_MainChar>(PlayerController->GetPawn()))
		{
			FString ItemTagString = ItemDefinition.ItemTag.ToString();
			if (IB_MainChar->OnObjectiveIdCalledDelegate.IsBound())
			{
				IB_MainChar->OnObjectiveIdCalledDelegate.Broadcast(ItemTagString, NumItems);
			}
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Collect %s, : %d"), *ItemTagString, NumItems));
		}
	}

	//PackageInventory(CachedInventory);

	// 나중에 얘도 데이터 테이블에서 data가져와서 stackable같은걸로 처리를 해줄 수 있으면 해주자
	return DefinitionItemAdd(ItemDefinition,NumItems);
}

void UInventoryComponent::ServerAddItem_Implementation(int32 NumItems, const FMasterItemDefinition& ItemDefinition)
{
	AddItem(ItemDefinition,NumItems);
}

void UInventoryComponent::SwapItemsInPackagedInventory(FPackagedInventory& CachedInventoryRef, int32 IndexA, int32 IndexB)
{
	
	if (!GetOwner()->HasAuthority())
	{
		ServerSwapItem(CachedInventoryRef,IndexA,IndexB);
		return;
	}
	if (!CachedInventoryRef.ItemTags.IsValidIndex(IndexA) || !CachedInventoryRef.ItemTags.IsValidIndex(IndexB))
	{
		return;
	}

	CachedInventoryRef.ItemTags.Swap(IndexA, IndexB);
	
	CachedInventoryRef.ItemQuantities.Swap(IndexA, IndexB);

	DefinitionSetInventory(CachedInventoryRef);
	
	ClientUpdateUserInventory(CachedUserInventory,CachedInventoryRef.InventoryType);

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Swapping %d <--> %d"), IndexA, IndexB));
}

void UInventoryComponent::ServerSwapItem_Implementation(FPackagedInventory CachedInventoryRef, int32 IndexA, int32 IndexB)
{
	SwapItemsInPackagedInventory(CachedInventoryRef, IndexA, IndexB);
}


FUserInventory& UInventoryComponent::GetCachedUserInventory()
{
	return CachedUserInventory;
}

int32 UInventoryComponent::GetInventorySize()
{
	return Inventorysize;
}

void UInventoryComponent::OnRep_CachedUserInventory()
{
	if (bOwnerLocallyControlled)
	{
		//UserInventoryPackageDelegate.Broadcast(CachedUserInventory);
	}
}

void UInventoryComponent::ClientUpdateUserInventory_Implementation(const FUserInventory& UserInventory,
	const EItemTypes InventoryType)
{
	if (bOwnerLocallyControlled)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this,InventoryType]()
		{
			UserInventoryPackageDelegate.Broadcast(CachedUserInventory,InventoryType);
		},0.05f,false);
		
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

	const TArray<FPackagedInventory*> Inventories = {
		&CachedUserInventory.EquippableInventory,
		&CachedUserInventory.ConsumableInventory,
		&CachedUserInventory.ETCInventory
	};

	for (const FPackagedInventory* Inventory : Inventories)
	{
		for (int32 Index = 0; Index < Inventory->ItemTags.Num(); ++Index)
		{
			if (Inventory->ItemTags[Index] == Tag)
			{
				return Inventory->ItemQuantities.IsValidIndex(Index) ? Inventory->ItemQuantities[Index] : 0;
			}
		}
	}
	return 0;
}

// 몇번째 아이템이 넘어왔는지 
void UInventoryComponent::UseItem(const FMasterItemDefinition& DynamicItemData,const int32& SlotIndex,int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (Owner == nullptr)
	{
		return;
	}

	if (!Owner->HasAuthority())
	{
		ServerUseItem(DynamicItemData,SlotIndex,NumItems);
		return;
	}
	

	DefinitionItemUse(DynamicItemData,SlotIndex,NumItems);

}

void UInventoryComponent::RemoveItem(const FMasterItemDefinition& DynamicItemData,const int32& SlotIndex,int32 NumItems)
{

	FGameplayTag NoneTag = FGameplayTag::RequestGameplayTag(TEXT("Item.None"));

	FPackagedInventory Inventory =  DefinitionGetInventory(DynamicItemData.ItemTag);
	
	if (!Inventory.ItemTags.IsValidIndex(SlotIndex) || !Inventory.ItemDefinitions.IsValidIndex(SlotIndex))
	{
		return;
	}
	
	if (Inventory.ItemTags[SlotIndex] == DynamicItemData.ItemTag)
	{
		Inventory.ItemQuantities[SlotIndex] -= NumItems;
		
		if (Inventory.ItemQuantities[SlotIndex] <=0)
		{
			Inventory.ItemTags[SlotIndex] = NoneTag;
			Inventory.ItemDefinitions[SlotIndex] = FMasterItemDefinition();
			Inventory.ItemQuantities[SlotIndex] = 0;
		}

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
			FString::Printf(TEXT("Slot %d cleared. Tag: %s"), SlotIndex, *DynamicItemData.ItemTag.ToString()));

		DefinitionSetInventory(Inventory);
		// 위젯 갱신 등 브로드캐스트
		ClientUpdateUserInventory(CachedUserInventory,Inventory.InventoryType);
	}
}

void UInventoryComponent::ServerUseItem_Implementation( const FMasterItemDefinition& DynamicItemData,const int32& SlotIndex,int32 NumItems)
{
	FPackagedInventory Inventory =  DefinitionGetInventory(DynamicItemData.ItemTag);
	
	if (Inventory.ItemTags.Contains(DynamicItemData.ItemTag))
	{
		UseItem(DynamicItemData,SlotIndex,NumItems);
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


void UInventoryComponent::DefinitionItemUse(const FMasterItemDefinition& DynamicItemData,const int32& SlotIndex,int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;
	if (!Owner->HasAuthority()) return;

	AIB_RPGPlayerController* IB_RPGPlayerContoller = Cast<AIB_RPGPlayerController>(Owner);
	if (!IsValid(IB_RPGPlayerContoller)) return;

	const FGameplayTag ConsumableTag = FGameplayTag::RequestGameplayTag(FName("Item.Consumable"));
	const FGameplayTag EquippableTag = FGameplayTag::RequestGameplayTag(FName("Item.Equippable"));
	
	const FMasterItemDefinition StaticItemDefinition = GetItemDefinitionByTag(DynamicItemData.ItemTag);
	
	if (StaticItemDefinition.ItemTag.MatchesTag(ConsumableTag))
	{
		if (UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
		{
			if (IsValid(StaticItemDefinition.ConsumableProps.ItemEffectClass))
			{
				const FGameplayEffectContextHandle ContextHandle = OwnerAsc->MakeEffectContext();
				const FGameplayEffectSpecHandle SpecHandle = OwnerAsc->MakeOutgoingSpec(StaticItemDefinition.ConsumableProps.ItemEffectClass,
					StaticItemDefinition.ConsumableProps.ItemEffectLevel, ContextHandle);
				OwnerAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
	else if (DynamicItemData.ItemTag.MatchesTag(EquippableTag))
	{
		IB_RPGPlayerContoller->EquipItem(DynamicItemData);
	}

	RemoveItem(DynamicItemData,SlotIndex,NumItems);

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Magenta, FString::Printf(TEXT("Server Item Used : %s"), *DynamicItemData.ItemTag.ToString()));

	// Armor를 장착하면 DT_Equippable에 저장되있는 GameplayEffect로 Defense를 +
	/*else if (DynamicItemData.ItemTag.MatchesTag(EquippableArmorTag))
	{
		 gameeffect를 사용할지 고민해보자
		if (UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner))
		{
			if (IsValid(StaticItemData.ConsumableProps.ItemEffectClass))
			{
				const FGameplayEffectContextHandle ContextHandle = OwnerAsc->MakeEffectContext();
				const FGameplayEffectSpecHandle SpecHandle = OwnerAsc->MakeOutgoingSpec(StaticItemData.ConsumableProps.ItemEffectClass,
					DynamicItemData.AbilityLevel, ContextHandle);
				OwnerAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}*/
	
}

bool UInventoryComponent::DefinitionItemAdd(const FMasterItemDefinition& ItemDefinition,int32 NumItems)
{
	FGameplayTag NoneTag = FGameplayTag::RequestGameplayTag(TEXT("Item.None"));
	FGameplayTag EquippableTag = FGameplayTag::RequestGameplayTag(TEXT("Item.Equippable"));

	const FMasterItemDefinition Item = GetItemDefinitionByTag(ItemDefinition.ItemTag);
	const bool bStackable = Item.bStackable;

	FPackagedInventory Inventory = DefinitionGetInventory(ItemDefinition.ItemTag);

	if (!Inventory.ItemTags.Contains(NoneTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory Is Full"));
		return false;
	}

	if (Inventory.ItemTags.Contains(ItemDefinition.ItemTag))
	{
		// 여러 개 Stack 할 수 없는 아이템 ( 무기 , 방어구 등)
		if (bStackable==false)
		{
			if (NumItems > 0)
			{
				int32 NoneIndex = Inventory.ItemTags.IndexOfByKey(NoneTag);
				if (NoneIndex != INDEX_NONE && Inventory.ItemQuantities.IsValidIndex(NoneIndex))
				{
					Inventory.ItemTags[NoneIndex] = ItemDefinition.ItemTag;
					Inventory.ItemQuantities[NoneIndex] = NumItems;
					Inventory.ItemDefinitions[NoneIndex] = ItemDefinition;

					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Sever Item Added To Inventory %s, qty:%d"), *ItemDefinition.ItemTag.ToString(), NumItems));

					DefinitionSetInventory(Inventory);

					ClientUpdateUserInventory(CachedUserInventory,Inventory.InventoryType);
					return true;
				}
			}
		}
		// Stackable == true면 수량++
		else
		{
			int32 FoundIndex = Inventory.ItemTags.IndexOfByKey(ItemDefinition.ItemTag);
			if (FoundIndex != INDEX_NONE && Inventory.ItemQuantities.IsValidIndex(FoundIndex))
			{
				Inventory.ItemQuantities[FoundIndex] += NumItems;
				if (Inventory.ItemQuantities[FoundIndex] <= 0)
				{
					Inventory.ItemTags[FoundIndex] = NoneTag;

					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Sever Item Added To Inventory %s, qty:%d"), *ItemDefinition.ItemTag.ToString(), NumItems));

					DefinitionSetInventory(Inventory);
					
					ClientUpdateUserInventory(CachedUserInventory,Inventory.InventoryType);
					return true;
				}
			}
		}
		
	}
	else
	{
		int32 NoneIndex = Inventory.ItemTags.IndexOfByKey(NoneTag);
		if (NoneIndex != INDEX_NONE && Inventory.ItemQuantities.IsValidIndex(NoneIndex))
		{
			Inventory.ItemTags[NoneIndex] = ItemDefinition.ItemTag;
			Inventory.ItemQuantities[NoneIndex] = NumItems;
			Inventory.ItemDefinitions[NoneIndex] = ItemDefinition;

			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Sever Item Added To Inventory %s, qty:%d"), *ItemDefinition.ItemTag.ToString(), NumItems));
			DefinitionSetInventory(Inventory);
			
			ClientUpdateUserInventory(CachedUserInventory,Inventory.InventoryType);

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

void UInventoryComponent::DefinitionSetInventory(FPackagedInventory& Inventory)
{

	if (Inventory.InventoryType==EItemTypes::Item_Equippable)
	{
		CachedUserInventory.EquippableInventory = Inventory;
	}
	else if (Inventory.InventoryType==EItemTypes::Item_Consumable)
	{
		CachedUserInventory.ConsumableInventory = Inventory;
	}
	else if (Inventory.InventoryType==EItemTypes::Item_ETC)
	{
		CachedUserInventory.ETCInventory = Inventory;
	}
}

FPackagedInventory UInventoryComponent::DefinitionGetInventory(const FGameplayTag& ItemTag)
{
	FGameplayTag EquippableTag = FGameplayTag::RequestGameplayTag("Item.Equippable");
	FGameplayTag ConsumableTag = FGameplayTag::RequestGameplayTag("Item.Consumable");
	FGameplayTag ETCTag = FGameplayTag::RequestGameplayTag("Item.ETC");

	if (ItemTag.MatchesTag(EquippableTag) )
	{
		return CachedUserInventory.EquippableInventory;
	}
	if (ItemTag.MatchesTag(ConsumableTag))
	{
		return CachedUserInventory.ConsumableInventory;
	}
	if (ItemTag.MatchesTag(ETCTag))
	{
		return CachedUserInventory.ETCInventory;
	}
	
	UE_LOG(Imbu, Error, TEXT("NOInventory"));
	
	return FPackagedInventory();
	
}
