#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "BasePickup.generated.h"

class USphereComponent;
class URotatingMovementComponent;
class APlayerCharacter;

UCLASS()
class ZOMBIESHOOTER_API ABasePickup : public AActor
{
	GENERATED_BODY()
	
public:	
	ABasePickup();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Visual representation of the pickup item (collision disabled)
	UPROPERTY(VisibleAnywhere, Category = "C++ | Components")
	UStaticMeshComponent* MeshComp;

	// Overlap volume to detect player interaction
	UPROPERTY(VisibleAnywhere, Category = "C++ | Components")
	USphereComponent* SphereComp;

	// Adds a continuous rotation effect for better visibility
	UPROPERTY(VisibleAnywhere, Category = "C++ | Components")
	URotatingMovementComponent* RotatingComp;

	// Base logic for pickup event. Intended to be overridden by child classes (e.g., Ammo, Health)
	virtual void OnPickup(class APlayerCharacter* Player);

	// Triggered when an actor enters the SphereComp radius
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	// Restores the pickup to its original state for respawning mechanisms
	void ResetPickup();

	// Tracks if the item is currently available in the world
	bool bIsActive = true;

	// Audio effect played upon successful collection
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Audio")
	USoundBase* PickupSound;
};