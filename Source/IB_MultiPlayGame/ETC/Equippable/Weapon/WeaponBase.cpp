#include "WeaponBase.h"
#include "Components\SkeletalMeshComponent.h"
#include "../../../Components/CollisionComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"


AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponSkeletalMesh");
	SetRootComponent(WeaponSkeletalMesh);
	WeaponSkeletalMesh->SetIsReplicated(true);

	CollisionComponent = CreateDefaultSubobject<UCollisionComponent>("CollisionComponent");
	CollisionComponent->SetIsReplicated(true);

	CollisionComponent->Onhit.AddDynamic(this, &ThisClass::OnHitActor);

}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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

			/* Rarity�� ���� ������������
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

UPrimitiveComponent* AWeaponBase::GetItemMesh()
{

	if (WeaponSkeletalMesh != nullptr)
	{
		return WeaponSkeletalMesh;
	}

	return nullptr;
}

void AWeaponBase::OnHitActor(FHitResult HitResult)
{
	if (!HasAuthority()) return;


}

UCollisionComponent* AWeaponBase::GetWeaponCollisionComponent()
{
	if (CollisionComponent)
	{
		return CollisionComponent;
	}
	return nullptr;
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

void AWeaponBase::SetCharacterAttack(float AttackPower,const uint8& DetermineIndex)
{
	if (CollisionComponent != nullptr)
	{
		CollisionComponent->SetCollisionMesh(GetItemMesh());
		CollisionComponent->AddActorsToIgnore(this->GetOwner());
	}

	if (DetermineIndex == 0)
	{
		WeaponAttackPower = AttackPower;

		if (!IsValid(WeaponAttackEffect)) return;

		if (AActor* OwnerCharacter = GetOwner())
		{
			if (UAbilitySystemComponent* OwnerAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter))
			{
				FGameplayEffectSpecHandle SpecHandle = OwnerAsc->MakeOutgoingSpec(WeaponAttackEffect, 1.f, OwnerAsc->MakeEffectContext());
				if (SpecHandle.IsValid())
				{
					SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Combat.Data.Attack")), WeaponAttackPower);
					ActiveGEHandle = OwnerAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}
	}
}
