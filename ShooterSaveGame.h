// Preencher aviso de copyright no editor do Unreal.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ShooterSaveGame.generated.h"

//Estrutura para cada linha da Leaderboard
USTRUCT(BlueprintType)
struct FLeaderboardEntry
{
	GENERATED_BODY()
	
	//Nome do jogador
	UPROPERTY(BlueprintReadWrite)
	FString PlayerName;
	
	//Pontuação
	UPROPERTY(BlueprintReadWrite)
	float Score;
	
	//Dificuldade usada
	UPROPERTY(BlueprintReadWrite)
	FString DifficultyName;
};

UCLASS()
class ZOMBIESHOOTER_API UShooterSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	//Construtor
	UShooterSaveGame();
	
	//Array para guardar dados na leaderboard
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	TArray<FLeaderboardEntry> Leaderboard;
	
	//Variável para guardar o valor de volume mestre
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	float MasterVolume;

    //Variável para guardar o valor de volume da música
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	float MusicVolume;
    
	//Variável para guardar a qualidade gráfica. 0 = Low, 1 = Medium, 2 = High, 3 = Epic
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	int32 GraphicsQuality;
	
	//Variável para ativar ou desativar o botão de Continue Game no menu principal
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	bool bHasSavedGame = false; 

    //Variável para guardar o número da onda onde o jogador ficou
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	int32 SavedWave;

    //Variável para guardar a pontuação que o jogador tinha quando gravou
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	float SavedScore;

    //Variável para guardar o valor da vida do jogador
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	float SavedHealth;

	//Variável para guardar a quantidade de tempo restante na onda ou na pausa
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	float SavedTimeRemaining;

	//Variável para saber se o jogador estava no meio da onda ou no intervalo entre elas
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	bool bWasInIntermission;
	
	//Variável para guardar a quantidade de balas que o jogador tinha no pente
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	int32 PrimaryAmmoInMag;

	//Variável para guardar a quantidade de balas que o jogador tinha no inventário
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	int32 PrimaryAmmoReserve;

	//Variável para guardar a quantidade de balas que o jogador tinha no pente
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	int32 SecondaryAmmoInMag;

	//Variável para guardar a quantidade de balas que o jogador tinha no inventário
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	int32 SecondaryAmmoReserve;

	//Variável que guarda o número de zombies que ainda faltam spawnar
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	int32 SavedZombiesRemaining;

	//Variável para guardar a localização do jogador quando gravou
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	FVector SavedPlayerLocation;

	//Variável para guardar a rotação do jogador quando gravou
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData")
	FRotator SavedPlayerRotation;

	UPROPERTY(VisibleAnywhere, Category = "C++ | SaveSystem")
	TArray<FString> CollectedPickups; //Array para armazenar os nomes dos pickups coletados, para que possamos salvar essa informação no progresso do jogo e restaurá-la corretamente quando o jogador carregar o jogo novamente, para evitar que os pickups coletados reapareçam no mundo após carregar um jogo salvo
	
};




