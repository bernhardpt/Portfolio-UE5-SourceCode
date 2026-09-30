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

	CurrentState = EWaveState::WaitingToStart; //Estado inicial do jogo
	
	Difficulty = EDifficulty::Normal; //Dificuldade padrão, pode ser alterada no início do jogo

	LoadedPlayerLocation = FVector::ZeroVector; //Inicializa a localização do jogador carregado para (0,0,0) por padrão
	LoadedPlayerRotation = FRotator::ZeroRotator; //Inicializa a rotação do jogador carregado para (0,0,0) por padrão
	
}

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UShooterGameInstance* GI = Cast<UShooterGameInstance>(GetGameInstance()))
	{
		int32 DiffIndex = GI->SelectedDifficultyIndex; //Obtém o índice da dificuldade selecionada no menu principal
		Difficulty = (EDifficulty)DiffIndex; //Define a dificuldade do jogo com base no índice selecionado

		TArray<FString> LoadedPickups; //Array para armazenar as pickups carregadas do progresso do jogo

		if (GI->LoadCurrentProgress(SavedWave, SavedZombies, SavedScore, SavedHealth, PrimMag, PrimRes, SecMag, SecRes, LoadedTime, bLoadedIntermission, LoadedPlayerLocation, LoadedPlayerRotation, LoadedPickups))
		{
			Wave = SavedWave; //Define a onda atual com base no progresso carregado
			
			ZombiesRemainingToSpawn = SavedZombies; //Define o número de zombies restantes para spawnar com base no progresso carregado
			
			Score = SavedScore; //Define a pontuação atual com base no progresso carregado

			TArray<AActor*> FoundPickups; //Array para armazenar as pickups encontradas no mapa
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePickup::StaticClass(), FoundPickups); //Obtém todas as pickups do mapa e armazena no array

			for (AActor* Actor : FoundPickups)
			{
				if (LoadedPickups.Contains(Actor->GetName())) //Verifica se o nome da pickup encontrada está na lista de pickups carregadas do progresso do jogo
				{
					ABasePickup* Pickup = Cast<ABasePickup>(Actor); //Faz um cast do ator encontrado para ABasePickup, para que possamos chamar a função de reset nessa pickup específica

					if (Pickup)
					{
						Pickup->SetActorHiddenInGame(true); //Esconde a pickup no jogo, para que o jogador não possa vê-la ou interagir com ela, já que ela já foi coletada no progresso carregado
						Pickup->SetActorEnableCollision(false); //Desativa a colisão da pickup, para que o jogador não possa colidir com ela ou tentar coletá-la, já que ela já foi coletada no progresso carregado
					}
				}
			}
			
			bIsLoadedGame = true; //Indica que um jogo foi carregado, o que afetará a lógica de início da onda e spawn de zombies
			
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Tempo Lido do Ficheiro: %f"), LoadedTime)); //Mensagem de debug para verificar o tempo carregado do save

		}
	}
	
	if (bIsLoadedGame)
	{
		CurrentState = bLoadedIntermission ? EWaveState::WaveIntermission : EWaveState::WaveInProgress; //Define o estado do jogo com base no progresso carregado, para que a lógica de início da onda e spawn de zombies funcione corretamente

		if (bLoadedIntermission)
		{
			if (LoadedTime > 0.1f)
			{
				GetWorldTimerManager().SetTimer(TimerHandle_BreakTimer, this, &AMainGameMode::StartWave, LoadedTime, false); //Inicia o timer do intervalo com o tempo restante carregado, para que a próxima onda comece automaticamente quando o intervalo terminar
			}
			else
			{
				StartWave(); //Se o tempo carregado for 0 ou negativo, inicia a próxima onda imediatamente
			}
		}
		else
		{
			if (LoadedTime > 0.1f)
			{
				GetWorldTimerManager().SetTimer(TimerHandle_WaveTimer, this, &AMainGameMode::EndWave, LoadedTime, false); //Inicia o timer da onda com o tempo restante carregado, para que a onda termine automaticamente quando o tempo acabar
			}

			GetWorldTimerManager().SetTimer(TimerHandle_SpawnTimer, this, &AMainGameMode::SpawnZombie, 2.0f, true); //Inicia o timer de spawn de zombies para continuar spawnando os zombies restantes da onda carregada

			if (APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
			{
				Player->SetActorLocation(LoadedPlayerLocation); //Define a localização do jogador com base no progresso carregado
				Player->SetActorRotation(LoadedPlayerRotation); //Define a rotação do jogador com base no progresso carregado
				Player->CurrentHealth = SavedHealth; //Define a saúde do jogador com base no progresso carregado
			}
		}
	}
	else
	{
		PrepareNextWave(); //Se nenhum progresso foi carregado, inicia o jogo normalmente preparando a primeira onda
	}
}


