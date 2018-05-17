#pragma once
#include "Entity.h"

class CEntityBullet : public CEntity
{
public:

	CEntityBullet();

	~CEntityBullet();

	void Update_Directions(int direction_x_, int direction_y_);

	void Set_Active(bool activity);

	void Set_Side(int other_side);

	void Update() override final;

private:

	int original_health;

};

