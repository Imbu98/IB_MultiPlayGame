#pragma once

#include "CoreMinimal.h"
#include "../../Interfaces/InteractInterface.h"
#include "../../Inventory/ItemTypes.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "BaseSpawnedItem.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class IB_MULTIPLAYGAME_API ABaseSpawnedItem : public AActor , public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	ABaseSpawnedItem();

protected:
	virtual void BeginPlay() override;
	
	virtual FString InteractWith_Implementation(APlayerController* PlayerController) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> ItemStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* CapsuleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	UNiagaraComponent* NiagaraComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* WidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemMesh)
	UMaterialInterface* ItemOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | DataTable")
	UDataTable* DT_ItemData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | DataTable")
	UDataTable* DT_SwordData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | DataTable")
	UDataTable* DT_AxeData;
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> ItemDropEffect;
	UPROPERTY(EditAnywhere, Category = "Custom Values | DropEffect")
	TObjectPtr<UNiagaraSystem>  CommonDropEffect;
	UPROPERTY(EditAnywhere, Category = "Custom Values | DropEffect")
	TObjectPtr<UNiagaraSystem>  RareDropEffect;
	UPROPERTY(EditAnywhere, Category = "Custom Values | DropEffect")
	TObjectPtr<UNiagaraSystem>  EpicDropEffect;
	UPROPERTY(EditAnywhere, Category = "Custom Values | DropEffect")
	TObjectPtr<UNiagaraSystem>  LegendaryDropEffect;
	

public:
	FMasterItemDefinition ItemDefinition;


public:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UFUNCTION()
	void InitializeWithTag(FGameplayTag InTag,EItemRarity ItemRarity);
	UFUNCTION()
	void SetMeshFromTag(FGameplayTag InItemTag);
	UFUNCTION()
	void SetItemParams(FGameplayTag InItemTag);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetMeshFromTag(FGameplayTag InItemTag);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetDropEffect(EItemRarity ItemRarity);
	

};
