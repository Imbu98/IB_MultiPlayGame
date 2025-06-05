#include "ItemSpawnComponent.h"
#include "../ETC/BaseSpawnedItem/BaseSpawnedItem.h"
#include "../ETC/BaseSpawnedItem/SpawnedItemSetProperty.h"
#include "../../IB_MultiPlayGame/IB_Framework/FunctionLibrary/IB_BlueprintFunctionLibrary.h"
#include "../../IB_MultiPlayGame/IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "Kismet/GameplayStatics.h"


UItemSpawnComponent::UItemSpawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UItemSpawnComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UItemSpawnComponent::SetFinalItemTypeAndDrop()
{
	
	TArray<AIB_RPGPlayerController*> LocalSessionPlayers;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AIB_RPGPlayerController* PC = Cast<AIB_RPGPlayerController>(It->Get()))
		{
			if (PC->GetPawn()) // Pawn이 있다는 건 접속 중이라는 뜻
			{
				LocalSessionPlayers.Add(PC);
			}
		}
	}

	for (AIB_RPGPlayerController* PC : LocalSessionPlayers)
	{
		FGameplayTag EquippableTag = FGameplayTag::RequestGameplayTag(TEXT("Item.Equippable"));
		FGameplayTag Consumable = FGameplayTag::RequestGameplayTag(TEXT("Item.Consumable"));
		FGameplayTag ItemTag = FGameplayTag();
		EItemRarity ItemRarity = EItemRarity::None;

		FGameplayTag ItemTypeTag = GetRandomDropItemTag();

		if (ItemTypeTag.MatchesTag(EquippableTag))
		{
			ItemTag = SetEquippableType();
			ItemRarity = SetItemRarity();

		}
		else if (ItemTypeTag.MatchesTag(Consumable))
		{
			ItemTag = SetConsumbaleType();

		}

		FActorSpawnParameters ActorSpawnParameters;
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetOwner()->GetActorLocation());


		if (ABaseSpawnedItem* BaseSpawnedItem = GetWorld()->SpawnActor<ABaseSpawnedItem>(BP_BaseSpawnedItem, SpawnTransform, ActorSpawnParameters))
		{
			BaseSpawnedItem->SetReplicates(true);
			BaseSpawnedItem->SetOwner(PC);                      
			BaseSpawnedItem->bOnlyRelevantToOwner = true;

			BaseSpawnedItem->ItemStaticMesh->SetSimulatePhysics(true);
			BaseSpawnedItem->InitializeWithTag(ItemTag, ItemRarity);

			
		}
	}

	
}

FGameplayTag  UItemSpawnComponent::GetRandomDropItemTag()
{
	if (!DT_DropItemTypeProbability) return FGameplayTag();

	TArray<FItemTypeProbability*> AllRows;
	DT_DropItemTypeProbability->GetAllRows(TEXT("DropProbabilityContext"), AllRows);

	float Total = 0.f;
	for (auto Row : AllRows)
		Total += Row->ItemTypeProbability;

	float Rand = FMath::FRandRange(0.f, Total);
	float Accumulated = 0.f;

	for (auto Row : AllRows)
	{
		Accumulated += Row->ItemTypeProbability;
		if (Rand <= Accumulated)
			return Row->ItemTag;
	}
	return FGameplayTag(); // fallback


}

FGameplayTag UItemSpawnComponent::SetEquippableType()
{
	if (DT_EquppableTypeProbability)
	{
		TArray<FItemTypeProbability*> AllRows;
		DT_EquppableTypeProbability->GetAllRows<FItemTypeProbability>(TEXT("WeaponTypeProbabilityContext"), AllRows);

		float TotalProbability = 0.f;

		for (FItemTypeProbability* Row : AllRows)
		{
			TotalProbability += Row->ItemTypeProbability;
		}

		float RandomValue = FMath::FRandRange(0.f, TotalProbability);
		float Accumulated = 0.f;

		for (FItemTypeProbability* Row : AllRows)
		{
			Accumulated += Row->ItemTypeProbability;
			if (RandomValue <= Accumulated)
			{
				return Row->ItemTag;
			}
		}
		// Fallback: return first tag or empty
		return AllRows.Num() > 0 ? AllRows[0]->ItemTag : FGameplayTag();
	}
	return FGameplayTag();
}

FGameplayTag UItemSpawnComponent::SetConsumbaleType()
{
	if (DT_ConsumbaleTypeProbability)
	{
		TArray<FItemTypeProbability*> AllRows;
		DT_ConsumbaleTypeProbability->GetAllRows<FItemTypeProbability>(TEXT("ConsumableTypeProbabilityContext"), AllRows);

		float TotalProbability = 0.f;

		for (FItemTypeProbability* Row : AllRows)
		{
			TotalProbability += Row->ItemTypeProbability;
		}

		float RandomValue = FMath::FRandRange(0.f, TotalProbability);
		float Accumulated = 0.f;

		for (FItemTypeProbability* Row : AllRows)
		{
			Accumulated += Row->ItemTypeProbability;
			if (RandomValue <= Accumulated)
			{
				return Row->ItemTag;
			}
		}
		// Fallback: return first tag or empty
		return AllRows.Num() > 0 ? AllRows[0]->ItemTag : FGameplayTag();
	}
	return FGameplayTag();
}

EItemRarity UItemSpawnComponent::SetItemRarity()
{
	if (DT_ItemRarityProbability)
	{
		TArray<FItemRarityProbability*> AllRows;
		DT_ItemRarityProbability->GetAllRows<FItemRarityProbability>(TEXT("ConsumableTypeProbabilityContext"), AllRows);

		float TotalProbability = 0.f;

		for (FItemRarityProbability* Row : AllRows)
		{
			TotalProbability += Row->ItemRarityProbability;
		}

		float RandomValue = FMath::FRandRange(0.f, TotalProbability);
		float Accumulated = 0.f;

		for (FItemRarityProbability* Row : AllRows)
		{
			Accumulated += Row->ItemRarityProbability;
			if (RandomValue <= Accumulated)
			{
				return Row->ItemRarity;
			}
		}
		// Fallback: return first tag or empty
		return AllRows.Num() > 0 ? AllRows[0]->ItemRarity : EItemRarity::None;
	}
	return EItemRarity::None;
}