void AMainGameMode::PrepareNextWave()
{
	CurrentState = EWaveState::WaveIntermission; //Define o estado do jogo para intervalo, para que a lógica de início da onda e spawn de zombies funcione corretamente

	TArray<AActor*> AllPickups; //Array para armazenar todas as pickups no mapa
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePickup::StaticClass(), AllPickups); //Obtém todas as pickups do mapa e armazena no array

	for (AActor* Actor : AllPickups)
	{
		if (ABasePickup* Pickup = Cast<ABasePickup>(Actor))
		{
			Pickup->ResetPickup(); //Chama a função de reset em cada pickup para reabastecer as armas do jogador, para que ele comece a próxima onda com munição cheia
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Pickups reabastecidos!")); //Mensagem de log para indicar que as pickups foram reabastecidas
	
	GetWorldTimerManager().SetTimer(TimerHandle_BreakTimer, this, &AMainGameMode::StartWave, BreakDuration, false); //Inicia o timer do intervalo, que quando terminar chamará a função StartWave para iniciar a próxima onda

	UE_LOG(LogTemp, Warning, TEXT("--- INTERVALO: A ONDA %d COMEÇA EM %f SEGUNDOS ---"), Wave + 1, BreakDuration); //Mensagem de log para indicar que o intervalo começou e quando a próxima onda começará, mostrando o número da próxima onda e a duração do intervalo
	
}

FString AMainGameMode::GetFormattedTime()
{

	if (CurrentState == EWaveState::WaveIntermission)
	{
		TimeLeft = GetWorldTimerManager().GetTimerRemaining(TimerHandle_BreakTimer); //Obtém o tempo restante do timer do intervalo, para mostrar na tela durante o intervalo entre as ondas
	}
	else if (CurrentState == EWaveState::WaveInProgress) 
	{
		TimeLeft = GetWorldTimerManager().GetTimerRemaining(TimerHandle_WaveTimer); //Obtém o tempo restante do timer da onda, para mostrar na tela durante a onda
	}
	else 
	{
		return TEXT("STATE ERROR"); //Se o estado do jogo não for intervalo ou onda em progresso, retorna uma string de erro, embora isso não deva acontecer
	}

	if (TimeLeft <= 0.0f)
	{
		return TEXT("0"); //Se o tempo restante for 0 ou negativo, retorna "0" para evitar mostrar números negativos na tela
	}

	int32 SecondsLeft = FMath::CeilToInt(TimeLeft); //Arredonda o tempo restante para cima para o próximo segundo inteiro, para mostrar um número inteiro de segundos restantes na tela, sem mostrar 0 até que o tempo realmente acabe

	return FString::FromInt(SecondsLeft); //Converte o número de segundos restantes para string e retorna, para mostrar na tela durante o intervalo e a onda
}

void AMainGameMode::StartWave()
{
	Wave++; //Incrementa o número da onda, para que a próxima onda seja numerada corretamente

	CurrentState = EWaveState::WaveInProgress; //Define o estado do jogo para onda em progresso, para que a lógica de spawn de zombies e pontuação funcione corretamente

	ZombiesRemainingToSpawn = ZombiesPerWave; //Define o número de zombies restantes para spawnar com base no número de zombies por onda, para que a lógica de spawn funcione corretamente e a onda tenha o número correto de zombies

	UE_LOG(LogTemp, Warning, TEXT("--- ONDA %d INICIADA! ---"), Wave); //Mensagem de log para indicar que a onda começou, mostrando o número da onda atual

	GetWorldTimerManager().SetTimer(TimerHandle_WaveTimer, this, &AMainGameMode::EndWave, WaveDuration, false); //Inicia o timer da onda, que quando terminar chamará a função EndWave para terminar a onda atual

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnTimer, this, &AMainGameMode::SpawnZombie, 2.0f, true); //Inicia o timer de spawn de zombies, que chamará a função SpawnZombie a cada 2 segundos para spawnar os zombies da onda
	
}


