#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ShooterSaveGame.h"
#include "ShooterGameInstance.generated.h"

UCLASS()
class ZOMBIESHOOTER_API UShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// --- SESSION STATE ---
	UPROPERTY(BlueprintReadWrite, Category = "C++ | Session")
	FString CurrentPlayerName;

	UPROPERTY(BlueprintReadWrite, Category = "C++ | Session")
	int32 SelectedDifficultyIndex;
	
	virtual void Init() override;

	// --- LEADERBOARD SYSTEM ---
	UFUNCTION(BlueprintCallable, Category = "C++ | Save System")
	void SaveScoreToLeaderboard(float NewScore);

	UFUNCTION(BlueprintCallable, Category = "C++ | Save System")
	TArray<FLeaderboardEntry> GetTopScores();

	// --- USER SETTINGS SYSTEM ---
	UFUNCTION(BlueprintCallable, Category = "C++ | Save System")
	void SaveSettings(float MasterVol, float MusicVol, int32 Quality);

	UFUNCTION(BlueprintCallable, Category = "C++ | Save System")
	void LoadSettings(float& OutMasterVol, float& OutMusicVol, int32& OutQuality);

	// --- GAME PROGRESS SERIALIZATION ---
    UFUNCTION(BlueprintCallable, Category = "C++ | Save System")
    void SaveCurrentProgress(int32 Wave, int32 ZombiesRemaining, float Score, float Health, int32 PrimMag, int32 PrimRes, int32 SecMag, int32 SecRes, float TimeRemaining, bool bIntermission, FVector PlayerLocation, FRotator PlayerRotation, const TArray<FString>& InCollectedPickups);

    UFUNCTION(BlueprintCallable, Category = "C++ | Save System")
    bool LoadCurrentProgress(int32& OutWave, int32& OutZombiesRemaining, float& OutScore, float& OutHealth, int32& OutPrimMag, int32& OutPrimRes, int32& OutSecMag, int32& OutSecRes, float& OutTimeRemaining, bool& OutIntermission, FVector& OutPlayerLocation, FRotator& OutPlayerRotation, TArray<FString>& OutCollectedPickups);
	
	// Flags the current progress as invalid (used on Game Over) while preserving settings and leaderboards
	UFUNCTION(BlueprintCallable, Category = "C++ | Save System")
	void ClearSavedProgress();

	// Target save file identifier on disk
	UPROPERTY()
	FString SaveSlotName = "SaveSlot01";
};