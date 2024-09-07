#include "includes.h"

bool Hooks::TempEntities( void *msg ) {
	if( !g_cl.m_processing ) {
		return g_hooks.m_client_state.GetOldMethod< TempEntities_t >( CClientState::TEMPENTITIES )( this, msg );
	}

	const bool ret = g_hooks.m_client_state.GetOldMethod< TempEntities_t >( CClientState::TEMPENTITIES )( this, msg );

	CEventInfo *ei = g_csgo.m_cl->m_events; 
	CEventInfo *next = nullptr;

	if( !ei ) {
		return ret;
	}

	do {
		next = *reinterpret_cast< CEventInfo ** >( reinterpret_cast< uintptr_t >( ei ) + 0x38 );

		uint16_t classID = ei->m_class_id - 1;

		auto m_pCreateEventFn = ei->m_client_class->m_pCreateEvent;
		if( !m_pCreateEventFn ) {
			continue;
		}

		void *pCE = m_pCreateEventFn( );
		if( !pCE ) {
			continue;
		}

		if( classID == 170 ){
			ei->m_fire_delay = 0.0f;
		}
		ei = next;
	} while( next != nullptr );

	return ret;
}

#define KAABA_KEY 0.0007479516421f
#define CHEESE_KEY 69.f

void __fastcall Hooks::hkVoiceData(void* msg) {

	static auto m_oVoiceData = g_hooks.m_client_state.GetOldMethod< FnVoiceData >(CClientState::VOICEDATA);

	if (!msg)
		return m_oVoiceData(this, msg);

	auto local = g_cl.m_local;


	CSVCMsg_VoiceData_Legacy* msg_ = (CSVCMsg_VoiceData_Legacy*)msg;
	int idx = msg_->client + 1;
	AimPlayer* data = &g_aimbot.m_players[idx - 1];
	VoiceDataCustom msg_data = msg_->get_data();

	if (!local || !local->alive() || !g_csgo.m_engine->IsConnected2() || !g_csgo.m_engine->IsInGame() || !g_cl.m_processing)
		return m_oVoiceData(this, msg);



	if (msg_->format != 0)
		return m_oVoiceData(this, msg);

	// check if its empty
	if (msg_data.section_number == 0 && msg_data.sequence_bytes == 0 && msg_data.uncompressed_sample_offset == 0)
		return m_oVoiceData(this, msg);

	if (local->index() == idx) {
		return m_oVoiceData(this, msg);
	}

	Voice_Vader* packet = (Voice_Vader*)msg_data.get_raw_data();

	// webster
	// old wNeJbkUsTleR
	if (!strcmp(packet->cheat_name, XOR("familhok2"))) {
		data->leak_familysadface = true;
	}
	if (!strcmp(packet->cheat_name, XOR("familhok3"))) {
		data->real_familyhappyface = true;
	}
	if (!strcmp(packet->cheat_name, XOR("Am1G0R3c0D3D"))) {
		data->m_is_amigo = true;
	}
	if (!strcmp(packet->cheat_name, XOR("imperial"))) {
		if (data) {
			data->m_networked_angle = msg_data.section_number / static_cast<float>(15);
		}
		data->m_is_imperial_beta = true;
	}
	if (!strcmp(packet->cheat_name, XOR("imperial dev"))) {
		data->m_is_imperial_dev = true;
	}

	// cheese crack
	else if (msg_->sequence_bytes == 321420420) {
		if (data) {
			data->m_networked_angle = (*(float*)&msg_data.section_number) / 69.f;
		}
		data->m_is_cheese_crack = true;
	}
	// kaaba crack
	else if (msg_->sequence_bytes == -858993664) {
		if (data) {
			data->m_networked_angle = math::NormalizedAngle(msg_data.section_number);
		}
		data->m_is_kaaba = true;
	}
	// dopium
	else if (msg_->xuid_low == 1338)
		data->m_is_dopium = true;
	// robertpaste
	else if (msg_->xuid_low == 4539731)
		data->m_is_robertpaste = true;
	else if (msg_->xuid_low == 2183)
		data->nap = true;
	else if (!strcmp(packet->cheat_name, XOR("novocheat"))) 
		data->novocheat = true;
	// pandora
	else if (msg_->xuid_low == 6969)
		data->m_is_pandora = true;
	// godhook
	else if (!strcmp(packet->cheat_name, XOR("g0db00k.indeed")) || !strcmp(packet->cheat_name, "h0dg00k.ind33d") || msg_->xuid_low == 666613333)
		data->m_is_godhook = true;
	// fade
	else if (msg_->xuid_low == 1701077350 || msg_->sequence_bytes == 24930)
		data->m_is_fade = true;


	m_oVoiceData(this, msg);
}



bool AllowPacket(int command) {
	if (!g_cl.m_processing || !g_csgo.m_engine->IsInGame() || g_cl.m_sent_cmds.empty())
		return true;

	for (int i = 0; i < g_cl.m_sent_cmds.size(); ++i) {
		int cmd = g_cl.m_sent_cmds.at(i);
		if (!cmd || cmd != command)
			continue;

		g_cl.m_sent_cmds.erase(g_cl.m_sent_cmds.begin() + i);
		return true;
	}

	return false;
}

void Hooks::PacketStart(int incoming_sequence, int outgoing_acknowledged) {
	if (AllowPacket(outgoing_acknowledged))
		return g_hooks.m_client_state.GetOldMethod< PacketStart_t >(CClientState::PACKETSTART)(this, incoming_sequence, outgoing_acknowledged);
}