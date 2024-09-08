#include "includes.h"

bool Hooks::InPrediction() {
	return g_hooks.m_prediction.GetOldMethod< InPrediction_t >(CPrediction::INPREDICTION)(this);
}

void __fastcall RunCommand::Hook(void* ecx, void* edx, Player* player, CUserCmd* ucmd, void* moveHelper) {
	if (!player || !player->alive())
		return original(ecx, edx, player, ucmd, moveHelper);

	const int tickrate = (int)std::round(1.f / g_csgo.m_globals->m_interval);
	static ConVar* sv_max_usercmd_future_ticks = g_csgo.m_cvar->FindVar(HASH("sv_max_usercmd_future_ticks"));

	if (ucmd->m_tick > (g_csgo.m_globals->m_tick_count + tickrate + sv_max_usercmd_future_ticks->GetInt())) {
		ucmd->m_predicted = true;

		player->SetAbsOrigin(player->m_vecOrigin());
		return;
	}

	const float velocity_modifier = player->m_flVelocityModifier();

	original(ecx, edx, player, ucmd, moveHelper);

	player->m_flVelocityModifier() = velocity_modifier;

	// store non compressed netvars.
	g_netdata.store();

	g_inputpred.UpdateViewmodelData();
}	