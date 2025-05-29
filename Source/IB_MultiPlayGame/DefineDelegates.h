#pragma once


#include "CoreMinimal.h"
#include "Inventory/ItemTypes.h"

class UQuestComponent;



// ItemUseButton Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnClickedActionButton,const FMasterItemDefinition&);
// QuestSystem Delegate
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnObjectiveIdCalled, FString,int32);
// QuestLogEntryButton Delegate
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnQuestSelected, FName,UQuestComponent*);
// QuestTrackButton Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestTrack,UQuestComponent*);
// QuestComplete Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, UQuestComponent*);

