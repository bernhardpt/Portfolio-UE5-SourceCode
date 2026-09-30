#include "ShooterGameInstance.h"
#include "ShooterSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UShooterGameInstance::Init()
{
	Super::Init();
}

void UShooterGameInstance::SaveScoreToLeaderboard(float NewScore)
{
	UShooterSaveGame* SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::CreateSaveGameObject(UShooterSaveGame::StaticClass()));
    
	// Load existing file to append data rather than overwriting it
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}

	FLeaderboardEntry NewEntry;
	NewEntry.PlayerName = CurrentPlayerName.IsEmpty() ? "Unknown" : CurrentPlayerName;
	NewEntry.Score = NewScore;
    
	// Map difficulty index to readable string
	switch (SelectedDifficultyIndex)
	{
		case 0: NewEntry.DifficultyName = "Easy"; break;
		case 1: NewEntry.DifficultyName = "Normal"; break;
		case 2: NewEntry.DifficultyName = "Hard"; break;
		default: NewEntry.DifficultyName = "Unknown"; break;
	}

	SaveInst->Leaderboard.Add(NewEntry);
    
	// Sort descending by score
	SaveInst->Leaderboard.Sort([](const FLeaderboardEntry& A, const FLeaderboardEntry& B) 
	{
		return A.Score > B.Score;
	});

	// Enforce top 10 limit
	if (SaveInst->Leaderboard.Num() > 10)
	{
		SaveInst->Leaderboard.SetNum(10);
	}

	UGameplayStatics::SaveGameToSlot(SaveInst, SaveSlotName, 0);
}

TArray<FLeaderboardEntry> UShooterGameInstance::GetTopScores()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		if (UShooterSaveGame* LoadInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0)))
		{ 
			return LoadInst->Leaderboard;
		}	
	}
	
	return TArray<FLeaderboardEntry>();
}

void UShooterGameInstance::SaveSettings(float MasterVol, float MusicVol, int32 Quality)
{
	UShooterSaveGame* SaveInst = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}

	if (!SaveInst)
	{
		SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::CreateSaveGameObject(UShooterSaveGame::StaticClass()));
	}

	if (SaveInst)
	{
		SaveInst->MasterVolume = MasterVol;
		SaveInst->MusicVolume = MusicVol;
		SaveInst->GraphicsQuality = Quality;

		UGameplayStatics::SaveGameToSlot(SaveInst, SaveSlotName, 0);
		UE_LOG(LogTemp, Log, TEXT("Settings Saved: Master=%.2f, Quality=%d"), MasterVol, Quality);
	}
}

void UShooterGameInstance::LoadSettings(float& OutMasterVol, float& OutMusicVol, int32& OutQuality)
{
	// Provide default values
	OutMasterVol = 1.0f;
	OutMusicVol = 1.0f;
	OutQuality = 2;

	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		if (UShooterSaveGame* SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0)))
		{
			OutMasterVol = SaveInst->MasterVolume;
			OutMusicVol = SaveInst->MusicVolume;
			OutQuality = SaveInst->GraphicsQuality;
		}
	}
}

void UShooterGameInstance::SaveCurrentProgress(int32 Wave, int32 ZombiesRemaining, float Score, float Health, int32 PrimMag, int32 PrimRes, int32 SecMag, int32 SecRes, float TimeRemaining, bool bIntermission, FVector PlayerLocation, FRotator PlayerRotation, const TArray<FString>& InCollectedPickups)
{
	UShooterSaveGame* SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::CreateSaveGameObject(UShooterSaveGame::StaticClass()));
	
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0)) 
	{
		SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}

	if (SaveInst) 
	{
		SaveInst->bHasSavedGame = true;
		
		// Serialize game state
		SaveInst->SavedWave = Wave;
		SaveInst->SavedZombiesRemaining = ZombiesRemaining;
		SaveInst->SavedScore = Score;
		SaveInst->SavedHealth = Health;
		
		// Serialize inventory
		SaveInst->PrimaryAmmoInMag = PrimMag;
		SaveInst->PrimaryAmmoReserve = PrimRes;
		SaveInst->SecondaryAmmoInMag = SecMag;
		SaveInst->SecondaryAmmoReserve = SecRes;
		
		// Serialize world state
		SaveInst->SavedTimeRemaining = TimeRemaining;
		SaveInst->bWasInIntermission = bIntermission;
		SaveInst->SavedPlayerLocation = PlayerLocation;
		SaveInst->SavedPlayerRotation = PlayerRotation;
    	SaveInst->CollectedPickups = InCollectedPickups;

		UGameplayStatics::SaveGameToSlot(SaveInst, SaveSlotName, 0);
	}
}

bool UShooterGameInstance::LoadCurrentProgress(int32& OutWave, int32& OutZombiesRemaining, float& OutScore, float& OutHealth, int32& OutPrimMag, int32& OutPrimRes, int32& OutSecMag, int32& OutSecRes, float& OutTimeRemaining, bool& OutIntermission, FVector& OutPlayerLocation, FRotator& OutPlayerRotation, TArray<FString>& OutCollectedPickups)
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0)) 
	{
		UShooterSaveGame* SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
		
		if (SaveInst && SaveInst->bHasSavedGame) 
		{
			// Deserialize game state
			OutWave = SaveInst->SavedWave;
			OutZombiesRemaining = SaveInst->SavedZombiesRemaining;
			OutScore = SaveInst->SavedScore;
			OutHealth = SaveInst->SavedHealth;
			
			// Deserialize inventory
			OutPrimMag = SaveInst->PrimaryAmmoInMag;
			OutPrimRes = SaveInst->PrimaryAmmoReserve;
			OutSecMag = SaveInst->SecondaryAmmoInMag;
			OutSecRes = SaveInst->SecondaryAmmoReserve;
			
			// Deserialize world state
			OutTimeRemaining = SaveInst->SavedTimeRemaining;
			OutIntermission = SaveInst->bWasInIntermission;
			OutPlayerLocation = SaveInst->SavedPlayerLocation;
			OutPlayerRotation = SaveInst->SavedPlayerRotation;
            OutCollectedPickups = SaveInst->CollectedPickups;

			UE_LOG(LogTemp, Log, TEXT("Game progress successfully loaded."));
			return true;
		}
	}
	return false;
}

void UShooterGameInstance::ClearSavedProgress()
{
    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
    {
        if (UShooterSaveGame* SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0)))
        {
            // Flag progress as invalid without deleting the file (preserves settings/leaderboards)
            SaveInst->bHasSavedGame = false;
            UGameplayStatics::SaveGameToSlot(SaveInst, SaveSlotName, 0);
            
            UE_LOG(LogTemp, Log, TEXT("Progress cleared due to Game Over or New Game."));
        }
    }
}