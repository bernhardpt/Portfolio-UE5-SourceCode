// Preencher aviso de copyright no editor do Unreal.


#include "PickupHealth.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void APickupHealth::OnPickup(APlayerCharacter* Player)
{
	//Só destrói se o jogador realmente aceitou a cura
	if (Player->Heal(HealAmount))
	{
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		}
		
		bIsActive = false;
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		
		
		UE_LOG(LogTemp, Log, TEXT("Curado!"));
		
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Vida cheia. Pickup ignorado."));
	}
}




