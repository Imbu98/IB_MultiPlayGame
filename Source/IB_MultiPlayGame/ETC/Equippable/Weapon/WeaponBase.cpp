#include "WeaponBase.h"
#include "Components\SkeletalMeshComponent.h"


AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("RootSceneComponent");
	SetRootComponent(RootSceneComponent);

	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponSkeletalMesh");
	WeaponSkeletalMesh->SetupAttachment(GetRootComponent());
	WeaponSkeletalMesh->SetIsReplicated(true);

}

void AWeaponBase::SetWeaponParams(const FWeaponParams& Params)
{
	if (IsValid(WeaponSkeletalMesh))
	{
		WeaponSkeletalMesh->SetSkeletalMesh(Params.WeaponSkeletalMesh);

		WeaponAttackMontageArray.SetNum(Params.WeaponAttackMontageArray.Num());
		WeaponAttackMontageArray = Params.WeaponAttackMontageArray;
	}

	
}


void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

