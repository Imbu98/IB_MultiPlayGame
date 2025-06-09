#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CannonSpawnManager.generated.h"

class ACannon;

UCLASS()
class IB_MULTIPLAYGAME_API ACannonSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ACannonSpawnManager();

protected:
	virtual void BeginPlay() override;

public:

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Values | Cannon")
	TSubclassOf<ACannon> CannonPawn; 

	UFUNCTION()
	void SpawnOwnedCannon(AIB_RPGPlayerController* IB_PlayerController);



	
};
