// Preencher aviso de copyright no editor do Unreal.


#include "ShooterGameInstance.h"
#include "ShooterSaveGame.h"
#include "Kismet/GameplayStatics.h"

//Função de inicialização da instância de jogo
void UShooterGameInstance::Init()
{
	Super::Init();
}

//Função que vai guardar a pontuação para a leaderboard
void UShooterGameInstance::SaveScoreToLeaderboard(float NewScore)
{
	//Cria a gravação do jogo
	UShooterSaveGame* SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::CreateSaveGameObject(UShooterSaveGame::StaticClass()));
    
	//Executa se existir uma gravação com o nome do slot
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		//Carrega a gravação do jogo
		SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}

	//Cria uma nova entrada na leaderboard
	FLeaderboardEntry NewEntry;
	
	//Define o nome do jogador escolhido. Caso não haja coloca "Unknown"
	NewEntry.PlayerName = CurrentPlayerName.IsEmpty() ? "Unknown" : CurrentPlayerName;
	
	//Define a pontuação
	NewEntry.Score = NewScore;
    
	//Define o texto que aparecerá na coluna da dificuldade
	if (SelectedDifficultyIndex == 0)
	{
		NewEntry.DifficultyName = "Easy";
	}
	else if (SelectedDifficultyIndex == 1)
	{
		NewEntry.DifficultyName = "Normal";
	}	
	else if (SelectedDifficultyIndex == 2)
	{ 
		NewEntry.DifficultyName = "Hard";
	}
	else
	{
		NewEntry.DifficultyName = "Unknown";
	}

	//Adiciona a nova entrada
	SaveInst->Leaderboard.Add(NewEntry);
    
	//Ordena as entradas
	SaveInst->Leaderboard.Sort([](const FLeaderboardEntry& A, const FLeaderboardEntry& B) 
	{
		return A.Score > B.Score;
	});

	//Mantém apenas o top 10
	if (SaveInst->Leaderboard.Num() > 10)
	{
		SaveInst->Leaderboard.SetNum(10);
	}

	//Grava no disco
	UGameplayStatics::SaveGameToSlot(SaveInst, SaveSlotName, 0);
}

//Array que chama as melhores pontuações
TArray<FLeaderboardEntry> UShooterGameInstance::GetTopScores()
{
	//Executa se a gravação existir
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		//Carrega o jogo
		UShooterSaveGame* LoadInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
		
		//Devolve uma entrada na leaderboard se existir gravação
		if (LoadInst)
		{ 
			return LoadInst->Leaderboard;
		}	
	}
	
	//Devolve array vazio se não existir gravação
	return TArray<FLeaderboardEntry>();
}

void UShooterGameInstance::SaveSettings(float MasterVol, float MusicVol, int32 Quality)
{
	//Por defeito torna o ponteiro da gravação nulo
	UShooterSaveGame* SaveInst = nullptr;

	//Executa se existir gravação
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		//Carrega a gravação
		SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}

	//Executa se não houver gravação
	if (!SaveInst)
	{
		//Cria uma nova gravação
		SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::CreateSaveGameObject(UShooterSaveGame::StaticClass()));
	}

	//Executa se houver gravação
	if (SaveInst)
	{
		//Define o valor do volume mestre
		SaveInst->MasterVolume = MasterVol;
		
		//Define o valor do volume da música
		SaveInst->MusicVolume = MusicVol;
		
		//Define a qualidade gráfica
		SaveInst->GraphicsQuality = Quality;

		//Grava no disco
		UGameplayStatics::SaveGameToSlot(SaveInst, SaveSlotName, 0);

		//Mostra valor no log
		UE_LOG(LogTemp, Log, TEXT("Opções guardadas: Vol=%.2f, Qualidade=%d"), MasterVol, Quality);
	}
}

//Função que carrega as definições
void UShooterGameInstance::LoadSettings(float& OutMasterVol, float& OutMusicVol, int32& OutQuality)
{
	//Define valores padrão
	OutMasterVol = 1.0f;
	OutMusicVol = 1.0f;
	OutQuality = 2;

	//Executa se existir gravação
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		//Carrega a gravação
		UShooterSaveGame* SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));

		//Executa se existir gravação
		if (SaveInst)
		{
			//Define os valores dos volumes e a qualidade gráfica
			OutMasterVol = SaveInst->MasterVolume;
			OutMusicVol = SaveInst->MusicVolume;
			OutQuality = SaveInst->GraphicsQuality;
            
			//Mostra no log se as opções foram bem carregadas
			UE_LOG(LogTemp, Log, TEXT("Opções Carregadas: Master=%.2f, Music=%.2f, Quality=%d"), OutMasterVol, OutMusicVol, OutQuality);
		}
	}
}

