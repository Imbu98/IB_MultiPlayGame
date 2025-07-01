#include "StateComponent.h"

UStateComponent::UStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UStateComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ResetCurrentState();
}

void UStateComponent::SetCurrentState(FGameplayTag CurrentTag)
{
	if (GetOwner())
	{
		if (!GetOwner()->HasAuthority())
		{
			ServerSetCurrentState(CurrentTag);
			return;
		}
		CurrentStateTag = CurrentTag;
	}
}

void UStateComponent::ServerSetCurrentState_Implementation(FGameplayTag CurrentTag)
{
	if (GetOwner())
	{
		if (GetOwner()->HasAuthority())
		{
			SetCurrentState(CurrentTag);
			
		}
	}
}

FGameplayTag UStateComponent::GetCurrentState()
{
	if (GetOwner())
	{
		if (GetOwner()->HasAuthority())
		{
			ServerGetCurrentState();
		}
		return CurrentStateTag;
	}
}

void UStateComponent::ServerGetCurrentState_Implementation()
{
	if (GetOwner())
	{
		if (GetOwner()->HasAuthority())
		{
			GetCurrentState();
		}
	}
}

void UStateComponent::ResetCurrentState()
{
	if (GetOwner())
	{
		if (!GetOwner()->HasAuthority())
		{
			ServerResetCurrentState();
		}
		CurrentStateTag = FGameplayTag::RequestGameplayTag("Player.Idle"); 
	}
	
}

void UStateComponent::ServerResetCurrentState_Implementation()
{
	if (GetOwner())
	{
		if (GetOwner()->HasAuthority())
		{
			ResetCurrentState();
		}
	}
}



