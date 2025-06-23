#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "../../../Inventory/ItemTypes.h"
#include "ArmorBase.generated.h"

class UGameplayEffect;

UCLASS()
class IB_MULTIPLAYGAME_API AArmorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AArmorBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | GamePlayEffect")
	TSubclassOf<UGameplayEffect> ArmorDefenseEffect;
	UPROPERTY()
	FActiveGameplayEffectHandle ActiveDefenseGEHandle;
	UPROPERTY()
	float ArmorDefense;
	UPROPERTY(Replicated)
	FMasterItemDefinition ItemDefinition;



public:
	void SetCharacterDefense(const float& ItemDefense);
	UFUNCTION()
	void SetItemDefinition(const FMasterItemDefinition& InItemDefinition);
	UFUNCTION()
	FMasterItemDefinition GetItemDefinition();
	
};
