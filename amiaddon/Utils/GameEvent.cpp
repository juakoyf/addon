#include "GameEvent.hpp"
#include "../SDK/Interfaces/Interfaces.hpp"
#include "FnvHash.hpp"
#include "../Family/Family.hpp"
#include "../SDK/Classes/Player.hpp"

#define ADD_GAMEEVENT(n)  g_pGameEvent->AddListener(this, #n, false)


GameEvent g_GameEvent;

std::vector<std::string> m_vecRagebait = {
	"say its okay",
	"say ask apache hes the ida goat LAAFFFFF",
	"say btw im leaking the method",
	"say thanks for spoonfeed im pasted",
	"say aa stuff from grimace could be thanos..",
	"say haiii heyyy hellooo",
	"say i might have to hop off",
	"say ok explain this you silver",
	"say i can debug in dev right?",
	"say jack changed the func name for manual",
	"say aa also has strings..."
};

size_t m_rageIndex = 0;

void GameEvent::Register() {
	ADD_GAMEEVENT(player_death);
}

void GameEvent::Shutdown() {
	g_pGameEvent->RemoveListener(this);
}

void GameEvent::FireGameEvent(IGameEvent* pEvent) {
	if (!pEvent)
		return;

	auto pLocal = C_CSPlayer::GetLocalPlayer();

	auto pEventHash = hash_32_fnv1a(pEvent->GetName());

	switch (pEventHash) {
		case hash_32_fnv1a_const("player_death"):
		{

			auto nEnemyIndex = pEvent->GetInt("userid");
			auto nAttackerIndex = pEvent->GetInt("attacker");
			auto pEnemy = g_pEngine->GetPlayerForUserID(nEnemyIndex);
			auto pAttacker = g_pEngine->GetPlayerForUserID(nAttackerIndex);

			// note - vix
			// it works but it doesnt work at the same time?
			// for some reason because of family ping spike 
			// its getting delayed like hours 
			// or not sending it cuz
			// Tried executing say as it would be an variable? 
			// or sum like that otherwise it works now so whateva
			
			if ( Family::bKillsay && pLocal && pEnemy != pLocal->EntIndex() && pAttacker == pLocal->EntIndex() )
			{
				if (m_vecRagebait.empty())
					return;

				if (m_rageIndex >= m_vecRagebait.size())
					m_rageIndex = 0;

				g_pEngine->ClientCmd(m_vecRagebait[m_rageIndex].c_str());

				m_rageIndex = (m_rageIndex + 1) % m_vecRagebait.size();
			}

			break;
		}
		case hash_32_fnv1a_const("round_start"):
		{
			Family::bRoundEnd = false;
			Family::bGameOver = false;
			break;
		}
		case hash_32_fnv1a_const("round_end"):
		{
			if (!pLocal)
				return;

			const int reason = pEvent->GetInt("reason");

			Family::bRoundEnd = false;

			if (pLocal->m_iTeamNum() == 3 && reason == 8)
				Family::bRoundEnd = true;

			else if (pLocal->m_iTeamNum() == 2 && reason == 9)
				Family::bRoundEnd = true;

			break;
		}
		case hash_32_fnv1a_const("player_spawn"):
		{
			auto enemy = pEvent->GetInt("userid");
			auto enemy_index = g_pEngine->GetPlayerForUserID(enemy);

			if (enemy_index == g_pEngine->GetLocalPlayer()) {
				Family::bGameOver = false;
			}

			break;
		}
		case hash_32_fnv1a_const("cs_intermission"):
		{
			Family::bGameOver = true;

			break;
		}
	}

}

int GameEvent::GetEventDebugID(void) {
	return 42;
}
