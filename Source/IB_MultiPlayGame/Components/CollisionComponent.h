#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CollisionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IB_MULTIPLAYGAME_API UCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCollisionComponent();

protected:
	virtual void BeginPlay() override;

	
};
