#include "W_Overlay.h"

#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"


void UW_Overlay::SetHealthBar(float CurrentHealth, float MaxHealth)
{
	if (PB_HealthBar)
	{
		PB_HealthBar->SetPercent(UKismetMathLibrary::SafeDivide(CurrentHealth, MaxHealth));
	}
	
}

void UW_Overlay::SetManaBar(float CurrentMana, float MaxMana)
{
	if (PB_ManaBar)
	{
		PB_ManaBar->SetPercent(UKismetMathLibrary::SafeDivide(CurrentMana, MaxMana));
	}
}
