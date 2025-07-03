#include "IB_LoginGameMode.h"

#include "../../Widget/W_LogInWidget.h"

#include "Blueprint/UserWidget.h"
#include "IB_MultiPlayGame/IB_Framework/IB_GAS/IB_RPGPlayerController.h"


void AIB_LoginGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AIB_RPGPlayerController* MyPC = Cast<AIB_RPGPlayerController>(NewPlayer);
	if (MyPC)
	{
		MyPC->ClientCreateLoginWidget();
	}
}
