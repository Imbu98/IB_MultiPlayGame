#include "EnemyBaseChar.h"
#include "../IB_Framework/IB_GAS/IB_RPGAbilitySystemComponent.h"
#include "../IB_Framework/IB_GAS/IB_RPGAttributeSet.h"
#include "../IB_Framework/IB_GAS/Data/IB_CharacterClassInfo.h"
#include "../IB_Framework/FunctionLibrary/IB_BlueprintFunctionLibrary.h"
#include "../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "Components/WidgetComponent.h"
#include "../Widget/W_EnemyOverHeadBars.h"
#include "IB_MainChar.h"

AEnemyBaseChar::AEnemyBaseChar()
{
	bReplicates = true;

	IB_RPGAbilitySystemComp = CreateDefaultSubobject<UIB_RPGAbilitySystemComponent>("IB_RPGAbilitySystemComponent");
	IB_RPGAbilitySystemComp->SetIsReplicated(true);
	IB_RPGAbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	IB_RPGAttributeSet = CreateDefaultSubobject<UIB_RPGAttributeSet>("IB_RPGAttributeSet");

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>("WidgetOverHeadBars");
	WidgetComponent->SetupAttachment(GetMesh());
}

UAbilitySystemComponent* AEnemyBaseChar::GetAbilitySystemComponent() const
{
	return IB_RPGAbilitySystemComp;

}

void AEnemyBaseChar::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();

	InitEnemyOverHeadBars();
	
}

void AEnemyBaseChar::InitAbilityActorInfo()
{
	if (IsValid(IB_RPGAbilitySystemComp) && IsValid(IB_RPGAttributeSet))
	{
		IB_RPGAbilitySystemComp->InitAbilityActorInfo(this, this);
		BindCallbacksToDependencies();

		if (HasAuthority())
		{
			InitClassDefaults();
		}
	}
}

void AEnemyBaseChar::InitClassDefaults()
{
	if (!CharacterTag.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("No Character Tag Selected In This Character : %s"), *GetNameSafe(this)));
	}
	else if(UIB_CharacterClassInfo* ClassInfo = UIB_BlueprintFunctionLibrary::GetCharacterClassDefaultInfo(this))
	{
		if (const FIB_CharacterClassDefaultInfo* SelectedClass = ClassInfo->ClassDefaultInfoMap.Find(CharacterTag))
		{
			if (IsValid(IB_RPGAbilitySystemComp))
			{
				IB_RPGAbilitySystemComp->AddCharacterAbilities(SelectedClass->StartingAbilities);
				IB_RPGAbilitySystemComp->AddCharacterPassiveAbilities(SelectedClass->StartingPassives);
				IB_RPGAbilitySystemComp->InitializeDefaultAttributes(SelectedClass->DefaultAttributes);
			}
		}
	}
}

void AEnemyBaseChar::BindCallbacksToDependencies()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("EnemyBase : BindCallbacksToDependencies")));
	if (IsValid(IB_RPGAbilitySystemComp) && IsValid(IB_RPGAttributeSet))
	{
		IB_RPGAbilitySystemComp->GetGameplayAttributeValueChangeDelegate(IB_RPGAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged(Data.NewValue, IB_RPGAttributeSet->GetMaxHealth());
			});
	}
}

void AEnemyBaseChar::OnHealthChanged(float CurrentHealth, float MaxHealth)
{
	
	if (OverHeadBars)
	{
		OverHeadBars->SetProgressBarPercent(CurrentHealth, MaxHealth);
	}
	
	if (CurrentHealth <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Current Health Is Zero"));
		// 클라기준 PlayerController가 IsLocalController()가 true면 클라니까 
		// DamageInstigator->IsLocalController()로 빌드해보자
		if (DamageInstigator&&!DamageInstigator->IsLocalController())
		{
			ServerHandleEnemyDeath(DamageInstigator);
		}
		
	}
}

void AEnemyBaseChar::ServerHandleEnemyDeath_Implementation(AIB_RPGPlayerController* IB_RPGPlayerController)
{
	if (IB_RPGPlayerController)
	{
		if (AIB_MainChar* IB_MainChar = Cast<AIB_MainChar>(IB_RPGPlayerController->GetPawn()))
		{
			if (IB_MainChar->OnObjectiveIdCalledDelegate.IsBound())
			{
				IB_MainChar->OnObjectiveIdCalledDelegate.Broadcast(ObjectiveID);
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Destroyer:%s,ObjectiveID:%s"), *IB_RPGPlayerController->GetName(), *ObjectiveID));
				Destroy();
			}

		}
	}
}

void AEnemyBaseChar::SetDamageInstigator_Implementation(AIB_RPGPlayerController* IB_RPGPlayerController)
{
	// Overlap이벤트는 Server에서 처리하므로 여기는 Server 실행
	if (!IsValid(IB_RPGPlayerController)||IB_RPGPlayerController->IsLocalController())
	{
		return;
	}
		DamageInstigator = IB_RPGPlayerController;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("DamageInstigator:%s"), *DamageInstigator->GetName()));
		
}

void AEnemyBaseChar::InitEnemyOverHeadBars()
{
	if (WBP_OverHeadBarsclass)
	{

		if (UUserWidget* Widget = CreateWidget<UW_EnemyOverHeadBars>(GetWorld(), WBP_OverHeadBarsclass))
		{

			OverHeadBars = Cast<UW_EnemyOverHeadBars>(Widget);
			if (OverHeadBars)
			{
				WidgetComponent->SetWidget(OverHeadBars);
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
					{

						OverHeadBars->SetProgressBarPercent(IB_RPGAttributeSet->GetHealth(), IB_RPGAttributeSet->GetMaxHealth());

					}, 0.5f, false);
			}

		}

	}
}
