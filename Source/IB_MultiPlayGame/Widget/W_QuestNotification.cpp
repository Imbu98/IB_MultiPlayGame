#include "W_QuestNotification.h"
#include "Components/TextBlock.h"

void UW_QuestNotification::NativeConstruct()
{
	Super::NativeConstruct();

	if (TextBlock_ObjectiveText)
	{
		TextBlock_ObjectiveText->SetText(ObjectiveText);
	}

	if (WAM_Appear)
	{
		PlayAnimationForward(WAM_Appear);

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				this->RemoveFromParent();
			}, 3.f, false);
	}
}
