#pragma once

#include "CoreMinimal.h"
#include "../../Interfaces/InteractInterface.h"
#include "../../Inventory/ItemTypes.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "BaseSpawnedItem.generated.h"

UCLASS()
class IB_MULTIPLAYGAME_API ABaseSpawnedItem : public AActor , public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	ABaseSpawnedItem();

protected:
	virtual void BeginPlay() override;
	
	FString InteractWith_Implementation(APlayerController* PlayerController);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ItemStaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* CapsuleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* WidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemMesh)
	UMaterialInterface* ItemOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | DataTable")
	UDataTable* DT_ItemData;
	

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
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetMeshFromTag(FGameplayTag InItemTag);

	

};
