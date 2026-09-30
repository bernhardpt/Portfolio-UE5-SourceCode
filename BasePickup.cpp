#include "BasePickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "PlayerCharacter.h"

ABasePickup::ABasePickup()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setup collision sphere for overlap detection
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(50.0f);
	SphereComp->SetCollisionProfileName("Trigger");
	RootComponent = SphereComp;

	// Setup static mesh (visual only, collision handled by SphereComp)
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Add a gentle rotation effect (90 degrees per second on the Yaw axis)
	RotatingComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComp"));
	RotatingComp->RotationRate = FRotator(0.0f, 90.0f, 0.0f);
}

void ABasePickup::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABasePickup::OnPickup(class APlayerCharacter* Player)
{
	// Base implementation left intentionally empty. 
	// Child classes will override this to apply specific gameplay effects.
}

void ABasePickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// Ignore overlaps if the pickup is already collected or inactive
	if (!bIsActive) return;
	
	Super::NotifyActorBeginOverlap(OtherActor);

	// Check if the overlapping actor is a valid player character
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
	{
		OnPickup(Player); 
	}
}

void ABasePickup::ResetPickup()
{
	// Re-enable the pickup in the game world
	bIsActive = true;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}