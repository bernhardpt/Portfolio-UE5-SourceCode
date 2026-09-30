#include "MainGameMode.h"
#include "BasePickup.h"
#include "ShooterGameInstance.h"
#include "ZombieCharacter.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"
#include "Engine/TargetPoint.h"

AMainGameMode::AMainGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize default game state
	CurrentState = EWaveState::WaitingToStart; 
	Difficulty = EDifficulty::Normal; 
	LoadedPlayerLocation = FVector::ZeroVector; 
	LoadedPlayerRotation = FRotator::ZeroRotator; 
}

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Attempt to retrieve persistent data and settings from the Game Instance
	if (UShooterGameInstance* GI = Cast<UShooterGameInstance>(GetGameInstance()))
	{
		int32 DiffIndex = GI->SelectedDifficultyIndex; 
		Difficulty = (EDifficulty)DiffIndex; 

		TArray<FString> LoadedPickups; 

		// Check if there is a valid save file to reconstruct the level state
		if (GI->LoadCurrentProgress(SavedWave, SavedZombies, SavedScore, SavedHealth, PrimMag, PrimRes, SecMag, SecRes, LoadedTime, bLoadedIntermission, LoadedPlayerLocation, LoadedPlayerRotation, LoadedPickups))
		{
			Wave = SavedWave; 
			ZombiesRemainingToSpawn = SavedZombies; 
			Score = SavedScore; 

			// Reconstruct world state by removing already collected pickups
			TArray<AActor*> FoundPickups; 
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePickup::StaticClass(), FoundPickups); 

			for (AActor* Actor : FoundPickups)
			{
				if (LoadedPickups.Contains(Actor->GetName())) 
				{
					if (ABasePickup* Pickup = Cast<ABasePickup>(Actor))
					{
						Pickup->SetActorHiddenInGame(true); 
						Pickup->SetActorEnableCollision(false); 
					}
				}
			}
			
			bIsLoadedGame = true; 
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Loaded Time from file: %f"), LoadedTime)); 
		}
	}
	
	// Branch execution based on whether we are loading a save or starting fresh
	if (bIsLoadedGame)
	{
		CurrentState = bLoadedIntermission ? EWaveState::WaveIntermission : EWaveState::WaveInProgress; 

		if (bLoadedIntermission)
		{
			if (LoadedTime > 0.1f)
			{
				GetWorldTimerManager().SetTimer(TimerHandle_BreakTimer, this, &AMainGameMode::StartWave, LoadedTime, false); 
			}
			else
			{
				StartWave(); 
			}
		}
		else
		{
			if (LoadedTime > 0.1f)
			{
				GetWorldTimerManager().SetTimer(TimerHandle_WaveTimer, this, &AMainGameMode::EndWave, LoadedTime, false); 
			}

			GetWorldTimerManager().SetTimer(TimerHandle_SpawnTimer, this, &AMainGameMode::SpawnZombie, 2.0f, true); 

			// Restore player transforms and attributes
			if (APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
			{
				Player->SetActorLocation(LoadedPlayerLocation); 
				Player->SetActorRotation(LoadedPlayerRotation); 
				Player->CurrentHealth = SavedHealth; 
			}
		}
	}
	else
	{
		// Fresh start
		PrepareNextWave(); 
	}
}

void AMainGameMode::PrepareNextWave()
{
	CurrentState = EWaveState::WaveIntermission; 

	// Replenish all pickups in the map for the upcoming wave
	TArray<AActor*> AllPickups; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePickup::StaticClass(), AllPickups); 

	for (AActor* Actor : AllPickups)
	{
		if (ABasePickup* Pickup = Cast<ABasePickup>(Actor))
		{
			Pickup->ResetPickup(); 
		}
	}

	GetWorldTimerManager().SetTimer(TimerHandle_BreakTimer, this, &AMainGameMode::StartWave, BreakDuration, false); 
}

FString AMainGameMode::GetFormattedTime()
{
	// Fetch the correct timer based on the current GameMode state
	if (CurrentState == EWaveState::WaveIntermission)
	{
		TimeLeft = GetWorldTimerManager().GetTimerRemaining(TimerHandle_BreakTimer); 
	}
	else if (CurrentState == EWaveState::WaveInProgress) 
	{
		TimeLeft = GetWorldTimerManager().GetTimerRemaining(TimerHandle_WaveTimer); 
	}
	else 
	{
		return TEXT("STATE ERROR"); 
	}

	if (TimeLeft <= 0.0f)
	{
		return TEXT("0"); 
	}

	int32 SecondsLeft = FMath::CeilToInt(TimeLeft); 
	return FString::FromInt(SecondsLeft); 
}

void AMainGameMode::StartWave()
{
	Wave++; 
	CurrentState = EWaveState::WaveInProgress; 
	ZombiesRemainingToSpawn = ZombiesPerWave; 

	GetWorldTimerManager().SetTimer(TimerHandle_WaveTimer, this, &AMainGameMode::EndWave, WaveDuration, false); 
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnTimer, this, &AMainGameMode::SpawnZombie, 2.0f, true); 
}

