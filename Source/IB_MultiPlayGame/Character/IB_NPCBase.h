#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interfaces/InteractInterface.h"
#include "IB_NPCBase.generated.h"

class UQuestGiverComponent;

UCLASS()
class IB_MULTIPLAYGAME_API AIB_NPCBase : public ACharacter , public IInteractInterface
{
	GENERATED_BODY()
		
public:
	AIB_NPCBase();

	virtual FString InteractWith_Implementation(APlayerController* CharacterController) override;

	virtual void LookAt_Implementation() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	class UWidgetComponent* WBP_Interact;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	class UMaterialInstance* InteractOverlayMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UQuestGiverComponent> QuestGiverComponent;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_InteractedActor)
	TObjectPtr<AActor> InteractedActor;

	UFUNCTION(BlueprintCallable)
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable)
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnRep_InteractedActor();

	
};

