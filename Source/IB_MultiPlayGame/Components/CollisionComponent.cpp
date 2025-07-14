#include "CollisionComponent.h"


UCollisionComponent::UCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UCollisionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

