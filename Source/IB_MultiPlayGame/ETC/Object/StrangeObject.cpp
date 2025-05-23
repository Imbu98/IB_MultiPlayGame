#include "StrangeObject.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"


AStrangeObject::AStrangeObject()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(SceneRoot);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(GetRootComponent());

	OverlapBoxComponent = CreateDefaultSubobject<UBoxComponent>("OverlapBoxComponent");
	OverlapBoxComponent->SetupAttachment(GetRootComponent());

	OverlapBoxComponent->OnComponentBeginOverlap.Clear();
	OverlapBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);

	OverlapBoxComponent->OnComponentEndOverlap.Clear();
	OverlapBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);


}

void AStrangeObject::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStrangeObject, InteractedActor);
}

void AStrangeObject::BeginPlay()
{
	Super::BeginPlay();
	
}

FString AStrangeObject::InteractWith_Implementation(APlayerController* PlayerController)
{

	return FString::Printf(TEXT("StrangeObject"));
}

void AStrangeObject::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		if (OtherActor->Implements<UInteractInterface>())
		{
			IInteractInterface::Execute_SetNPCActor(OtherActor, this);
		}
		if (WBP_Interact)
		{
			WBP_Interact->SetVisibility(true);
		}
		if (InteractOverlayMaterial&& StaticMeshComponent)
		{
			StaticMeshComponent->SetOverlayMaterial(InteractOverlayMaterial);
		}
	}
	if (!HasAuthority())
	{
		return;
	}
	InteractedActor = OtherActor;

}

void AStrangeObject::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		if (OtherActor->Implements<UInteractInterface>())
		{
			IInteractInterface::Execute_SetNPCActor(OtherActor, nullptr);
		}
		if (WBP_Interact)
		{
			WBP_Interact->SetVisibility(false);
		}
		
		if (StaticMeshComponent)
		{
			StaticMeshComponent->SetOverlayMaterial(nullptr);
		}
			
		
	}
	if (!HasAuthority())
	{
		return;
	}
	InteractedActor = nullptr;

}


