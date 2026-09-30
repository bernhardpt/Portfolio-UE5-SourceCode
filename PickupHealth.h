#pragma once

#include "CoreMinimal.h"
#include "BasePickup.h"
#include "PickupHealth.generated.h"

UCLASS()
class ZOMBIESHOOTER_API APickupHealth : public ABasePickup
{
	GENERATED_BODY()

public:
	// Amount of health restored when collected
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Pickup")
	float HealAmount = 50.0f;

	// Executes healing logic and updates item state
	virtual void OnPickup(APlayerCharacter* Player) override;
};