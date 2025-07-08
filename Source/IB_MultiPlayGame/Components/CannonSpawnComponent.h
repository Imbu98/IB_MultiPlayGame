#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CannonSpawnComponent.generated.h"

class ACannonActor;
class ACannonPawn;
 
USTRUCT(BlueprintType)
struct FCannonSpawnParameters : public FTableRowBase
{
	 GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CannonLevel=1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> CannonMesh;


};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IB_MULTIPLAYGAME_API UCannonSpawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCannonSpawnComponent();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UDataTable> DT_CannonSpawnData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Cannon")
	TSubclassOf<ACannonPawn> BP_CannonPawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Cannon")
	TSubclassOf<ACannonActor> BP_CannonActor;

public:
	UFUNCTION()
	void SpawnOwnedCannonActor(AIB_RPGPlayerController* IB_PlayerController);
	UFUNCTION(Server, Reliable)
	void ServerSpawnOwnedCannonActor(AIB_RPGPlayerController* IB_PlayerController);
	UFUNCTION()
	void SpawnOwnedCannonPawn(AIB_RPGPlayerController* IB_PlayerController);

	
};
