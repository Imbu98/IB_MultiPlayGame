#include "W_CannonWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/ProgressBar.h"

void UW_CannonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (T_EquipWeaponCaution)
	{
		T_EquipWeaponCaution->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_CannonWidget::UpdatePowerProgressBar(const float InCannonPowner,const float InMaxCannonPowner)
{
	if (PB_Power != nullptr)
	{
		PB_Power->SetPercent(InCannonPowner /InMaxCannonPowner);
	}
}
