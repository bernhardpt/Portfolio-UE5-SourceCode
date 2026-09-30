#pragma once

#include "CoreMinimal.h"
#include "BasePickup.h"
#include "BaseWeapon.h"
#include "PickupAmmo.generated.h"

UCLASS()
class ZOMBIESHOOTER_API APickupAmmo : public ABasePickup
{
	GENERATED_BODY()

public:
	// Amount of ammunition granted to the player's reserve
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Pickup")
	int32 AmmoAmount = 30;

	// Defines which specific weapon type this pickup refills (e.g., Rifle, Pistol)
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Pickup")
	TSubclassOf<ABaseWeapon> WeaponClassToRefill;

	// Executes the ammo refill logic and updates item state
	virtual void OnPickup(APlayerCharacter* Player) override;
};