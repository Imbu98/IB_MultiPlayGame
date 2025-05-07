#include "IB_RPGPlayerState.h"
#include "IB_RPGAbilitySystemComponent.h"


AIB_RPGPlayerState::AIB_RPGPlayerState()
{
	SetNetUpdateFrequency(100.0f);
	SetMinNetUpdateFrequency(66.f);

	IB_RPGAbilitySystemComp = CreateDefaultSubobject<UIB_RPGAbilitySystemComponent>("IB_AbilitySystemComponent");
	IB_RPGAbilitySystemComp->SetIsReplicated(true);
	IB_RPGAbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	IB_RPGAttributeSet=CreateDefaultSubobject<UIB_RPGAttributeSet>("IB_AttributeSet");
}

UAbilitySystemComponent* AIB_RPGPlayerState::GetAbilitySystemComponent() const
{
	return IB_RPGAbilitySystemComp;
}

UIB_RPGAbilitySystemComponent* AIB_RPGPlayerState::GetIB_RPGAbilitySystemComponent() const
{
	return IB_RPGAbilitySystemComp;
}

UIB_RPGAttributeSet* AIB_RPGPlayerState::GetIB_RPGAttributeSet() const
{
	return IB_RPGAttributeSet;
}
