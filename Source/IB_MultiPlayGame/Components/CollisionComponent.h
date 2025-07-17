#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CollisionComponent.generated.h"

class UMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitDelegate, FHitResult, HitResult);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IB_MULTIPLAYGAME_API UCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCollisionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	static TSet<AActor*> GlobalAlreadyHitActors;

	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;
	UPROPERTY()
	AActor* HitActor;
	UPROPERTY()
	UPrimitiveComponent* CollisionMeshComponent;
	bool IsCollisionEnabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	FName StartSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Defaults)
	FName EndSocketName;
	float SphereRadius = 10.f;

	TEnumAsByte<EObjectTypeQuery> ObjectType;
	FHitResult LastHit;

	FOnHitDelegate Onhit;

public:
	void EnableCollision();
	void DisableCollision();
	void ClearHitActors();
	void SetCollisionMesh(UPrimitiveComponent* MeshComponent);
	void CollisionTrace();
	void AddActorsToIgnore(AActor* InActors);

	
};
