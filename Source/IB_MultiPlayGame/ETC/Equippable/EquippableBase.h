#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../IB_Framework/IB_GAS/AbilitySystem/IB_RPGAbilityTypes.h"
#include "../../Inventory/ItemTypes.h"

#include "GameplayEffectTypes.h"
#include "EquippableBase.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API AEquippableBase : public AActor
{
	GENERATED_BODY()

public:
	AEquippableBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:

	UFUNCTION()
	void SetItemDefinition(const FMasterItemDefinition& InItemDefinition);
	UFUNCTION()
	FMasterItemDefinition GetItemDefinition();


protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Replicated)
	FMasterItemDefinition ItemDefinition;



};
