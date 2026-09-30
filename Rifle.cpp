#include "Rifle.h"
#include "TimerManager.h"

ARifle::ARifle()
{
	// Configure rifle-specific combat parameters
	BaseDamage = 25.0f;
	FireRate = 0.12f;
	bIsAutomatic = true;

	// Initialize default ammunition capacities
	MaxAmmoInMag = 30;
}