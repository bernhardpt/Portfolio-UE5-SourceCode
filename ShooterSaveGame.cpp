// Preencher aviso de copyright no editor do Unreal.


#include "ShooterSaveGame.h"

//Construtor
UShooterSaveGame::UShooterSaveGame()
{
	//Inicializa os valores padrão para as variáveis de salvamento
	MasterVolume = 1.0f; //Valor padrão para o volume mestre
	MusicVolume = 1.0f; //Valor padrão para o volume da música
	GraphicsQuality = 2; //

	//Valores padrão para o progresso do jogo
	bHasSavedGame = false; //Indica que não há jogo salvo
	SavedWave = 0; //Número da onda salva
	SavedZombiesRemaining = 0; //Número de zombies restantes para spawnar
	SavedScore = 0.0f; //Pontuação salva
	SavedHealth = 100.0f; //Vida salva
	SavedTimeRemaining = 0.0f; //Tempo restante salvo
	SavedPlayerLocation = FVector::ZeroVector; //Localização salva
	SavedPlayerRotation = FRotator::ZeroRotator; //Rotação salva
}




