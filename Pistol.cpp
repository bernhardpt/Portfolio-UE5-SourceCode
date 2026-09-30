#include "Pistol.h"

APistol::APistol()
{
	// Configure pistol-specific combat parameters
	BaseDamage = 15.0f;
	FireRate = 0.25f;
	bIsAutomatic = false;

	// Initialize default ammunition capacities
	MaxAmmoInMag = 12;
	CurrentAmmoInMag = 12;
	TotalAmmoReserve = 60;
}