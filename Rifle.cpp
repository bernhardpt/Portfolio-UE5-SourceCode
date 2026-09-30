// Preencher aviso de copyright no editor do Unreal.


#include "Rifle.h"
#include "TimerManager.h"

ARifle::ARifle()
{
	//Valores iniciais

	//Dano
	BaseDamage = 25.0f;

	//Tempo entre tiros
	FireRate = 0.12f;

	//É automática
	bIsAutomatic = true;

	//Capacidade do pente
	MaxAmmoInMag = 30;
	
}




