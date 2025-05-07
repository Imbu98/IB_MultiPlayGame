//#include "W_ItemRow.h"
//#include "Components/Button.h"
//#include "Components/TextBlock.h"
//#include "GameplayTagContainer.h"
//
//void UW_ItemRow::NativeConstruct()
//{
//	if (BTN_ActionButton)
//	{
//		BTN_ActionButton->OnClicked.Clear();
//		BTN_ActionButton->OnClicked.AddDynamic(this, &UW_ItemRow::OnclickedActionButton);
//	}
//}
//
//void UW_ItemRow::SetActionText(const FGameplayTag& CategoryTag)
//{
//	if (TXT_ActionText)
//	{
//		if (FilterCategoryTag(CategoryTag) == EItemTypes::Consumable)
//		{
//			TXT_ActionText->SetText(FText::FromString(TEXT("Use")));
//		}
//	}
//}
//
//void UW_ItemRow::SetItemNameText(FText ItemName)
//{
//	if (TXT_ItemNameText)
//	{
//		TXT_ItemNameText->SetText(ItemName);
//	}
//}
//
//void UW_ItemRow::SetQuiantityText(int32 Quantity)
//{
//	if (TXT_QuantityText)
//	{
//		TXT_QuantityText->SetText(FText::FromString(FString::Printf(TEXT("x %d"), Quantity)));
//	}
//}
//
//void UW_ItemRow::OnclickedActionButton()
//{
//	OnClickedActionButtonDelegate.Broadcast(Item);
//}
//
//EItemTypes UW_ItemRow::FilterCategoryTag(const FGameplayTag& Tag)
//{
//	FGameplayTag ConsumableTag = FGameplayTag::RequestGameplayTag(FName("Item.Consumable"));
//	if (Tag.MatchesTag(ConsumableTag))
//	{
//		return EItemTypes::Consumable;
//	}
//	return EItemTypes::None;
//}
//
