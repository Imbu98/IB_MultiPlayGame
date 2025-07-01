#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "StateComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IB_MULTIPLAYGAME_API UStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStateComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	FGameplayTag CurrentStateTag;

public:
	UFUNCTION()
	void SetCurrentState(FGameplayTag CurrentTag);
	UFUNCTION(Server,Reliable)
	void ServerSetCurrentState(FGameplayTag CurrentTag);
	UFUNCTION(BlueprintCallable)
	FGameplayTag GetCurrentState();
	UFUNCTION(Server,Reliable)
	void ServerGetCurrentState();
	UFUNCTION(BlueprintCallable)
	void ResetCurrentState();
	UFUNCTION(Server,Reliable)
	void ServerResetCurrentState();

		
	
};
