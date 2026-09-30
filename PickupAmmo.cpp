// Preencher aviso de copyright no editor do Unreal.


#include "PickupAmmo.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void APickupAmmo::OnPickup(APlayerCharacter* Player)
{
	if (WeaponClassToRefill)
	{
		Player->AddAmmo(WeaponClassToRefill, AmmoAmount);
		
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		}
		
		bIsActive = false;
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);

		
		
		UE_LOG(LogTemp, Log, TEXT("Munição apanhada!"));
	}
}




