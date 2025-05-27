#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CheckRoleComponent.generated.h"


UCLASS(Blueprintable,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IB_MULTIPLAYGAME_API UCheckRoleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCheckRoleComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
