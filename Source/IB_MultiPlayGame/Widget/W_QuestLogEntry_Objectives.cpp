#include "W_QuestLogEntry_Objectives.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"
#include "../QuestSystem/Quest_Base.h"
#include "../Components/QuestComponent.h"

void UW_QuestLogEntry_Objectives::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(QuestActor)&& !ActiveQuestData.QuestID.IsNone())
	{
		for (int i = 0; i < QuestActor->CurrentQuests.Num();i++)
		{
			if (int32* Current = QuestActor->CurrentQuests[i].CurrentStageObjectiveProgress.Find(ObjectiveData.ObjectiveID))
			{
				if (TextBlock_Description)
				{
					FText Description = FText::FromString(FString::Printf(TEXT("%s %d/%d"), *ObjectiveData.Description.ToString(), *Current, ObjectiveData.Quantity));
					TextBlock_Description->SetText(Description);
				}
				if (CheckBox_IsCompleted)
				{
					if (ObjectiveData.Quantity <= *Current)
					{
						CheckBox_IsCompleted->SetCheckedState(ECheckBoxState::Checked);
					}

					else
					{
						CheckBox_IsCompleted->SetCheckedState(ECheckBoxState::Unchecked);
					}
				}

			}
			else
			{
				if (TextBlock_Description)
				{
					FText Description = FText::FromString(FString::Printf(TEXT("%s 0/%d"), *ObjectiveData.Description.ToString(), ObjectiveData.Quantity));
					TextBlock_Description->SetText(Description);
				}
				if (CheckBox_IsCompleted)
				{

				}
			}
			break;
		}
	}
	
	else
	{
		if (TextBlock_Description)
		{
			FText Description = FText::FromString(FString::Printf(TEXT("%s 0/%d"), *ObjectiveData.Description.ToString(), ObjectiveData.Quantity));
			TextBlock_Description->SetText(Description);
		}
		if (CheckBox_IsCompleted)
		{

		}
	}
}
		
