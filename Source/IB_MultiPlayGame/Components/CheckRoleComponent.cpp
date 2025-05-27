#include "CheckRoleComponent.h"


UCheckRoleComponent::UCheckRoleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	
}


void UCheckRoleComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UCheckRoleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	AActor* Owner = GetOwner();
	if (!Owner) return;

	ENetMode NetMode = Owner->GetNetMode();
	FString NetModeStr;
	switch (NetMode)
	{
	case NM_Client: NetModeStr = TEXT("Client"); break;
	case NM_ListenServer: NetModeStr = TEXT("ListenServer"); break;
	case NM_DedicatedServer: NetModeStr = TEXT("DedicatedServer"); break;
	case NM_Standalone: NetModeStr = TEXT("Standalone"); break;
	default: NetModeStr = TEXT("Unknown"); break;
	}

	FString DebugText = FString::Printf(TEXT("Authority: %s\nNetMode: %s\nRemoteRole: %s\nLocalRole: %s"),
		Owner->HasAuthority() ? TEXT("TRUE") : TEXT("FALSE"),
		*NetModeStr,
		*UEnum::GetValueAsString(Owner->GetRemoteRole()),
		*UEnum::GetValueAsString(Owner->GetLocalRole())
	);

	if (GetOwner()->GetNetMode() == NM_DedicatedServer)
	{
		FVector Location = Owner->GetActorLocation() + FVector(0, 0, -100.f); // ¸ö Áß¾Ó À§
		DrawDebugString(GetWorld(), Location, DebugText, nullptr, FColor::Cyan, 0.f, true);
	}
	else if(GetOwner()->GetNetMode() == NM_Client)
	{
		FVector Location = Owner->GetActorLocation() + FVector(0, 0, 100.f); // ¸ö Áß¾Ó À§
		DrawDebugString(GetWorld(), Location, DebugText, nullptr, FColor::Cyan, 0.f, true);
	}

	
	
}

