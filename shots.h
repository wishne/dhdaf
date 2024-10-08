#pragma once

class ShotRecord {
public:
	__forceinline ShotRecord() : m_i{}, m_target {}, m_record{}, m_time{}, m_lat{}, m_damage{}, m_range{}, m_pos{}, m_end_pos{}, m_matched{} {}

public:
	int m_i;
	Player* m_target;
	LagRecord* m_record;
	float      m_time, m_lat, m_damage, m_range;
	vec3_t     m_pos, m_end_pos;
	bool       m_matched;
};

class VisualImpactData_t {
public:
	vec3_t m_impact_pos, m_shoot_pos;
	int    m_tickbase;
	bool   m_ignore, m_hit_player;

public:
	__forceinline VisualImpactData_t(const vec3_t& impact_pos, const vec3_t& shoot_pos, int tickbase) :
		m_impact_pos{ impact_pos }, m_shoot_pos{ shoot_pos }, m_tickbase{ tickbase }, m_ignore{ false }, m_hit_player{ false } {}
};

class ImpactRecord {
public:
	__forceinline ImpactRecord() : m_spread{}, m_hurt{}, m_shot{}, m_pos{}, m_tick{} {}

public:
	bool        m_spread;
	bool        m_hurt;
	ShotRecord* m_shot;
	int         m_tick;
	vec3_t      m_pos;
};

class HitRecord {
public:
	__forceinline HitRecord() : m_impact{}, m_group{ -1 }, m_damage{} {}

public:
	ImpactRecord* m_impact;
	int           m_group;
	float         m_damage;
};

class Shots {
public:
	std::array< std::string, 8 > m_groups = {
	XOR("BODY"),
	XOR("HEAD"),
	XOR("CHEST"),
	XOR("STOMACH"),
	XOR("LEFT ARM"),
	XOR("RIGHT ARM"),
	XOR("LEFT LEG"),
	XOR("RIGHT LEG")
	};


	void OnShotFire(Player* target, float damage, LagRecord* record, vec3_t end_pos);
	void OnImpact(IGameEvent* evt);
	void OnHurt(IGameEvent* evt);

public:
	vec3_t                            m_impact;

	std::deque< ShotRecord >          m_shots;
	std::vector< VisualImpactData_t > m_vis_impacts;
	std::deque< ImpactRecord >        m_impacts;
	std::deque< HitRecord >           m_hits;
};

extern Shots g_shots;