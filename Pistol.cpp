// Preencher aviso de copyright no editor do Unreal.


#include "Pistol.h"

APistol::APistol()
{
	//Valores iniciais

	//Dano
	BaseDamage = 15.0f;

	//Tempo entre tiros
	FireRate = 0.25f;

	//Não é automática
	bIsAutomatic = false;

	//Capacidade do pente
	MaxAmmoInMag = 12;

	//Quantidade atual de balas
	CurrentAmmoInMag = 12;

	//Munição de reserva
	TotalAmmoReserve = 60;
}




