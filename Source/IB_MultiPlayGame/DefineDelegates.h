#pragma once


#include "Inventory/ItemTypes.h"
#include "Components/QuestComponent.h"
#include "CoreMinimal.h"

class UQuestComponent;
class AIB_RPGPlayerController;
struct FActiveQuestData;


// ItemUseButton Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnClickedActionButton,const FMasterItemDefinition&);
// QuestSystem Delegate
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnObjectiveIdCalled, FString,int32);
// QuestLogEntryButton Delegate

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnQuestSelected, FName, const FActiveQuestData&);
// QuestTrackButton Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestTrack, const FActiveQuestData&);
// QuestComplete Delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestCompletedDeleteTrack, const FActiveQuestData&);
//
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestCompletedDeleteLogEntry, const FActiveQuestData&);



