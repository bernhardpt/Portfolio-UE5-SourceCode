// Preencher aviso de copyright no editor do Unreal.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ShooterSaveGame.h"
#include "ShooterGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESHOOTER_API UShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	//Variável que guarda o nome atual do jogador
	UPROPERTY(BlueprintReadWrite, Category = "C++ | Session")
	FString CurrentPlayerName;

	//Variável que guarda a dificuldade escolhida
	UPROPERTY(BlueprintReadWrite, Category = "C++ | Session")
	int32 SelectedDifficultyIndex;
	
	//Chamada quando o jogo abre para carregar opções
	virtual void Init() override;

	//Função que guarda o valor da pontuação para a leaderboard
	UFUNCTION(BlueprintCallable, Category = "C++ | SaveSystem")
	void SaveScoreToLeaderboard(float NewScore);

	//Array que chama as melhores pontuações
	UFUNCTION(BlueprintCallable, Category = "C++ | SaveSystem")
	TArray<FLeaderboardEntry> GetTopScores();

	//Função que guarda as definições de volume e de qualidade gráfica
	UFUNCTION(BlueprintCallable, Category = "C++ | SaveSystem")
	void SaveSettings(float MasterVol, float MusicVol, int32 Quality);

	//Função que carrega as definições guardadas
	UFUNCTION(BlueprintCallable, Category = "C++ | SaveSystem")
	void LoadSettings(float& OutMasterVol, float& OutMusicVol, int32& OutQuality);

	//Função que guarda o progresso do jogo
    UFUNCTION(BlueprintCallable, Category = "C++ | SaveSystem")
    void SaveCurrentProgress(int32 Wave, int32 ZombiesRemaining, float Score, float Health, int32 PrimMag, int32 PrimRes, int32 SecMag, int32 SecRes, float TimeRemaining, bool bIntermission, FVector PlayerLocation, FRotator PlayerRotation, const TArray<FString>& InCollectedPickups);

    //Função que carrega o progresso do jogo caso esse jogo gravado exista
    UFUNCTION(BlueprintCallable, Category = "C++ | SaveSystem")
    bool LoadCurrentProgress(int32& OutWave, int32& OutZombiesRemaining, float& OutScore, float& OutHealth, int32& OutPrimMag, int32& OutPrimRes, int32& OutSecMag, int32& OutSecRes, float& OutTimeRemaining, bool& OutIntermission, FVector& OutPlayerLocation, FRotator& OutPlayerRotation, TArray<FString>& OutCollectedPickups);
	
	//Função que limpa o progresso gravado caso se comece um novo jogo ou caso o jogador morra
	UFUNCTION(BlueprintCallable, Category = "C++ | SaveSystem")
	void ClearSavedProgress();

	//Variável do nome do slot do jogo gravado
	UPROPERTY()
	FString SaveSlotName = "SaveSlot01";

};




