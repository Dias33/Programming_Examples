#pragma once
#include "Entity.h"

class CPlayerEntity : public CEntity
{
public:
	CPlayerEntity();
	~CPlayerEntity();

	void Increase_Score();

	int Get_Score();

	void Update() override final;

private:
	
	HAPISPACE::DWORD last_time_trigger{ 0 };

	float trigger_tick{ 250.f };

	int score;

};

