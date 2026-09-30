#include "PickupHealth.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void APickupHealth::OnPickup(APlayerCharacter* Player)
{
	// Only consume the pickup if the player actually requires healing
	if (Player->Heal(HealAmount))
	{
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		}
		
		// Deactivate and hide the pickup until the GameMode resets it for the next wave
		bIsActive = false;
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		
		UE_LOG(LogTemp, Log, TEXT("Player healed."));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Health is already full. Pickup ignored."));
	}
}