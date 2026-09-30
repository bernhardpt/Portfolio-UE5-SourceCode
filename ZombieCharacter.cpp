#include "ZombieAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PlayerCharacter.h"

AZombieAIController::AZombieAIController()
{
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
    
	// Configure sight parameters
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1000.0f; 
	SightConfig->LoseSightRadius = 1200.0f; 
	SightConfig->PeripheralVisionAngleDegrees = 60.0f; 
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	// Configure hearing parameters
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	HearingConfig->HearingRange = 3000.0f; 
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

	// Register senses to perception component
	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->ConfigureSense(*HearingConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AZombieAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	if (!GetBlackboardComponent()) return;

	// Handle visual stimuli (Player spotting)
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
	// Handle auditory stimuli (Gunshots/Noises)
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

	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AZombieAIController::OnTargetDetected);
	}
	
	if (BehaviorTreeAsset && InPawn)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}