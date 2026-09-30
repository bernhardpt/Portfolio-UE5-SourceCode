#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ABaseProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ZOMBIESHOOTER_API AABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	AABaseProjectile();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	// Root collision component for hit detection
	UPROPERTY(VisibleDefaultsOnly, Category = "C++ | Projectile")
	USphereComponent* CollisionComp;

	// Handles projectile speed, direction, and physics behavior
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Movement")
	UProjectileMovementComponent* ProjectileMovement;

	// Visual representation of the projectile
	UPROPERTY(VisibleAnywhere, Category = "C++ | Projectile")
	UStaticMeshComponent* ProjectileMesh;

	// Base damage applied to hit targets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++ | Combat")
	float DamageValue = 10.0f;

	// Called automatically upon collision to handle damage application
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};