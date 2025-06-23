#include "IB_RPGAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"


void UIB_RPGAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UIB_RPGAttributeSet,Health,COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UIB_RPGAttributeSet,MaxHealth,COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UIB_RPGAttributeSet,Mana,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UIB_RPGAttributeSet,MaxMana,COND_None,REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UIB_RPGAttributeSet, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UIB_RPGAttributeSet, Attack, COND_None, REPNOTIFY_Always);
}

void UIB_RPGAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute==GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(),0.f,GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute==GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(),0.f,GetMaxMana()));
	}
}

void UIB_RPGAttributeSet::OnREP_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UIB_RPGAttributeSet,Health,OldHealth);
}

void UIB_RPGAttributeSet::OnREP_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UIB_RPGAttributeSet,MaxHealth,OldMaxHealth);
}

void UIB_RPGAttributeSet::OnREP_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UIB_RPGAttributeSet,Mana,OldMana);
}

void UIB_RPGAttributeSet::OnREP_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UIB_RPGAttributeSet,MaxMana,OldMaxMana);
}

void UIB_RPGAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UIB_RPGAttributeSet, Defense, OldDefense);

	UE_LOG(LogTemp, Warning, TEXT("Defense:%f"), Defense.GetCurrentValue());
}

void UIB_RPGAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UIB_RPGAttributeSet, Attack, OldAttack);
}

void UIB_RPGAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& Delta)
{
	Super::PreAttributeChange(Attribute, Delta);

	if (Attribute == GetHealthAttribute())
	{
		float Def = FMath::Clamp(Defense.GetCurrentValue(), 0.f, 100.f);
		Delta = Delta * (200 - Def) / 200;
	}
}