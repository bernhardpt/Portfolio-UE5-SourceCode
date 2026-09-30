// Preencher aviso de copyright no editor do Unreal.


#include "ZombieAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PlayerCharacter.h"

AZombieAIController::AZombieAIController()
{
	//Criação do componente de perceção
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
    
	//Criação e configuração da visão
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    
	SightConfig->SightRadius = 1000.0f; //Vê até 10 metros
	SightConfig->LoseSightRadius = 1200.0f; //Deixa de ver aos 12m
	SightConfig->PeripheralVisionAngleDegrees = 60.0f; //Campo de visão

	//Configuração da audição
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 3000.0f; //30 metros
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerceptionComp->ConfigureSense(*HearingConfig);
	
	//Configurações de deteção
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	//Regista o sentido no componente
	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AZombieAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	//Se a blackboard não for válida, interrompe
	if (!GetBlackboardComponent()) return;

	//Verifica se o estímulo foi visual
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor))
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), PlayerCharacter);
				GetBlackboardComponent()->SetValueAsBool(TEXT("IsAlerted"), true);
			}
			else
			{
				GetBlackboardComponent()->ClearValue(TEXT("TargetActor"));
			}
		}
	}
	//Verifica se o estímulo foi auditivo
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			GetBlackboardComponent()->SetValueAsVector(TEXT("SoundLocation"), Stimulus.StimulusLocation);
			GetBlackboardComponent()->SetValueAsBool(TEXT("IsAlerted"), true);
		}
	}
}

void AZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//Liga a função ao evento de deteção
	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AZombieAIController::OnTargetDetected);
	}
	
	//Se houver um BT e um zombie válido
	if (BehaviorTreeAsset && InPawn)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}




