#include "W_QuestLogEntry_Objectives.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"
#include "../QuestSystem/Quest_Base.h"
#include "../Components/QuestComponent.h"

void UW_QuestLogEntry_Objectives::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(QuestActor) && !ActiveQuestData.QuestID.IsNone())
	{
		bool bFound = false;
		for (const FActiveQuestData& Quest : QuestActor->CurrentQuests)
		{
			if (const int32* Current = Quest.CurrentStageObjectiveProgress.Find(ObjectiveData.ObjectiveID))
			{
				int32 Progress = *Current;

				if (TextBlock_Description)
				{
					FText Description = FText::FromString(FString::Printf(TEXT("%s %d/%d"), *ObjectiveData.Description.ToString(), Progress, ObjectiveData.Quantity));
					TextBlock_Description->SetText(Description);
				}

				if (CheckBox_IsCompleted)
				{
					ECheckBoxState CheckState = (Progress >= ObjectiveData.Quantity) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
					CheckBox_IsCompleted->SetCheckedState(CheckState);
				}
				bFound = true;
				break; // ��ġ�ϴ� Objective ã������ ����
			}
		}

		if (!bFound)
		{
			// �� ã���� �� ó��
			if (TextBlock_Description)
			{
				FText Description = FText::FromString(FString::Printf(TEXT("%s 0/%d"), *ObjectiveData.Description.ToString(), ObjectiveData.Quantity));
				TextBlock_Description->SetText(Description);
			}
			if (CheckBox_IsCompleted)
			{
				CheckBox_IsCompleted->SetCheckedState(ECheckBoxState::Unchecked);
			}
		}
	}
	else
	{
		// QuestActor�� ���ų� QuestID�� ���� �� ó��
		if (TextBlock_Description)
		{
			FText Description = FText::FromString(FString::Printf(TEXT("%s 0/%d"), *ObjectiveData.Description.ToString(), ObjectiveData.Quantity));
			TextBlock_Description->SetText(Description);
		}
		if (CheckBox_IsCompleted)
		{
			CheckBox_IsCompleted->SetCheckedState(ECheckBoxState::Unchecked);
		}
	}
}
		
