#include "Application.h"
#include "ModuleTime.h"

#include "ComponentRenderer.h"

#include "BossStateInterPhase.h"

#include "BossBehaviourScript.h"
#include "EnemyControllerScript/EnemyControllerScript.h"

BossStateInterPhase::BossStateInterPhase(BossBehaviourScript* AIBoss)
{
	boss = AIBoss;
}


BossStateInterPhase::~BossStateInterPhase()
{
}

void BossStateInterPhase::HandleIA()
{
	if (finished)
	{
		boss->currentState = (BossState*)boss->idle;
	}
}

void BossStateInterPhase::Update()
{

	switch (ipState)
	{
	case IpState::None:
		ipState = IpState::Fadeoff;
		break;
	case IpState::Fadeoff:

		if (boss->enemyController->GetMainRenderer()->dissolveAmount >= 1.0f)
		{
			ipState = IpState::TP;
		}
		else
		{
			for (auto render : boss->enemyController->myRenders)
			{
				render->dissolveAmount += boss->App->time->gameDeltaTime;
			}
		}

		break;
	case IpState::TP:

		//move her to the throne
		boss->enemyController->SetPosition(boss->firstInterphasePosition);
		boss->enemyController->LookAt2D(boss->pointToLookAtFirstInterphase);
		ipState = IpState::FadeIn;

		break;
	case IpState::FadeIn:

		if (boss->enemyController->GetMainRenderer()->dissolveAmount <= 0.0f)
		{
			ipState = IpState::Powerup;
		}
		else
		{
			for (auto render : boss->enemyController->myRenders)
			{
				render->dissolveAmount -= boss->App->time->gameDeltaTime;
			}
		}

		break;
	case IpState::Powerup:
		//this could be better animation driven, but w/e
		if (powerUpTimer < boss->firstInterphaseDuration)
		{
			ipState = IpState::Relocate;
		}
		else
		{
			powerUpTimer += boss->App->time->gameDeltaTime;
		}

		break;

	case IpState::Relocate:
	{
		//better to move and not to TP
		relocateTimer += boss->App->time->gameDeltaTime;

		float lambda = relocateTimer / boss->relocateInterPhaseTime;

		boss->InterpolateFloat3(boss->firstInterphasePosition, boss->topTP, lambda);

		if (lambda >= 1.0f)
		{
			ipState = IpState::Finished;
		}
	}
		break;

	case IpState::Finished:
		finished = true;
		break;
	}
	// disappear



	

	//Go the throne

	//appear

	//do the full animation

	//make her do the TP
}

void BossStateInterPhase::Enter()
{
	//Here we reset all bosses variables for the next phase
	boss->ResetVariables();
}

void BossStateInterPhase::Exit()
{
}