void AMainGameMode::SpawnZombie()
{
	if (ZombiesRemainingToSpawn <= 0 || CurrentState != EWaveState::WaveInProgress)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_SpawnTimer); //Se não houver mais zombies para spawnar ou a onda tiver terminado, limpa o timer de spawn para parar de chamar essa função
		
		return; //E sai da função para evitar spawnar zombies desnecessariamente
	}

	TArray<AActor*> SpawnPoints; //Array para armazenar os pontos de spawn disponíveis no mapa
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), SpawnPoints); //Obtém todos os atores do tipo TargetPoint do mapa e armazena no array, para usar como pontos de spawn para os zombies

	if (SpawnPoints.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1); //Gera um índice aleatório para escolher um ponto de spawn do array, para que os zombies spawnem em locais variados no mapa
		FVector Location = SpawnPoints[Index]->GetActorLocation(); //Obtém a localização do ponto de spawn escolhido, para spawnar o zombie nessa localização
		FRotator Rotation = SpawnPoints[Index]->GetActorRotation(); //Obtém a rotação do ponto de spawn escolhido, para spawnar o zombie com essa rotação

		Location.X += FMath::RandRange(-50.0f, 50.0f); //Adiciona um deslocamento aleatório na posição X para evitar que os zombies spawnem exatamente no mesmo local, para criar mais variedade e evitar que eles se empilhem
		Location.Y += FMath::RandRange(-50.0f, 50.0f); //Adiciona um deslocamento aleatório na posição Y para evitar que os zombies spawnem exatamente no mesmo local, para criar mais variedade e evitar que eles se empilhem

		FActorSpawnParameters SpawnParams; //Estrutura para definir parâmetros de spawn do ator
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; //Define o método de tratamento de colisão para o spawn, para que os zombies tentem ajustar sua posição se houver colisão, mas ainda assim spawnem mesmo que não consigam encontrar um local sem colisão

		if (ZombieClass)
		{
			GetWorld()->SpawnActor<ACharacter>(ZombieClass, Location, Rotation, SpawnParams); //Spawna o zombie do tipo definido em ZombieClass na localização e rotação escolhidas, usando os parâmetros de spawn definidos

			ZombiesRemainingToSpawn--; //Decrementa o número de zombies restantes para spawnar, para que a lógica de spawn funcione corretamente e a onda tenha o número correto de zombies
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ERRO: Não há TargetPoints no mapa! Adiciona alguns.")); //Se não houver pontos de spawn disponíveis, loga um erro para indicar que é necessário adicionar TargetPoints no mapa para que os zombies possam spawnar

		}
	}
	
}


void AMainGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_SpawnTimer); //Limpa o timer de spawn para parar de spawnar novos zombies, já que a onda está terminando

	UE_LOG(LogTemp, Warning, TEXT("TEMPO ESGOTADO! A limpar zombies restantes...")); //Mensagem de log para indicar que o tempo da onda acabou e os zombies restantes estão sendo limpos, para que o jogador saiba que a onda terminou mesmo que ainda haja zombies no mapa

	bIsClearingWave = true; //Define a flag para indicar que estamos limpando a onda, para que a lógica de pontuação e morte dos zombies funcione corretamente durante esse processo

	TArray<AActor*> ExistingZombies; //Array para armazenar os zombies existentes no mapa, para que possamos iterar sobre eles e matá-los para limpar a onda
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AZombieCharacter::StaticClass(), ExistingZombies); //Obtém todos os atores do tipo AZombieCharacter do mapa e armazena no array, para iterar sobre eles e matá-los para limpar a onda

	for (AActor* Actor : ExistingZombies)
	{
		if (AZombieCharacter* Zombie = Cast<AZombieCharacter>(Actor))
		{
			Zombie->Die();  //Chama a função de morte em cada zombie para matá-los e limpar a onda, para que o jogador possa se preparar para a próxima onda sem se preocupar com os zombies restantes da onda anterior
		}
	}

	bIsClearingWave = false; //Reseta a flag de limpeza da onda, para que a lógica de pontuação e morte dos zombies funcione corretamente para a próxima onda

	PrepareNextWave(); //Chama a função para preparar a próxima onda, que definirá o estado do jogo para intervalo e iniciará o timer de duração do intervalo, para que o jogador tenha um tempo para se preparar antes da próxima onda começar
}


void AMainGameMode::OnPlayerDied()
{
	CurrentState = EWaveState::GameOver; //Define o estado do jogo para GameOver, para que a lógica de pontuação, spawn de zombies, etc. funcione corretamente e o jogo saiba que acabou

	if (UShooterGameInstance* GI = Cast<UShooterGameInstance>(GetGameInstance()))
	{
		GI->SaveScoreToLeaderboard(Score); //Chama a função para salvar a pontuação atual no leaderboard, para que o jogador possa competir com outros jogadores e ver sua posição no ranking

		GI->ClearSavedProgress(); //Chama a função para limpar o progresso salvo, para que quando o jogador começar um novo jogo ele não carregue o progresso anterior e comece do zero
	}

	GameOver(); //Chama a função para finalizar o jogo, que pausará o jogo, mostrará o cursor e exibirá a tela de Game Over para o jogador, para que ele saiba que perdeu e possa escolher o que fazer a seguir
}

void AMainGameMode::GameOver()
{
	GetWorldTimerManager().ClearAllTimersForObject(this); //Limpa todos os timers relacionados a este objeto, para garantir que nenhum timer continue rodando após o jogo acabar, o que poderia causar comportamentos indesejados ou erros

	UE_LOG(LogTemp, Error, TEXT("GAME OVER! Final Score: %d"), Score); //Mensagem de log para indicar que o jogo acabou e mostrar a pontuação final do jogador, para que ele saiba que perdeu e qual foi sua pontuação final

	UGameplayStatics::SetGamePaused(GetWorld(), true); //Pausa o jogo, para que o jogador possa ver a tela de Game Over e escolher o que fazer a seguir sem que o jogo continue rodando em segundo plano

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->bShowMouseCursor = true; //Mostra o cursor do mouse, para que o jogador possa clicar nos botões da tela de Game Over para escolher o que fazer a seguir (como voltar ao menu principal, reiniciar o jogo, etc.)

		PC->SetInputMode(FInputModeUIOnly()); //Define o modo de entrada para UI Only, para que o jogador possa interagir com a interface da tela de Game Over sem que o personagem do jogo responda a inputs, para evitar que o jogador continue controlando o personagem mesmo após perder
       
		ShowGameOverScreen();  //Chama a função para mostrar a tela de Game Over, que exibirá a pontuação final do jogador e opções para o que fazer a seguir, para que ele possa escolher se quer voltar ao menu principal, reiniciar o jogo, etc.
	}
}

void AMainGameMode::OnZombieKilled(AController* Killer)
{
	if (bIsClearingWave) 
	{
		return; //Se estamos limpando a onda, não adicionamos pontos por matar zombies, para evitar que o jogador ganhe pontos facilmente matando os zombies restantes após o tempo da onda acabar
	}

	float BasePoints = 100.0f; //Número base de pontos por matar um zombie, pode ser ajustado para aumentar ou diminuir a pontuação por kill
    
	int32 PointsToAdd = FMath::RoundToInt(BasePoints * ScoreMult()); //Calcula os pontos a adicionar com base no multiplicador de pontuação da dificuldade, para que o jogador ganhe mais pontos em dificuldades mais altas e menos pontos em dificuldades mais fáceis

	Score += PointsToAdd; //Adiciona os pontos calculados à pontuação atual, para que o jogador veja sua pontuação aumentar a cada zombie morto e possa competir por uma pontuação mais alta
}


