#include "ABaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AABaseProjectile::AABaseProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setup spherical collision and bind the Hit event
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f); 
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile"); 
	CollisionComp->OnComponentHit.AddDynamic(this, &AABaseProjectile::OnHit); 
	RootComponent = CollisionComp;
	
	// Setup visual mesh (collision is handled by SphereComp)
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(CollisionComp);
	ProjectileMesh->SetCollisionProfileName("NoCollision");
	
	// Configure projectile movement logic (no gravity, no bouncing)
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 0.f;
	ProjectileMovement->MaxSpeed = 0.f; 
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	
	// Destroy the projectile after 3 seconds to prevent memory leaks
	InitialLifeSpan = 3.0f;
}

void AABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AABaseProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Ensure we don't damage ourselves or the actor that fired this projectile
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherActor != GetOwner()))
	{
		UGameplayStatics::ApplyDamage(OtherActor, DamageValue, GetInstigatorController(), this, UDamageType::StaticClass()); 
		
		// Destroy projectile on successful impact
		Destroy();
	}
}