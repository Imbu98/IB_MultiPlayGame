#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CannonSpawnManager.generated.h"

class ACannonPawn;
class AAIController;

UCLASS()
class IB_MULTIPLAYGAME_API ACannonSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ACannonSpawnManager();

protected:
	virtual void BeginPlay() override;
	


	
};
