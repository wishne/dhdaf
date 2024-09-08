#include "includes.h"
#include "pred.h"

InputPrediction g_inputpred{};;

void InputPrediction::update() {
	bool        valid{ g_csgo.m_cl->m_delta_tick > 0 };

	int start = g_csgo.m_cl->m_last_command_ack;
	int stop = g_csgo.m_cl->m_last_outgoing_command + g_csgo.m_cl->m_choked_commands;

	if (valid && g_cl.m_stage == FRAME_NET_UPDATE_END)
		g_csgo.m_prediction->Update(g_csgo.m_cl->m_delta_tick, valid, start, stop);

	static auto cl_move_clamp = pattern::find(g_csgo.m_engine_dll, XOR("B8 ? ? ? ? 3B F0 0F 4F F0 89 5D FC")) + 1;
	unsigned long protect = 0;

	VirtualProtect((void*)cl_move_clamp, 4, PAGE_EXECUTE_READWRITE, &protect);
	*(std::uint32_t*)cl_move_clamp = 62;
	VirtualProtect((void*)cl_move_clamp, 4, protect, &protect);

	g_cl.m_unpred_netvars.store(g_cl.m_local);
}

void InputPrediction::run(bool repredict) {
	static CMoveData data{};

	// CPrediction::StartCommand
	g_cl.m_local->m_pCurrentCommand() = g_cl.m_cmd;
	g_cl.m_local->m_PlayerCommand() = *g_cl.m_cmd;

	*g_csgo.m_nPredictionRandomSeed = g_cl.m_cmd->m_random_seed;
	g_csgo.m_pPredictionPlayer = g_cl.m_local;

	// backup globals.
	m_curtime = g_csgo.m_globals->m_curtime;
	m_frametime = g_csgo.m_globals->m_frametime;

	// CPrediction::RunCommand

	// set globals appropriately.
	g_csgo.m_globals->m_curtime = game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase());
	g_csgo.m_globals->m_frametime = g_csgo.m_prediction->m_engine_paused ? 0.f : g_csgo.m_globals->m_interval;

	// set target player ( host ).
	g_csgo.m_move_helper->SetHost(g_cl.m_local);
	g_csgo.m_game_movement->StartTrackPredictionErrors(g_cl.m_local);

	// setup input.
	g_csgo.m_prediction->SetupMove(g_cl.m_local, g_cl.m_cmd, g_csgo.m_move_helper, &data);

	// run movement.
	g_csgo.m_game_movement->ProcessMovement(g_cl.m_local, &data);
	g_csgo.m_prediction->FinishMove(g_cl.m_local, g_cl.m_cmd, &data);

	if (g_cl.m_weapon)
		g_cl.m_weapon->UpdateAccuracyPenalty();
}

void InputPrediction::restore() {
	g_csgo.m_game_movement->FinishTrackPredictionErrors(g_cl.m_local);
	g_csgo.m_move_helper->SetHost(nullptr);

	g_cl.m_local->m_pCurrentCommand() = NULL;

	*g_csgo.m_nPredictionRandomSeed = -1;
	g_csgo.m_pPredictionPlayer = nullptr;

	g_csgo.m_game_movement->Reset();

	g_cl.m_unpred_netvars.restore(g_cl.m_local);

	// restore globals.
	g_csgo.m_globals->m_curtime = m_curtime;
	g_csgo.m_globals->m_frametime = m_frametime;
}

void InputPrediction::CorrectViewmodelData() {
	if (!g_cl.m_local || !g_cl.m_processing) {
		return;
	}

	if (g_cl.m_local->m_hViewModel() != 0xFFFFFFFF) {
		Entity* const pViewModel = g_csgo.m_entlist->GetClientEntityFromHandle(g_cl.m_local->m_hViewModel());

		if (!pViewModel) {
			return;
		}

		pViewModel->m_flAnimTime() = StoredViewmodel.m_flViewmodelAnimTime;
		pViewModel->m_flCycle() = StoredViewmodel.m_flViewmodelCycle;
	}
}

void InputPrediction::UpdateViewmodelData() {
	if (!g_cl.m_local || !g_cl.m_processing) {
		return;
	}

	if (g_cl.m_local->m_hViewModel() != 0xFFFFFFFF) {
		Entity* const pViewModel = g_csgo.m_entlist->GetClientEntityFromHandle(g_cl.m_local->m_hViewModel());

		if (!pViewModel) {
			return;
		}

		StoredViewmodel.m_flViewmodelCycle = pViewModel->m_flCycle();
		StoredViewmodel.m_flViewmodelAnimTime = pViewModel->m_flAnimTime();
	}
}