#include "EquippableBase.h"
#include "Components\SkeletalMeshComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"


AEquippableBase::AEquippableBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

}

void AEquippableBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEquippableBase, ItemDefinition);
}


void AEquippableBase::BeginPlay()
{
	Super::BeginPlay();

}

void AEquippableBase::SetItemDefinition(const FMasterItemDefinition& InItemDefinition)
{
	ItemDefinition = InItemDefinition;
}

FMasterItemDefinition AEquippableBase::GetItemDefinition()
{
	return ItemDefinition;
}

void AEquippableBase::UnEquipItem()
{
	if (AActor* OwnerCharacter = GetOwner())
	{
		if (UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter))
		{
			if (ActiveGEHandle.IsValid())
			{
				OwnerAsc->RemoveActiveGameplayEffect(ActiveGEHandle);
			}
		}
	}
}

