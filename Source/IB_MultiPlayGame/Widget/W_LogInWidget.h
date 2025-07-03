#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_LogInWidget.generated.h"

class UEditableTextBox;
class UWidgetSwitcher;
class UButton;

UCLASS()
class IB_MULTIPLAYGAME_API UW_LogInWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UButton> BTN_SignUpMenu;
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UButton> BTN_SignInMenu;
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UButton> BTN_ExitGame;
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UButton> Btn_SignUpReturnMainMenu;
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UButton> Btn_SignInReturnMainMenu;
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UButton> Btn_SignUpComplete;
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UButton> Btn_SignInComplete;
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher_Switcher;
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_SignUpId;
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_SignUpPassword;
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_SignUpPlayerName;
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_SignInId;
	UPROPERTY(EditAnywhere,meta=(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_SignInPassword;
	

protected:
	UFUNCTION()
	void OnClickedSignUpMenuButton();
	UFUNCTION()
	void OnClickedSignInMenuButton();
	UFUNCTION()
	void OnClickedExitGameButton();
	UFUNCTION()
	void OnClickedSignUpCompleteButton();
	UFUNCTION()
	void OnClickedSignInCompleteButton();
	UFUNCTION()
	void OnClickedReturnMainMenuButton();
};
