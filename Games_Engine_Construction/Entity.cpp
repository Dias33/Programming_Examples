#include "Entity.h"

CEntity::~CEntity()
{
}

void CEntity::Initialise(const std::string & spriteName, int posX, int posY, int numFrames, int frame_width_, int frame_height_, int Sprite_ID_)
{
	my_spriteName = spriteName;

	my_pos_x = posX;
	my_pos_y = posY;
	previous_position_x = my_pos_x;
	previous_position_y = my_pos_y;

	my_numFrames = numFrames;
	my_currentFrame = 0;
	frame_width = frame_width_;
	frame_height = frame_height_;

	Sprite_ID = Sprite_ID_;
	trigger_bullet = false;
}

void CEntity::Render(const Visualisation & viz)
{
	viz.RenderSprite(Sprite_ID, my_pos_x, my_pos_y, my_currentFrame, my_numFrames, frame_width, frame_height, row_ID);

	My_Rectangle = viz.Get_Rectangle(Sprite_ID);
}

bool CEntity::Die()
{
	if (Health <= 0)
	{
		active = false;
		return true;
	}

	return false;
}


bool CEntity::Check_Screen_Boundaries(Visualisation & viz)
{
	if (my_pos_x > viz.Get_Screen_Width() || my_pos_x < 0)
	{
		return false;
	}

	if (my_pos_y > viz.Get_Screen_Height() || my_pos_y < 0)
	{
		return false;
	}

	return true;
}

bool CEntity::Check_Collision(CEntity & other_entity)
{
	if (My_Rectangle.Check_Collision(other_entity.My_Rectangle) )
	{
		return true;
	}
	else
	{
		return false;
	}

}

void CEntity::Take_Damage(CEntity& other_entity)
{
	Health -= other_entity.Damage;
	other_entity.Health -= Damage;

	if (Health > 0 && other_entity.Health > 0)
	{
		my_pos_x = previous_position_x;
		my_pos_y = previous_position_y;

		other_entity.my_pos_x = other_entity.previous_position_x;
		other_entity.my_pos_y = other_entity.previous_position_y;
	}
}

void CEntity::Update_My_Rectangle()
{
	My_Rectangle.left = my_pos_x;
	My_Rectangle.right = My_Rectangle.left + frame_width;

	My_Rectangle.top = my_pos_y;
	My_Rectangle.bottom = My_Rectangle.top + frame_height;
}

int CEntity::Get_Health()
{
	return Health;
}



bool CEntity::Get_Trigger_Bullet()
{
	return trigger_bullet;
}

int CEntity::Get_Current_Position_x()
{
	return my_pos_x;
}

int CEntity::Get_Current_Position_y()
{
	return my_pos_y;
}

int CEntity::Get_Current_Direction_x()
{
	return direction_x;
}

int CEntity::Get_Current_Direction_y()
{
	return direction_y;
}

bool CEntity::Get_Active()
{
	return active;
}

int CEntity::GetSide()
{
	return My_Side;
}

bool CEntity::IsEnemyOf(int other_side)
{
	if (My_Side != other_side)
	{
		return true;
	}
	else
	{
		return false;
	}
}
