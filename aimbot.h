#pragma once

struct SpreadSeeds {
	bool initialized;
	float r1, r2, r3, r4;
	SpreadSeeds() { initialized = false; };
};

enum HitscanMode : int {
	NORMAL = 0,
	LETHAL = 1,
	LETHAL2 = 2,
	PREFER = 3
};

enum PointMode : int {
	CENTER = 0,
	TOP = 1,
	SIDE = 2,
};

struct AnimationBackup_t {
	vec3_t           m_origin, m_abs_origin;
	vec3_t           m_velocity, m_abs_velocity;
	int              m_flags, m_eflags;
	float            m_duck, m_body, m_time;
	C_AnimationLayer m_layers[13];
};

struct PointData_t {
	vec3_t m_pos;
	int    m_mode, m_damage, m_hitgroup;
	bool   m_pen;

	__forceinline PointData_t() { m_hitgroup = -1; m_pen = false; m_pos = { 0, 0, 0 }; m_mode = PointMode::CENTER; m_damage = 0; }

	__forceinline PointData_t(vec3_t pos, int mode) { m_hitgroup = -1; m_pen = false; m_pos = pos; m_mode = mode; m_damage = 0; }
};

struct HitscanData_t {
	int    m_hitbox;
	float  m_damage;
	PointData_t m_point;

	__forceinline HitscanData_t() : m_hitbox{ 0 }, m_damage{ 0.f }, m_point{} {}
};

struct HitscanBox_t {
	int         m_index;
	int         m_mode;

	__forceinline bool operator==(const HitscanBox_t& c) const {
		return m_index == c.m_index && m_mode == c.m_mode;
	}
};

enum CheatIds {
	rax = 1,
	minty,
	family,
	cheese,
	kaaba,
	robert,
	fade,
	dopium,
	pandora,
	godhook
};

class AimPlayer {
public:
	using records_t = std::deque< LagRecord >;
	using hitboxcan_t = stdpp::unique_vector< HitscanBox_t >;

public:
	int m_cheat_id;

	// essential data.
	Player* m_player;
	float	  m_spawn;
	records_t m_records;

	// aimbot data.
	hitboxcan_t m_hitboxes;

	// resolve data.
	int       m_shots;
	int       m_missed_shots;

	int       m_last_air_stand_mode;

	float     m_body_update, m_walk_body, m_last_body, m_last_move_time;
	bool      m_moved, m_standing, m_can_predict;

	bool      m_failed_flick;
	int       m_update_count;
	float     m_lby_delta;
	vec3_t origin_walk;  // origin walk vector
	float     m_target_time;

	int m_ff_ticks;

	int m_ff_index;
	int m_stand_index;
	int m_stand2_index;
	int m_body_index;
	int m_air_index;

	float m_ff_angle;

	float m_resolved_angle;

	float m_move_away;


	/*network*/
	float m_networked_angle = 1337.f;   
	bool m_is_kaaba = false;                   // kaaba flag
	bool m_is_cheese_crack = false;            // cheese crack flag
	bool leak_familysadface = false;                 // webster flag
	bool real_familyhappyface = false;                 // webster flag
	bool m_is_amigo = false;                 // webster flag
	bool m_is_imperial_beta = false;                 // webster flag
	bool m_is_imperial_dev = false;                 // webster flag
	bool m_is_dopium = false;                  // dopium flag
	bool m_is_robertpaste = false;             // robertpaste flag
	bool nap = false;             // robertpaste flag
	bool novocheat = false;             // robertpaste flag
	bool m_is_fade = false;                    // fade flag
	bool m_is_godhook = false;                 // godhook flag
	bool m_is_pandora = false;                 // pandora flag


public:
	void HandleLagcompensation();

	void CorrectOldStateData(Player* player, LagRecord* record, LagRecord* previous);
	void SyncPlayerVelocity(Player* player, LagRecord* record, LagRecord* previous);

