#include "EnemyEntity.h"
#include "EntityBullet.h"
#include "PlayerEntity.h"



CEnemyEntity::CEnemyEntity()
{
	My_Side = 2;
	Health = 50;
	Damage = 5;
	attack_range = 150;
	speed = 1;
	m_currentState = EState::eStateMoving;
}


CEnemyEntity::~CEnemyEntity()
{
}

void CEnemyEntity::Move_Toward_Destination()
{
	if (my_pos_x < destination_x)
	{
		direction_x = 1;
   		direction_y = 0;
		row_ID = 3;

		my_pos_x += speed;
		previous_position_x = my_pos_x - speed;

		if (my_currentFrame > 6)
		{
			my_currentFrame = 2;
		}
		else
		{
			my_currentFrame++;
		}
	}
	
	if (my_pos_x > destination_x)
	{
		direction_x = -1;
		direction_y = 0;

		row_ID = 4;

		my_pos_x -= speed;
		previous_position_x = my_pos_x + speed;

		if (my_currentFrame > 6)
		{
			my_currentFrame = 2;
		}
		else
		{
			my_currentFrame++;
		}
	}

	if (my_pos_y < destination_y)
	{
		direction_x = 0;
		direction_y = 1;

		row_ID = 0;

		my_pos_y += speed;
		previous_position_y = my_pos_y - speed;

		if (my_currentFrame > 6)
		{
			my_currentFrame = 2;
		}
		else
		{
			my_currentFrame++;
		}
	}

	if (my_pos_y > destination_y)
	{
		direction_x = 0;
		direction_y = -1;

		row_ID = 1;

		my_pos_y -= speed;
		previous_position_y = my_pos_y + speed;

		if (my_currentFrame > 6)
		{
			my_currentFrame = 2;
		}
		else
		{
			my_currentFrame++;
		}
	}
}


void CEnemyEntity::Get_Other_Entity_Position(CPlayerEntity other_entity)
{
	destination_x = other_entity.Get_Current_Position_x();
	destination_y = other_entity.Get_Current_Position_y();
}

void CEnemyEntity::Attack()
{
	
	if (HAPI.GetTime() - last_time_trigger > trigger_tick)
	{
		trigger_bullet = true;
		last_time_trigger = HAPI.GetTime();
	}
	
}

void CEnemyEntity::Update()
{
	Die();

	switch (m_currentState)
	{
	
	case EState::eStateMoving:

		Move_Toward_Destination();

		trigger_bullet = false;


		if ( ( (my_pos_x > destination_x - attack_range) || (destination_x < my_pos_x - attack_range) ) 
			&&
			 ( (my_pos_y > destination_y - attack_range) || (destination_y < my_pos_y - attack_range) )
		   )
		{
			m_currentState = EState::eStateAttack;
		}

		break;
	case EState::eStateAttack:
		
		Attack();

		m_currentState = EState::eStateMoving;

		break;

	}

	Update_My_Rectangle();
}
