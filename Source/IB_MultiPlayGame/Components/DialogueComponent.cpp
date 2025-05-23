#include "DialogueComponent.h"


UDialogueComponent::UDialogueComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UDialogueComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UDialogueComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