float AMainGameMode::PlayerDmgMult() const
{
	switch (Difficulty)
	{
	case EDifficulty::Easy:   return 2.0f;
	case EDifficulty::Normal: return 1.0f;
	case EDifficulty::Hard:   return 0.5f; 
	default: return 1.0f;
	} //Retorna o multiplicador de dano para o jogador com base na dificuldade, para que o jogador cause mais dano em dificuldades mais fáceis e menos dano em dificuldades mais difíceis, aumentando o desafio do jogo
}

float AMainGameMode::EnemyDmgMult() const
{
	switch (Difficulty)
	{
	case EDifficulty::Easy:   return 0.5f; 
	case EDifficulty::Normal: return 1.0f;
	case EDifficulty::Hard:   return 2.0f;
	default: return 1.0f;
	} //Retorna o multiplicador de dano para os inimigos com base na dificuldade, para que os inimigos causem menos dano em dificuldades mais fáceis e mais dano em dificuldades mais difíceis, aumentando o desafio do jogo
}

float AMainGameMode::ScoreMult() const
{
	switch (Difficulty)
	{
	case EDifficulty::Easy:   return 0.5f; 
	case EDifficulty::Normal: return 1.0f;
	case EDifficulty::Hard:   return 2.0f;
	default: return 1.0f;
	} //Retorna o multiplicador de pontuação com base na dificuldade, para que o jogador ganhe menos pontos em dificuldades mais fáceis e mais pontos em dificuldades mais difíceis, incentivando os jogadores a jogarem em dificuldades mais altas para obter uma pontuação maior
}

