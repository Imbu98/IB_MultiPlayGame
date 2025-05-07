#include "W_EnemyOverHeadBars.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"


void UW_EnemyOverHeadBars::SetProgressBarPercent(float CurrentHealth, float MaxHealth)
{
	if (PB_HealthBar)
	{
			if (PB_HealthBar)
			{
				PB_HealthBar->SetPercent(UKismetMathLibrary::SafeDivide(CurrentHealth, MaxHealth));
			}
	}
}
