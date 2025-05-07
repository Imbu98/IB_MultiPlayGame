#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "RPGInputConfig.h"
#include "RPGSystemsInputComponents.generated.h"

struct FRPGInputAction;
class URPGInputConfig;

UCLASS()
class IB_MULTIPLAYGAME_API URPGSystemsInputComponents : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	template<class UserClass,typename PressedFuncType,typename ReleasedFuncType>
	void BindAbililtyActions(URPGInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);
	
	
};
 
template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void URPGSystemsInputComponents::BindAbililtyActions(URPGInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
	check(InputConfig);

	for (const FRPGInputAction& Action : InputConfig->RPGInputActions)
	{
		if (IsValid(Action.InputAction) && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
		}
	}
}
