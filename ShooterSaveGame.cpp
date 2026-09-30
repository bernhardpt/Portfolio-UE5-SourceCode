#include "ShooterSaveGame.h"

UShooterSaveGame::UShooterSaveGame()
{
	// Initialize default user settings (Volume at max, Graphics at High)
	MasterVolume = 1.0f; 
	MusicVolume = 1.0f; 
	GraphicsQuality = 2; 

	// Initialize default campaign state (prevents uninitialized variable errors on first launch)
	bHasSavedGame = false; 
	SavedWave = 0; 
	SavedZombiesRemaining = 0; 
	SavedScore = 0.0f; 
	SavedHealth = 100.0f; 
	SavedTimeRemaining = 0.0f; 
	SavedPlayerLocation = FVector::ZeroVector; 
	SavedPlayerRotation = FRotator::ZeroRotator; 
}