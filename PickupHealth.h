// Preencher aviso de copyright no editor do Unreal.

#pragma once

#include "CoreMinimal.h"
#include "BasePickup.h"
#include "PickupHealth.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESHOOTER_API APickupHealth : public ABasePickup
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Pickup")
	float HealAmount = 50.0f;

	virtual void OnPickup(APlayerCharacter* Player) override;
};




