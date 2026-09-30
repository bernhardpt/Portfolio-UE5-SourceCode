#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ShooterSaveGame.generated.h"

// Structure representing a single leaderboard entry
USTRUCT(BlueprintType)
struct FLeaderboardEntry
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FString PlayerName;
	
	UPROPERTY(BlueprintReadWrite)
	float Score;
	
	UPROPERTY(BlueprintReadWrite)
	FString DifficultyName;
};

UCLASS()
class ZOMBIESHOOTER_API UShooterSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UShooterSaveGame();
	
	// --- GLOBAL PERSISTENT DATA ---
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | Leaderboard")
	TArray<FLeaderboardEntry> Leaderboard;
	
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | Settings")
	float MasterVolume;

	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | Settings")
	float MusicVolume;
    
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | Settings")
	int32 GraphicsQuality;
	
	// --- CAMPAIGN PROGRESS DATA ---
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | State")
	bool bHasSavedGame = false; 

	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | State")
	int32 SavedWave;

	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | State")
	int32 SavedZombiesRemaining;

	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | State")
	float SavedScore;

	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | State")
	float SavedHealth;

	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | State")
	float SavedTimeRemaining;

	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | State")
	bool bWasInIntermission;
	
	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | Inventory")
	int32 PrimaryAmmoInMag;

	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | Inventory")
	int32 PrimaryAmmoReserve;

	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | Inventory")
	int32 SecondaryAmmoInMag;

	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | Inventory")
	int32 SecondaryAmmoReserve;

	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | World")
	FVector SavedPlayerLocation;

	UPROPERTY(BlueprintReadWrite, Category = "C++ | SaveData | World")
	FRotator SavedPlayerRotation;

	UPROPERTY(VisibleAnywhere, Category = "C++ | SaveData | World")
	TArray<FString> CollectedPickups; 
};