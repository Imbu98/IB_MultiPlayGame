#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemSpawnComponent.generated.h"

class ABaseSpawnedItem;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IB_MULTIPLAYGAME_API UItemSpawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UItemSpawnComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Custom Values|DataTable")
	UDataTable* DT_DropItemTypeProbability;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | DataTable")
	UDataTable* DT_EquppableTypeProbability;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | DataTable")
	UDataTable* DT_ConsumbaleTypeProbability;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | DataTable")
	UDataTable* DT_ItemRarityProbability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Asset")
	TSubclassOf<ABaseSpawnedItem> BP_BaseSpawnedItem;

	UFUNCTION()
	void  SetFinalItemTypeAndDrop();
	UFUNCTION()
	FGameplayTag  GetRandomDropItemTag();
	UFUNCTION()
	FGameplayTag  SetEquippableType();
	UFUNCTION()
	FGameplayTag  SetConsumbaleType();
	UFUNCTION()
	EItemRarity  SetItemRarity();


		
	
};
