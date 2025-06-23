#include "ArmorBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"


AArmorBase::AArmorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

}

void AArmorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AArmorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArmorBase, ItemDefinition);
}

void AArmorBase::SetCharacterDefense(const float& ItemDefense)
{
	ArmorDefense = ItemDefense;

	if (!IsValid(ArmorDefenseEffect)) return;

	if (AActor* OwnerCharacter = GetOwner())
	{
		if (UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter))
		{
			FGameplayEffectSpecHandle SpecHandle = OwnerAsc->MakeOutgoingSpec(ArmorDefenseEffect, 1.f, OwnerAsc->MakeEffectContext());
			if (SpecHandle.IsValid())
			{
				SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Combat.Data.Defense")), ArmorDefense);
				ActiveDefenseGEHandle = OwnerAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

void AArmorBase::SetItemDefinition(const FMasterItemDefinition& InItemDefinition)
{
	ItemDefinition = InItemDefinition;
}

FMasterItemDefinition AArmorBase::GetItemDefinition()
{
	return ItemDefinition;
}