//Função que grava o progresso atual
void UShooterGameInstance::SaveCurrentProgress(int32 Wave, int32 ZombiesRemaining, float Score, float Health, int32 PrimMag, int32 PrimRes, int32 SecMag, int32 SecRes, float TimeRemaining, bool bIntermission, FVector PlayerLocation, FRotator PlayerRotation, const TArray<FString>& InCollectedPickups)
{
	//Cria a gravação do jogo
	UShooterSaveGame* SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::CreateSaveGameObject(UShooterSaveGame::StaticClass()));
	
	//Executa se houver gravação
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0)) 
	{
		//Carrega a gravação
		SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	}

	//Executa se houver gravação
	if (SaveInst) 
	{
		//Existe gravação
		SaveInst->bHasSavedGame = true;
		
		//Grava a onda
		SaveInst->SavedWave = Wave;
		
		//Grava o número restante de zombies
		SaveInst->SavedZombiesRemaining = ZombiesRemaining;
		
		//Grava a pontuação
		SaveInst->SavedScore = Score;
		
 		//Grava a vida
		SaveInst->SavedHealth = Health;
		
		//Grava a quantidade de balas em cada arma e no inventário
		SaveInst->PrimaryAmmoInMag = PrimMag;
		SaveInst->PrimaryAmmoReserve = PrimRes;
		SaveInst->SecondaryAmmoInMag = SecMag;
		SaveInst->SecondaryAmmoReserve = SecRes;
		
		//Grava o tempo restante
		SaveInst->SavedTimeRemaining = TimeRemaining;
		
		//Grava o estado do jogo
		SaveInst->bWasInIntermission = bIntermission;

		//Grava a localização e rotação do jogador
		SaveInst->SavedPlayerLocation = PlayerLocation;
		SaveInst->SavedPlayerRotation = PlayerRotation;

		// Passa a lista que recebemos do GameMode para dentro do SaveGame
    	SaveInst->CollectedPickups = InCollectedPickups;

		//Grava tudo no slot
		UGameplayStatics::SaveGameToSlot(SaveInst, SaveSlotName, 0);
	}
}

//Função que carrega o progresso caso ele exista
bool UShooterGameInstance::LoadCurrentProgress(int32& OutWave, int32& OutZombiesRemaining, float& OutScore, float& OutHealth, int32& OutPrimMag, int32& OutPrimRes, int32& OutSecMag, int32& OutSecRes, float& OutTimeRemaining, bool& OutIntermission, FVector& OutPlayerLocation, FRotator& OutPlayerRotation, TArray<FString>& OutCollectedPickups)
{
	//Executa se existir gravação
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0)) 
	{
		//Cria a gravação
		UShooterSaveGame* SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
		
		//Executa caso exista gravação e a variável de controlo seja verdadeira
		if (SaveInst && SaveInst->bHasSavedGame) 
		{
			//Define o valor da onda
			OutWave = SaveInst->SavedWave;
			
			//Define quantos zombies faltam nascer
			OutZombiesRemaining = SaveInst->SavedZombiesRemaining;
			
			//Define a pontuação
			OutScore = SaveInst->SavedScore;
			
			//Define a vida
			OutHealth = SaveInst->SavedHealth;
			
			//Define a quantidade de balas em cada arma e no inventário
			OutPrimMag = SaveInst->PrimaryAmmoInMag;
			OutPrimRes = SaveInst->PrimaryAmmoReserve;
			OutSecMag = SaveInst->SecondaryAmmoInMag;
			OutSecRes = SaveInst->SecondaryAmmoReserve;
			
			//Define o tempo restante
			OutTimeRemaining = SaveInst->SavedTimeRemaining;
			
			//Define o estado do jogo
			OutIntermission = SaveInst->bWasInIntermission;

			//Define a localização e rotação do jogador
			OutPlayerLocation = SaveInst->SavedPlayerLocation;
			OutPlayerRotation = SaveInst->SavedPlayerRotation;

			// Entrega a lista de pickups ao GameMode
            OutCollectedPickups = SaveInst->CollectedPickups;

			//Imprime no log se foi bem sucedido ou não
			UE_LOG(LogTemp, Warning, TEXT("Progresso carregado com sucesso!"));

			//Devolve verdadeiro
			return true;
		}
	}
	//Devolve falso
	return false;
}

//Função que limpa as variáveis do jogo
void UShooterGameInstance::ClearSavedProgress()
{

    //Executa caso exista gravação
    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
    {
		//Carrega a gravação
        UShooterSaveGame* SaveInst = Cast<UShooterSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
        
		//Executa caso exista gravação
        if (SaveInst)
        {
            //Definimos como falso
            SaveInst->bHasSavedGame = false;

            //Grava no disco
            UGameplayStatics::SaveGameToSlot(SaveInst, SaveSlotName, 0);
            
			//Imprime no log
            UE_LOG(LogTemp, Warning, TEXT("Progresso limpo devido a Game Over."));
        }
    }
}



