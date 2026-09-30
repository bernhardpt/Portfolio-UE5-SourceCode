// Preencher aviso de copyright no editor do Unreal.

#pragma once

#include "CoreMinimal.h"
#include "BasePickup.h"
#include "BaseWeapon.h"
#include "PickupAmmo.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESHOOTER_API APickupAmmo : public ABasePickup
{
	GENERATED_BODY()

public:
	//Quantas balas dá?
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Pickup")
	int32 AmmoAmount = 30;

	//Para que arma é esta munição?
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Pickup")
	TSubclassOf<ABaseWeapon> WeaponClassToRefill;

	virtual void OnPickup(APlayerCharacter* Player) override;
};




