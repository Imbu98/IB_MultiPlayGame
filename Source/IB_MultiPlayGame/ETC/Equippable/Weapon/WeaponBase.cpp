#include "WeaponBase.h"
#include "Components\SkeletalMeshComponent.h"


AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponSkeletalMesh");
	SetRootComponent(WeaponSkeletalMesh);
	WeaponSkeletalMesh->SetIsReplicated(true);

}

void AWeaponBase::SetWeaponParams(const FWeaponParams& Params)
{
	if (HasAuthority())
	{
		if (IsValid(WeaponSkeletalMesh))
		{
			UE_LOG(LogTemp, Log, TEXT("[%s] WeaponSkeletalMesh is valid before setting mesh."), HasAuthority() ? TEXT("Server") : TEXT("Client"));

			WeaponSkeletalMesh->SetSkeletalMesh(Params.WeaponSkeletalMesh);

			UE_LOG(LogTemp, Log, TEXT("[%s] SetSkeletalMesh called with mesh: %s"),
				HasAuthority() ? TEXT("Server") : TEXT("Client"),
				*GetNameSafe(Params.WeaponSkeletalMesh));

			/* Rarity에 따른 데미지나누기
			if(Rarity == common)
			{
				DamageEffectInfo.BaseDamage = ?.?f
			}*/

			WeaponAttackMontageArray.SetNum(Params.WeaponAttackMontageArray.Num());
			WeaponAttackMontageArray = Params.WeaponAttackMontageArray;

			MulticastSetWeaponMesh(WeaponSkeletalMesh->GetSkeletalMeshAsset());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] WeaponSkeletalMesh is NOT valid!"), HasAuthority() ? TEXT("Server") : TEXT("Client"));
		}
	}
}


void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("WeaponBase spawned on: %s"), GetWorld()->IsNetMode(NM_Client) ? TEXT("Client") : TEXT("Server"));
}

void AWeaponBase::MulticastSetWeaponMesh_Implementation(USkeletalMesh* InMesh)
{
	if (IsValid(InMesh))
	{
		if (IsValid(WeaponSkeletalMesh))
		{
			UE_LOG(LogTemp, Log, TEXT("[%s] WeaponSkeletalMesh is valid before setting mesh."), HasAuthority() ? TEXT("Server") : TEXT("Client"));

			WeaponSkeletalMesh->SetSkeletalMesh(InMesh);

			UE_LOG(LogTemp, Log, TEXT("[%s] SetSkeletalMesh called with mesh: %s"),
				HasAuthority() ? TEXT("Server") : TEXT("Client"),
				*GetNameSafe(InMesh));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] WeaponSkeletalMesh is NOT valid!"), HasAuthority() ? TEXT("Server") : TEXT("Client"));
		}
	}
}

