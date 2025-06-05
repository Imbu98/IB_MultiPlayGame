#include "BaseSpawnedItem.h"
#include "../../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "../../Components/InventoryComponent.h"
#include "SpawnedItemSetProperty.h"
#include "../../Interfaces/InventoryInterface.h"
#include "../../Character/IB_MainChar.h"

#include "Components/CapsuleComponent.h"
#include "Components\StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"


ABaseSpawnedItem::ABaseSpawnedItem()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	SetNetUpdateFrequency(100.0f);
	SetMinNetUpdateFrequency(66.f);

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMesh"));
	SetRootComponent(ItemStaticMesh);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetupAttachment(ItemStaticMesh);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(ItemStaticMesh);


	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);

}
void ABaseSpawnedItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// on server
FString ABaseSpawnedItem::InteractWith_Implementation(APlayerController* PlayerController)
{
	if (PlayerController == GetOwner())
	{
		if (AIB_RPGPlayerController* IB_RPGPlayerController = Cast<AIB_RPGPlayerController>(PlayerController))
		{
			if (IB_RPGPlayerController->Implements<UInventoryInterface>())
			{
				if (UInventoryComponent* InventoryComponent = IInventoryInterface::Execute_GetInventoryComponent(IB_RPGPlayerController))
				{
					InventoryComponent->AddItem(ItemDefinition.ItemTag, 1);
					Destroy();
				}
			}
		}
		return FString();
	}
	return FString();
}

void ABaseSpawnedItem::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			if (OtherActor->Implements<UInteractInterface>())
			{
				IInteractInterface::Execute_SetNPCActor(OtherActor, this);
			}
		}
		if (ItemOverlayMaterial&& ItemStaticMesh)
		{
			ItemStaticMesh->SetOverlayMaterial(ItemOverlayMaterial);
		}
		if (WidgetComponent)
		{
			WidgetComponent->SetVisibility(true);
		}
	}
}

void ABaseSpawnedItem::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{

		if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			if (OtherActor->Implements<UInteractInterface>())
			{
				IInteractInterface::Execute_SetNPCActor(OtherActor, nullptr);
			}
		}
		if (ItemStaticMesh)
		{
			ItemStaticMesh->SetOverlayMaterial(nullptr);
		}


		if (WidgetComponent)
		{
			WidgetComponent->SetVisibility(false);
		}
	}
}

// on server
void ABaseSpawnedItem::InitializeWithTag(FGameplayTag InTag, EItemRarity ItemRarity)
{
	ItemDefinition.ItemRarity = ItemRarity;
	ItemDefinition.ItemTag = InTag;
	SetMeshFromTag(InTag);
	
}


void ABaseSpawnedItem::SetMeshFromTag(FGameplayTag InItemTag)
{
	if (IsValid(DT_ItemData))
	{
		static const FString ContextString(TEXT("ItemMeshLookup"));
		TArray<FItemProperty*> AllRows;
		DT_ItemData->GetAllRows(ContextString, AllRows);
		for (const FItemProperty* Row : AllRows)
		{
			if (Row->ItemTag == InItemTag)
			{
				if (ItemStaticMesh && Row->Mesh)
				{
					ItemStaticMesh->SetStaticMesh(Row->Mesh);
					MulticastSetMeshFromTag(InItemTag);
					return;
				}
			}
		}
	}
	else
	{
		
	}
}

void ABaseSpawnedItem::MulticastSetMeshFromTag_Implementation(FGameplayTag InItemTag)
{
	if (HasAuthority()) return;

	if (IsValid(DT_ItemData))
	{
		static const FString ContextString(TEXT("ItemMeshLookup"));
		TArray<FItemProperty*> AllRows;
		DT_ItemData->GetAllRows(ContextString, AllRows);
		for (const FItemProperty* Row : AllRows)
		{
			if (Row->ItemTag == InItemTag)
			{
				if (ItemStaticMesh && Row->Mesh)
				{
					ItemStaticMesh->SetStaticMesh(Row->Mesh);
					return;
				}
			}
		}
	}
	else
	{

	}
}
