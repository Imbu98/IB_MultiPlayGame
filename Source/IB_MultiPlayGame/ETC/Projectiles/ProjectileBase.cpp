#include "ProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "../../IB_Framework/IB_GAS/AbilitySystem/IB_RPGAbilityTypes.h"
#include "../../IB_Framework/IB_GAS/IB_RPGPlayerController.h"
#include "AbilitySystemGlobals.h"
#include "../../IB_Framework/FunctionLibrary/IB_BlueprintFunctionLibrary.h"
#include "../../Character/IB_MainChar.h"



AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	SetRootComponent(ProjectileMesh);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProjectileMesh->SetCollisionObjectType(ECC_WorldDynamic);
	ProjectileMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileMesh->SetIsReplicated(true);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComponent");

	OverlapSphere = CreateDefaultSubobject<USphereComponent>("OverlapSphere"); 
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapSphere->SetupAttachment(GetRootComponent());


}

void AProjectileBase::SetProjectileParams(const FProjectileParams& Params)
{
	if (IsValid(ProjectileMesh))
	{
		ProjectileMesh->SetStaticMesh(Params.ProjectileMesh);
	}
	if (IsValid(ProjectileMovementComponent))
	{
		ProjectileMovementComponent->InitialSpeed = Params.InitialSpeed;
		ProjectileMovementComponent->ProjectileGravityScale = Params.GravityScale;
		ProjectileMovementComponent->bShouldBounce = Params.bShouldBounce;
		ProjectileMovementComponent->Bounciness = Params.Bounciness;
	}
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnSphereBeginOverlap);
	}
	
}

void AProjectileBase::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
	{
		return;
	}
	// Overlap된 Actor의 ASC를 찾아 TargetASC로 설정하고, TaragetASC에 DamageApply
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor))
	{

		if (OtherActor->Implements<UInteractInterface>())
		{
			if (AIB_MainChar* PlayerCharacter = Cast<AIB_MainChar>(GetOwner()))
			{
				if (AIB_RPGPlayerController* IBRPGPlayerController = Cast<AIB_RPGPlayerController>(PlayerCharacter->GetController()))
				{
					IInteractInterface::Execute_SetDamageInstigator(OtherActor, IBRPGPlayerController);
					DamageEffectInfo.TargetASC = TargetASC;
					UIB_BlueprintFunctionLibrary::ApplyDamageEffect(DamageEffectInfo);
				}
			}
			
		}

		Destroy();
	}
}

