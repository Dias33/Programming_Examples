#include "PlayerEntity.h"
#include "EntityBullet.h"



CPlayerEntity::CPlayerEntity()
{
	My_Side = 1;
	Health = 300;
	Damage = 0;
	speed = 7;
	score = 0;
}


CPlayerEntity::~CPlayerEntity()
{
}

void CPlayerEntity::Increase_Score()
{
	score++;
}

int CPlayerEntity::Get_Score()
{
	return score;
}


void CPlayerEntity::Update()
{
	//Movement

	const HAPI_TKeyboardData& keyData = HAPI.GetKeyboardData();

	const HAPI_TControllerData& controller_data = HAPI.GetControllerData(0);

	if (controller_data.isAttached)
	{
		int X_Right_Thumb = controller_data.analogueButtons[HK_ANALOGUE_RIGHT_THUMB_X];
		int Y_Right_Thumb = controller_data.analogueButtons[HK_ANALOGUE_RIGHT_THUMB_Y];


		if (controller_data.digitalButtons[HK_DIGITAL_DPAD_DOWN] || Y_Right_Thumb<-HK_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			my_pos_y += speed;
			previous_position_y = my_pos_y - speed;
			row_ID = 0;
			direction_y = 1;
			direction_x = 0;

			if (my_currentFrame > 6)
			{
				my_currentFrame = 2;
			}
			else
			{
				my_currentFrame++;
			}

		}

		if (controller_data.digitalButtons[HK_DIGITAL_DPAD_UP] || Y_Right_Thumb>HK_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			my_pos_y -= speed;
			previous_position_y = my_pos_y + speed;
			row_ID = 1;
			direction_y = -1;
			direction_x = 0;

			if (my_currentFrame > 6)
			{
				my_currentFrame = 2;
			}
			else
			{
				my_currentFrame++;
			}
		}

		if (controller_data.digitalButtons[HK_DIGITAL_DPAD_LEFT] || X_Right_Thumb<-HK_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			my_pos_x -= speed;
			previous_position_x = my_pos_x + speed;
			row_ID = 2;
			direction_x = -1;
			direction_y = 0;

			if (my_currentFrame > 6)
			{
				my_currentFrame = 2;
			}
			else
			{
				my_currentFrame++;
			}
		}

		if (controller_data.digitalButtons[HK_DIGITAL_DPAD_RIGHT] || X_Right_Thumb>HK_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			my_pos_x += speed;
			previous_position_x = my_pos_x - speed;
			row_ID = 3;
			direction_x = 1;
			direction_y = 0;

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


	//Keyboard Controls

	if ( ( (controller_data.digitalButtons[HK_DIGITAL_X]) || (keyData.scanCode['J']) ) && (HAPI.GetTime() - last_time_trigger > trigger_tick))
	{
		trigger_bullet = true;
		last_time_trigger = HAPI.GetTime();
	}
	else
	{
		trigger_bullet = false;
	}

	if (keyData.scanCode['W'])
	{
		my_pos_y -= speed;
		previous_position_y = my_pos_y + speed;
		row_ID = 1;
		direction_y = -1;
		direction_x = 0;

		if (my_currentFrame > 6)
		{
			my_currentFrame = 2;
		}
		else
		{
			my_currentFrame++;
		}
	}

	if (keyData.scanCode['A'])
	{

		my_pos_x -= speed;
		previous_position_x = my_pos_x + speed;
		row_ID = 2;
		direction_x = -1;
		direction_y = 0;

		if (my_currentFrame > 6)
		{
			my_currentFrame = 2;
		}
		else
		{
			my_currentFrame++;
		}
	}

	if (keyData.scanCode['S'])
	{
		my_pos_y += speed;
		previous_position_y = my_pos_y - speed;
		row_ID = 0;
		direction_y = 1;
		direction_x = 0;

		if (my_currentFrame > 6)
		{
			my_currentFrame = 2;
		}
		else
		{
			my_currentFrame++;
		}
	}

	if (keyData.scanCode['D'])
	{
		my_pos_x += speed;
		previous_position_x = my_pos_x - speed;
		row_ID = 3;
		direction_x = 1;
		direction_y = 0;

		if (my_currentFrame > 6)
		{
			my_currentFrame = 2;
		}
		else
		{
			my_currentFrame++;
		}
	}

	Die();

	Update_My_Rectangle();
}
