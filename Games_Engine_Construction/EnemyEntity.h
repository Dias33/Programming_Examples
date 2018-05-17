#pragma once
#include "Entity.h"

class CPlayerEntity;

class CEnemyEntity : public CEntity
{
public:
	CEnemyEntity();
	~CEnemyEntity();

	void Move_Toward_Destination();

	void Get_Other_Entity_Position(CPlayerEntity other_entity);

	void Attack();

	void Update() override final;

private:
	enum class EState{eStateMoving, eStateAttack};
	
	EState m_currentState;

	HAPISPACE::DWORD last_time_trigger{ 0 };

	float trigger_tick{ 750.f };

	int attack_range;

	int destination_x;
	int destination_y;
};