void AMainGameMode::SpawnZombie()
{
	if (ZombiesRemainingToSpawn <= 0 || CurrentState != EWaveState::WaveInProgress)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_SpawnTimer); 
		return; 
	}

	TArray<AActor*> SpawnPoints; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), SpawnPoints); 

	if (SpawnPoints.Num() > 0)
	{
		// Select a random TargetPoint
		int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1); 
		FVector Location = SpawnPoints[Index]->GetActorLocation(); 
		FRotator Rotation = SpawnPoints[Index]->GetActorRotation(); 

		// Apply random offset to prevent AI from overlapping heavily on spawn
		Location.X += FMath::RandRange(-50.0f, 50.0f); 
		Location.Y += FMath::RandRange(-50.0f, 50.0f); 

		FActorSpawnParameters SpawnParams; 
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; 

		if (ZombieClass)
		{
			GetWorld()->SpawnActor<ACharacter>(ZombieClass, Location, Rotation, SpawnParams); 
			ZombiesRemainingToSpawn--; 
		}
	}
}

void AMainGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_SpawnTimer); 

	// Flag to prevent score increments during cleanup
	bIsClearingWave = true; 

	TArray<AActor*> ExistingZombies; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZombieCharacter::StaticClass(), ExistingZombies); 

	for (AActor* Actor : ExistingZombies)
	{
		if (AZombieCharacter* Zombie = Cast<AZombieCharacter>(Actor))
		{
			Zombie->Die();  
		}
	}

	bIsClearingWave = false; 
	PrepareNextWave(); 
}

void AMainGameMode::OnPlayerDied()
{
	CurrentState = EWaveState::GameOver; 

	if (UShooterGameInstance* GI = Cast<UShooterGameInstance>(GetGameInstance()))
	{
		// Submit score and wipe current progress upon death
		GI->SaveScoreToLeaderboard(Score); 
		GI->ClearSavedProgress(); 
	}

	GameOver(); 
}

void AMainGameMode::GameOver()
{
	// Halt all internal loop mechanics
	GetWorldTimerManager().ClearAllTimersForObject(this); 
	UGameplayStatics::SetGamePaused(GetWorld(), true); 

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		// Return control to UI
		PC->bShowMouseCursor = true; 
		PC->SetInputMode(FInputModeUIOnly()); 
       
		ShowGameOverScreen();  
	}
}

void AMainGameMode::OnZombieKilled(AController* Killer)
{
	if (bIsClearingWave) 
	{
		return; 
	}

	float BasePoints = 100.0f; 
	// Scale score reward based on difficulty multiplier
	int32 PointsToAdd = FMath::RoundToInt(BasePoints * ScoreMult()); 
	Score += PointsToAdd; 
}

float AMainGameMode::PlayerDmgMult() const
{
	switch (Difficulty)
	{
	case EDifficulty::Easy:   return 2.0f;
	case EDifficulty::Normal: return 1.0f;
	case EDifficulty::Hard:   return 0.5f; 
	default: return 1.0f;
	} 
}

float AMainGameMode::EnemyDmgMult() const
{
	switch (Difficulty)
	{
	case EDifficulty::Easy:   return 0.5f; 
	case EDifficulty::Normal: return 1.0f;
	case EDifficulty::Hard:   return 2.0f;
	default: return 1.0f;
	} 
}

float AMainGameMode::ScoreMult() const
{
	switch (Difficulty)
	{
	case EDifficulty::Easy:   return 0.5f; 
	case EDifficulty::Normal: return 1.0f;
	case EDifficulty::Hard:   return 2.0f;
	default: return 1.0f;
	} 
}

void AMainGameMode::SaveGameData()
{
	UShooterGameInstance* GI = Cast<UShooterGameInstance>(GetGameInstance()); 
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)); 

	if (GI && Player)
	{
		// Extract weapon ammunition states
		int32 P_Mag = 0, P_Res = 0, S_Mag = 0, S_Res = 0; 

		if (Player->RifleRef)
		{
			P_Mag = Player->RifleRef->CurrentAmmoInMag; 
			P_Res = Player->RifleRef->TotalAmmoReserve; 
		}

		if (Player->PistolRef)
		{
			S_Mag = Player->PistolRef->CurrentAmmoInMag; 
			S_Res = Player->PistolRef->TotalAmmoReserve; 
		}

		// Snapshot current wave timer
		bool bIsIntermission = (CurrentState == EWaveState::WaveIntermission); 

		if (bIsIntermission)
		{
			TimeLeft = GetWorldTimerManager().GetTimerRemaining(TimerHandle_BreakTimer); 
		}
		else
		{
			TimeLeft = GetWorldTimerManager().GetTimerRemaining(TimerHandle_WaveTimer); 
		}

		// Track collected pickups to ensure they don't respawn on load
		TArray<FString> PickupsToSave; 
		TArray<AActor*> FoundPickups; 
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePickup::StaticClass(), FoundPickups); 

		for (AActor* Actor : FoundPickups)
		{
			if(ABasePickup* Pickup = Cast<ABasePickup>(Actor))
			{
				if (Pickup->IsHidden())
				{
					PickupsToSave.Add(Pickup->GetName()); 
				}
			}
		}
		
		if (TimeLeft < 0.0f) TimeLeft = 0.0f; 

		// Serialize all data to GameInstance
		GI->SaveCurrentProgress(Wave, ZombiesRemainingToSpawn, Score, Player->CurrentHealth, P_Mag, P_Res, S_Mag, S_Res, TimeLeft, bIsIntermission, Player->GetActorLocation(), Player->GetActorRotation(), PickupsToSave); 
	}
}