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
}; 

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,
	WaveInProgress,
	WaveIntermission,
	GameOver
}; 

UCLASS()
class ZOMBIESHOOTER_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainGameMode();

protected:
	virtual void BeginPlay() override;
	
	// Core timers for game loop management
	FTimerHandle TimerHandle_WaveTimer; 
	FTimerHandle TimerHandle_BreakTimer; 
	FTimerHandle TimerHandle_SpawnTimer; 

public:
	// --- GAME RULES & STATE ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++ | Rules")
	EDifficulty Difficulty; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++ | Score")
	int32 Score; 

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "C++ | Waves")
	int32 Wave; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++ | Game State")
	int32 ZombiesRemainingToSpawn; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "C++ | Game State")
	EWaveState CurrentState; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "C++ | Waves")
	int32 ZombiesPerWave = 100; 
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Waves")
	float WaveDuration = 300.0f; 
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Waves")
	float BreakDuration = 30.0f; 
	
	UPROPERTY(EditDefaultsOnly, Category = "C++ | Waves")
	TSubclassOf<class ACharacter> ZombieClass; 
	
	// --- GAME FLOW FUNCTIONS ---
	UFUNCTION(BlueprintCallable, Category = "C++ | UI")
	FString GetFormattedTime(); 

	UFUNCTION()
	void StartWave(); 
	
	UFUNCTION()
	void EndWave(); 
	
	UFUNCTION()
	void PrepareNextWave(); 
	
	UFUNCTION()
	void SpawnZombie(); 
	
	UFUNCTION()
	void OnPlayerDied(); 
	
	UFUNCTION()
	void GameOver(); 
	
	UFUNCTION()
	void OnZombieKilled(AController* Killer); 
	
	// --- DIFFICULTY MULTIPLIERS ---
	UFUNCTION(BlueprintCallable, Category = "C++ | Rules")
	float EnemyDmgMult() const; 
	
	UFUNCTION(BlueprintCallable, Category = "C++ | Rules")
	float PlayerDmgMult() const; 
	
	UFUNCTION(BlueprintCallable, Category = "C++ | Rules")
	float ScoreMult() const; 
	
	UPROPERTY()
	bool bIsClearingWave = false; 
	
	UFUNCTION(BlueprintImplementableEvent, Category = "C++ | UI")
	void ShowGameOverScreen(); 

	// --- SAVE & LOAD SYSTEM ---
	UFUNCTION(BlueprintCallable, Category = "C++ | Save System")
	void SaveGameData(); 

	UPROPERTY()
	bool bIsLoadedGame = false; 
	
	// Variables for state restoration upon loading
	UPROPERTY()
	float LoadedTime = 0.0f; 
	UPROPERTY()
	bool bLoadedIntermission = false; 
	UPROPERTY()
	int32 SavedWave; 
	UPROPERTY()
	int32 SavedZombies; 
	UPROPERTY()
	int32 PrimMag; 
	UPROPERTY()
	int32 PrimRes; 
	UPROPERTY()
	int32 SecMag; 
	UPROPERTY()
	int32 SecRes; 
	UPROPERTY()
	float SavedScore; 
	UPROPERTY()
	float SavedHealth; 
	UPROPERTY()
	float TimeLeft = 0.0f; 
	UPROPERTY()
	FVector LoadedPlayerLocation; 
	UPROPERTY()
	FRotator LoadedPlayerRotation; 
};