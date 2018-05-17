#include "EntityBullet.h"

CEntityBullet::CEntityBullet()
{
	active = false;
	My_Side = 0;
	Health = 1;
	original_health = Health;
	Damage = 15;
	speed = 15;
}


CEntityBullet::~CEntityBullet()
{
}

void CEntityBullet::Update_Directions(int direction_x_, int direction_y_)
{
	direction_x = direction_x_;
	direction_y = direction_y_;
}

void CEntityBullet::Set_Active(bool activity)
{
	active = activity;
}

void CEntityBullet::Set_Side(int other_side)
{
	My_Side = other_side;
}

void CEntityBullet::Update()
{
	if (direction_x > 0)
	{
		row_ID = 2;
	}
	else
	{
		if (direction_x < 0)
		{
			row_ID = 1;
		}
	}

	if (direction_y > 0)
	{
		row_ID = 0;
	}
	else
	{
		if (direction_y < 0)
		{
			row_ID = 3;
		}
	}

	my_pos_x += direction_x * speed;
	my_pos_y += direction_y * speed;

	if (Die())
	{
		Health = original_health;
	}
	


	Update_My_Rectangle();
}
