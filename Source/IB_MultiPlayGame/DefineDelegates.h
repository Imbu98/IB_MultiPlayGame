#pragma once


#include "Inventory/ItemTypes.h"
#include "Components/QuestComponent.h"
#include "CoreMinimal.h"

class UQuestComponent;
class AIB_RPGPlayerController;


// ItemUseButton Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnClickedActionButton,const FMasterItemDefinition&);
// QuestSystem Delegate
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnObjectiveIdCalled, FString,int32);
// QuestLogEntryButton Delegate
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnQuestSelected, FName,UQuestComponent*);
// QuestTrackButton Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestTrack, UQuestComponent*);
// QuestComplete Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestCompletedDeleteTrack, UQuestComponent*);
//
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestCompletedDeleteLogEntry, UQuestComponent*);