	void UpdateAnimations(LagRecord* record);
	void OnNetUpdate(Player* player);
	void OnRoundStart(Player* player);
	void SetupHitboxes(LagRecord* record, bool history);
	bool SetupHitboxPoints(LagRecord* record, BoneArray* bones, int index, std::vector< PointData_t >& points);
	bool GetBestAimPosition(vec3_t& aim, float& damage, int& hitgroup, LagRecord* record);

public:
	void reset() {
		m_player = nullptr;
		m_last_move_time = m_walk_body = m_spawn = 0.f;
		m_shots = 0;
		m_update_count = m_missed_shots = 0;
		m_failed_flick = m_can_predict = false;
		m_networked_angle = 0.f;
		m_is_kaaba = false;
		m_is_cheese_crack = false;
		m_is_amigo = false;
		m_is_imperial_beta = false;
		m_is_imperial_dev = false;
		leak_familysadface = false;
		real_familyhappyface = false;
		m_is_dopium = false;
		nap = false;
		novocheat = false;
		m_is_robertpaste = false;
		m_is_fade = false;
		m_is_godhook = false;
		m_is_pandora = false;
		m_last_air_stand_mode = 0;
		m_target_time = 0;

		m_records.clear();
		m_hitboxes.clear();
	}
};

class Aimbot {
private:
	struct target_t {
		Player* m_player;
		AimPlayer* m_data;
	};

	struct knife_target_t {
		target_t  m_target;
		LagRecord m_record;
	};

	struct table_t {
		uint8_t swing[2][2][2]; // [ first ][ armor ][ back ]
		uint8_t stab[2][2];		  // [ armor ][ back ]
	};

	const table_t m_knife_dmg{ { { { 25, 90 }, { 21, 76 } }, { { 40, 90 }, { 34, 76 } } }, { { 65, 180 }, { 55, 153 } } };

	std::array< ang_t, 12 > m_knife_ang{
		ang_t{ 0.f, 0.f, 0.f }, ang_t{ 0.f, -90.f, 0.f }, ang_t{ 0.f, 90.f, 0.f }, ang_t{ 0.f, 180.f, 0.f },
		ang_t{ -80.f, 0.f, 0.f }, ang_t{ -80.f, -90.f, 0.f }, ang_t{ -80.f, 90.f, 0.f }, ang_t{ -80.f, 180.f, 0.f },
		ang_t{ 80.f, 0.f, 0.f }, ang_t{ 80.f, -90.f, 0.f }, ang_t{ 80.f, 90.f, 0.f }, ang_t{ 80.f, 180.f, 0.f }
	};

public:
	std::array<SpreadSeeds, 256> m_spread_seeds;

	std::array< AimPlayer, 64 > m_players;
	std::vector< AimPlayer* >   m_targets;

	BackupRecord m_backup[64];

	// target selection stuff.
	float m_best_dist;
	float m_best_fov;
	float m_best_damage;
	int   m_best_hp;
	float m_best_lag;
	float m_best_height;

	// found target stuff.
	Player* m_target;
	ang_t      m_angle;
	vec3_t     m_aim;
	float      m_damage;
	int        m_hitgroup;
	LagRecord* m_record;

	// fake latency stuff.
	bool       m_fake_latency;

	bool m_stop, m_override, m_baim;

public:
	__forceinline void reset() {
		// reset aimbot data.
		init();

		// reset all players data.
		for (auto& p : m_players)
			p.reset();
	}

	__forceinline bool IsValidTarget(Player* player) {
		if (!player)
			return false;

		if (!player->IsPlayer())
			return false;

		if (!player->alive())
			return false;

		if (player->dormant())
			return false;

		if (player->m_bIsLocalPlayer())
			return false;

		if (!player->enemy(g_cl.m_local))
			return false;

		return true;
	}

public:
	// aimbot.
	void init();
	void UpdateSettings();
	void StripAttack();
	void think();
	void find();
	void InitializeSeed(int seed);
	bool CheckHitchance(Player* player, const vec3_t& src, const vec3_t& enda, const int hitgroup);
	bool SelectTarget(LagRecord* record, const vec3_t& aim, float damage);
	void apply();
	void NoSpread();

	// knifebot.
	void knife();
	bool CanKnife(LagRecord* record, ang_t angle, bool& stab);
	bool KnifeTrace(vec3_t dir, bool stab, CGameTrace* trace, Player* target);
	bool KnifeIsBehind(LagRecord* record);
};

extern Aimbot g_aimbot;