void AMainGameMode::SaveGameData()
{
	UShooterGameInstance* GI = Cast<UShooterGameInstance>(GetGameInstance()); //Obtém uma referência para o GameInstance, para acessar as funções de salvar o progresso do jogo, para que possamos salvar o estado atual do jogo quando necessário (como quando o jogador salva manualmente ou quando o jogo é pausado)

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)); //Obtém uma referência para o personagem do jogador, para acessar suas propriedades como saúde, munição, localização, etc., para que possamos salvar essas informações no progresso do jogo e restaurá-las corretamente quando o jogador carregar o jogo novamente

	if (GI && Player)
	{
		int32 P_Mag = 0, P_Res = 0, S_Mag = 0, S_Res = 0; //Variáveis para armazenar a munição atual e reserva da arma primária e secundária do jogador, para salvar essas informações no progresso do jogo e restaurá-las corretamente quando o jogador carregar o jogo novamente

		if (Player->RifleRef)
		{
			P_Mag = Player->RifleRef->CurrentAmmoInMag; //Obtém a munição atual na arma primária do jogador, para salvar essa informação no progresso do jogo e restaurá-la corretamente quando o jogador carregar o jogo novamente
			P_Res = Player->RifleRef->TotalAmmoReserve; //Obtém a munição total de reserva da arma primária do jogador, para salvar essa informação no progresso do jogo e restaurá-la corretamente quando o jogador carregar o jogo novamente
		}

		if (Player->PistolRef)
		{
			S_Mag = Player->PistolRef->CurrentAmmoInMag; //Obtém a munição atual na arma secundária do jogador, para salvar essa informação no progresso do jogo e restaurá-la corretamente quando o jogador carregar o jogo novamente
			S_Res = Player->PistolRef->TotalAmmoReserve; //Obtém a munição total de reserva da arma secundária do jogador, para salvar essa informação no progresso do jogo e restaurá-la corretamente quando o jogador carregar o jogo novamente
		}

		bool bIsIntermission = (CurrentState == EWaveState::WaveIntermission); //Determina se o jogo está atualmente no intervalo entre ondas, para salvar essa informação no progresso do jogo e restaurar o estado correto quando o jogador carregar o jogo novamente

		if (bIsIntermission)
		{
			TimeLeft = GetWorldTimerManager().GetTimerRemaining(TimerHandle_BreakTimer); //Se estivermos no intervalo, obtém o tempo restante do timer do intervalo para salvar essa informação no progresso do jogo e restaurar o tempo correto quando o jogador carregar o jogo novamente, para que o jogador tenha o tempo correto restante do intervalo quando carregar o jogo
		}
		else
		{
			TimeLeft = GetWorldTimerManager().GetTimerRemaining(TimerHandle_WaveTimer); //Se estivermos na onda em progresso, obtém o tempo restante do timer da onda para salvar essa informação no progresso do jogo e restaurar o tempo correto quando o jogador carregar o jogo novamente, para que o jogador tenha o tempo correto restante da onda quando carregar o jogo
		}

		TArray<FString> PickupsToSave; //Array para armazenar os nomes das pickups coletadas pelo jogador, para salvar essa informação no progresso do jogo e restaurá-la corretamente quando o jogador carregar o jogo novamente, para que as pickups coletadas sejam mantidas mesmo após carregar o jogo
		TArray<AActor*> FoundPickups; //Array para armazenar as pickups encontradas no mapa, para comparar com as pickups coletadas e determinar quais delas devem ser salvas no progresso do jogo
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePickup::StaticClass(), FoundPickups); //Obtém todas as pickups do mapa e armazena no array, para comparar com as pickups coletadas e determinar quais delas devem ser salvas no progresso do jogo

		for (AActor* Actor : FoundPickups)
		{
			ABasePickup* Pickup = Cast<ABasePickup>(Actor); //Tenta fazer cast de cada ator encontrado para o tipo ABasePickup, para acessar suas propriedades e determinar se ele foi coletado pelo jogador

			if(Pickup && Pickup->IsHidden())
			{
				PickupsToSave.Add(Pickup->GetName()); //Se a pickup estiver escondida, significa que ela foi coletada pelo jogador, então adicionamos o nome dela ao array de pickups a salvar no progresso do jogo, para que possamos restaurar corretamente as pickups coletadas quando o jogador carregar o jogo novamente
			}
		}

		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Tempo CRU lido do temporizador: %f"), TimeLeft)); //Mensagem de debug para verificar o tempo cru obtido do temporizador, para garantir que estamos salvando o tempo correto restante do intervalo ou da onda no progresso do jogo, o que é crucial para restaurar o estado correto do jogo quando o jogador carregar o jogo novamente
		
		if (TimeLeft < 0.0f) TimeLeft = 0.0f; //Garante que o tempo restante não seja negativo, para evitar salvar um tempo inválido no progresso do jogo, o que poderia causar problemas ao restaurar o estado do jogo quando o jogador carregar o jogo novamente

		if (Player)
		{
			GI->SaveCurrentProgress(Wave, ZombiesRemainingToSpawn, Score, Player->CurrentHealth, P_Mag, P_Res, S_Mag, S_Res, TimeLeft, bIsIntermission, Player->GetActorLocation(), Player->GetActorRotation(), PickupsToSave); //Chama a função para salvar o progresso atual do jogo, passando todas as informações relevantes como número da onda, zombies restantes para spawnar, pontuação, saúde do jogador, munição atual e reserva das armas, tempo restante do intervalo ou da onda, se estamos no intervalo ou na onda, e a localização e rotação do jogador, para que possamos restaurar corretamente o estado do jogo quando o jogador carregar o jogo novamente

			UE_LOG(LogTemp, Warning, TEXT("Jogo Gravado com sucesso!")); //Mensagem de log para indicar que o jogo foi salvo com sucesso, para que o jogador saiba que seu progresso foi salvo e pode carregar esse progresso mais tarde
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Não foi possível gravar porque o Jogador não existe.")); //Se o jogador não existir (o que pode acontecer se ele estiver morto), loga um erro para indicar que o jogo não pode ser salvo, já que não podemos salvar informações como saúde, localização, rotação, etc. do jogador, o que é crucial para restaurar o estado do jogo corretamente quando o jogador carregar o jogo novamente
		}
		
	}
}



