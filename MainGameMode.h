// Preencher aviso de copyright no editor do Unreal.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

UENUM(BlueprintType)
enum class EDifficulty : uint8
{
	Easy	UMETA(DisplayName = "Easy"),
	Normal	UMETA(DisplayName = "Normal"),
	Hard	UMETA(DisplayName = "Hard"),
}; //Enum para definir a dificuldade do jogo

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,
	WaveInProgress,
	WaveIntermission,
	GameOver
}; //Enum para definir o estado atual do jogo

UCLASS()
class ZOMBIESHOOTER_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainGameMode();

protected:
	virtual void BeginPlay() override;
	
	FTimerHandle TimerHandle_WaveTimer; //Timer para a duração da onda
	FTimerHandle TimerHandle_BreakTimer; //Timer para a duração do intervalo
	FTimerHandle TimerHandle_SpawnTimer; //Timer para o spawn dos zombies

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++ | Regras")
	EDifficulty Difficulty; //Dificuldade do jogo, definida no início
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++ | Pontuação")
	int32 Score; //Pontuação atual do jogador, incrementada por cada zombie morto

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++ | Ondas")
	int32 Wave; //Número da onda atual, incrementado no início de cada onda
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++ | Estado do Jogo")
	int32 ZombiesRemainingToSpawn; //Número de zombies restantes para spawnar na onda atual, decrementado a cada spawn
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++ | Estado do Jogo")
	EWaveState CurrentState; //Estado atual do jogo, usado para controlar a lógica de spawn, pontuação, etc.
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Ondas")
	int32 ZombiesPerWave = 100; //Número de zombies a spawnar por onda, pode ser ajustado para aumentar a dificuldade progressivamente
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Ondas")
	float WaveDuration = 300.0f; //Duração de cada onda em segundos, após o que a onda termina automaticamente
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Ondas")
	float BreakDuration = 30.0f; //Duração do intervalo entre ondas em segundos, durante o qual o jogador pode se preparar para a próxima onda
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Ondas")
	TSubclassOf<class ACharacter> ZombieClass; //Classe do zombie a ser spawnada, definida no editor
	
	UFUNCTION(BlueprintCallable, Category = "C++ | UI")
	FString GetFormattedTime(); //Função para obter o tempo formatado para exibição no UI, mostrando o tempo restante da onda ou do intervalo dependendo do estado atual do jogo

	UFUNCTION()
	void StartWave(); //Função para iniciar uma nova onda, incrementando o número da onda, resetando o número de zombies restantes para spawnar, e iniciando os timers de duração da onda e spawn de zombies
	
	UFUNCTION()
	void EndWave(); //Função para terminar a onda atual, parando o spawn de novos zombies, matando os zombies restantes no mapa, e iniciando o intervalo
	
	UFUNCTION()
	void PrepareNextWave(); //Função para preparar a próxima onda, definindo o estado do jogo para intervalo e iniciando o timer de duração do intervalo
	
	UFUNCTION()
	void SpawnZombie(); //Função para spawnar um zombie em um TargetPoint aleatório do mapa, decrementando o número de zombies restantes para spawnar, e parando o timer de spawn quando não houver mais zombies para spawnar ou a onda tiver terminado
	
	UFUNCTION()
	void OnPlayerDied(); //Função para lidar com a morte do jogador, definindo o estado do jogo para GameOver e mostrando a tela de Game Over
	
	UFUNCTION()
	void GameOver(); //Função para lidar com o Game Over, mostrando a tela de Game Over e parando toda a lógica do jogo
	
	UFUNCTION()
	void OnZombieKilled(AController* Killer); //Função para lidar com a morte de um zombie, incrementando a pontuação do jogador com base na dificuldade e no multiplicador de pontuação, e ignorando a atribuição de pontos se estivermos a limpar a onda após o tempo esgotar
	
	UFUNCTION(BlueprintCallable, Category = "C++ | Regras")
	float EnemyDmgMult() const; //Função para obter o multiplicador de dano dos inimigos com base na dificuldade, usada para calcular o dano que os zombies causam ao jogador
	
	UFUNCTION(BlueprintCallable, Category = "C++ | Regras")
	float PlayerDmgMult() const; //Função para obter o multiplicador de dano do jogador com base na dificuldade, usada para calcular o dano que o jogador causa aos zombies
	
	UFUNCTION(BlueprintCallable, Category = "C++ | Regras")
	float ScoreMult() const; //Função para obter o multiplicador de pontuação com base na dificuldade, usada para calcular a pontuação dada por cada zombie morto
	
	UPROPERTY()
	bool bIsClearingWave = false; //Variável para indicar se estamos a limpar a onda após o tempo esgotar, usada para evitar que o jogador ganhe pontos por matar zombies durante a limpeza da onda
	
	UFUNCTION(BlueprintImplementableEvent, Category = "C++ | UI")
	void ShowGameOverScreen(); //Evento para mostrar a tela de Game Over, implementado em Blueprint para permitir a criação de uma UI personalizada

	UFUNCTION(BlueprintCallable, Category = "C++ | Save System")
	void SaveGameData(); //Função para salvar os dados do jogo, incluindo a onda atual, número de zombies restantes para spawnar, pontuação, saúde do jogador, munição das armas, tempo restante no temporizador, estado de intervalo ou combate, e transform do jogador. Chamado quando o jogador escolhe salvar o jogo no menu de pausa.

	UPROPERTY()
	bool bIsLoadedGame = false; //Variável para indicar se o jogo foi carregado de um save, usada para controlar a lógica de início do mapa e temporizadores
	
	UPROPERTY()
	float LoadedTime = 0.0f; //Variável para armazenar o tempo carregado do save, usada para reiniciar os temporizadores com o tempo correto após carregar um jogo salvo
	
	UPROPERTY()
	bool bLoadedIntermission = false; //Variável para indicar se o jogo foi salvo durante um intervalo, usada para definir o estado do jogo corretamente após carregar um jogo salvo

	UPROPERTY()
	int32 SavedWave; //Variável para armazenar a onda salva, usada para definir a onda atual após carregar um jogo salvo
	
	UPROPERTY()
	int32 SavedZombies; //Variável para armazenar o número de zombies restantes para spawnar salvo, usada para definir o número de zombies restantes para spawnar após carregar um jogo salvo
	
	UPROPERTY()
	int32 PrimMag; //Variável para armazenar a munição atual da arma primária salva, usada para definir a munição da arma primária após carregar um jogo salvo
	
	UPROPERTY()
	int32 PrimRes; //Variável para armazenar a munição de reserva da arma primária salva, usada para definir a munição de reserva da arma primária após carregar um jogo salvo
	
	UPROPERTY()
	int32 SecMag; //Variável para armazenar a munição atual da arma secundária salva, usada para definir a munição da arma secundária após carregar um jogo salvo
	
	UPROPERTY()
	int32 SecRes; //Variável para armazenar a munição de reserva da arma secundária salva, usada para definir a munição de reserva da arma secundária após carregar um jogo salvo
	
	UPROPERTY()
	float SavedScore; //Variável para armazenar a pontuação salva, usada para definir a pontuação atual após carregar um jogo salvo
	
	UPROPERTY()
	float SavedHealth; //Variável para armazenar a saúde do jogador salva, usada para definir a saúde atual do jogador após carregar um jogo salvo
	
	UPROPERTY()
	float TimeLeft = 0.0f; //Variável para armazenar o tempo restante do temporizador, usada para mostrar o tempo correto no UI e para salvar o tempo correto no save file

	UPROPERTY()
	FVector LoadedPlayerLocation; //Variável para armazenar a localização do jogador salva, usada para definir a localização do jogador após carregar um jogo salvo

	UPROPERTY()
	FRotator LoadedPlayerRotation; //Variável para armazenar a rotação do jogador salva, usada para definir a rotação do jogador após carregar um jogo salvo

};



