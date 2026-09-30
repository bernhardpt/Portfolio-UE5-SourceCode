#include "PickupAmmo.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void APickupAmmo::OnPickup(APlayerCharacter* Player)
{
	// Ensure a target weapon class is configured before attempting to grant ammo
	if (WeaponClassToRefill)
	{
		// Grant ammo to the specified weapon class reserve
		Player->AddAmmo(WeaponClassToRefill, AmmoAmount);
		
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		}
		
		// Deactivate and hide the pickup until the GameMode resets it for the next wave
		bIsActive = false;
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		
		UE_LOG(LogTemp, Log, TEXT("Ammunition collected."));
	}
}