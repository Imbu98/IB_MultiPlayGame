#include "W_LogInWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"


void UW_LogInWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BTN_SignUpMenu)
	{
		BTN_SignUpMenu->OnClicked.Clear();
		BTN_SignUpMenu->OnClicked.AddDynamic(this,&ThisClass::OnClickedSignUpMenuButton);
	}
	if (BTN_SignInMenu)
	{
		BTN_SignInMenu->OnClicked.Clear();
		BTN_SignInMenu->OnClicked.AddDynamic(this,&ThisClass::OnClickedSignInMenuButton);
	}
	if (Btn_SignInReturnMainMenu)
	{
		Btn_SignInReturnMainMenu->OnClicked.Clear();
		Btn_SignInReturnMainMenu->OnClicked.AddDynamic(this,&ThisClass::OnClickedReturnMainMenuButton);
	}
	if (Btn_SignUpReturnMainMenu)
	{
		Btn_SignUpReturnMainMenu->OnClicked.Clear();
		Btn_SignUpReturnMainMenu->OnClicked.AddDynamic(this,&ThisClass::OnClickedReturnMainMenuButton);
	}
	if (Btn_SignUpComplete)
	{
		Btn_SignUpComplete->OnClicked.Clear();
		Btn_SignUpComplete->OnClicked.AddDynamic(this,&ThisClass::OnClickedSignUpCompleteButton);
	}
	if (Btn_SignInComplete)
	{
		Btn_SignInComplete->OnClicked.Clear();
		Btn_SignInComplete->OnClicked.AddDynamic(this,&ThisClass::OnClickedSignInCompleteButton);
	}
	if (BTN_ExitGame)
	{
		BTN_ExitGame->OnClicked.Clear();
		BTN_ExitGame->OnClicked.AddDynamic(this,&ThisClass::OnClickedExitGameButton);
	}
	
}

void UW_LogInWidget::OnClickedSignUpMenuButton()
{
	if (WidgetSwitcher_Switcher)
	{
		WidgetSwitcher_Switcher->SetActiveWidgetIndex(1);
	}
}

void UW_LogInWidget::OnClickedSignInMenuButton()
{
	if (WidgetSwitcher_Switcher)
	{
		WidgetSwitcher_Switcher->SetActiveWidgetIndex(2);
	}
}

void UW_LogInWidget::OnClickedReturnMainMenuButton()
{
	if (WidgetSwitcher_Switcher)
	{
		WidgetSwitcher_Switcher->SetActiveWidgetIndex(0);
	}
}

void UW_LogInWidget::OnClickedExitGameButton()
{
	// 게임 종료
}

void UW_LogInWidget::OnClickedSignUpCompleteButton()
{
	//데이터베이스에 정보들 올리기 (회원가입)
	if (EditableTextBox_SignUpId)
	{
		
	}
	if (EditableTextBox_SignUpPassword)
	{
		
	}
	if (EditableTextBox_SignUpPlayerName)
	{
		
	}
}

void UW_LogInWidget::OnClickedSignInCompleteButton()
{
	//정보들로 로그인 하기
	if (EditableTextBox_SignInId)
	{
		
	}
	if (EditableTextBox_SignInPassword)
	{
		
	}
}

