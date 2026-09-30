// Preencher aviso de copyright no editor do Unreal.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "ZombieAIController.generated.h"

class UBehaviorTree;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

UCLASS()
class ZOMBIESHOOTER_API AZombieAIController : public AAIController
{
	GENERATED_BODY()

public:
	//Construtor
	AZombieAIController();
	
	//O cérebro (a atribuir no editor)
	UPROPERTY(EditDefaultsOnly, Category = "C++ | AI")
	UBehaviorTree* BehaviorTreeAsset;

	//Componente que vê
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComp;

	//Configuração da visão
	UAISenseConfig_Sight* SightConfig;

	//Configuração da audição
	UAISenseConfig_Hearing* HearingConfig;

	//Função chamada quando o sensor deteta algo
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

	//Override da função
	virtual void OnPossess(APawn* InPawn) override;
};




