#pragma once
#include<string>
#include "Visualisation.h"

class Visualisation;

class CEntity

{
public:

	virtual ~CEntity();

	void Initialise(const std::string &spriteName, int posX, int posY, int numFrames, int frame_width_, int frame_height_,int Sprite_ID_);

	void Render(const Visualisation& viz);

	bool Die();

	bool Check_Screen_Boundaries(Visualisation& viz);

	bool Check_Collision(CEntity& other_entity);

	void Take_Damage(CEntity& other_entity);

	void Update_My_Rectangle();

	int Get_Health();

	bool Get_Trigger_Bullet();

	int Get_Current_Position_x();

	int Get_Current_Position_y();

	int Get_Current_Direction_x();

	int Get_Current_Direction_y();

	bool Get_Active();

	int GetSide();

	bool IsEnemyOf(int other_side);

	virtual void Update() = 0;

protected:

	int my_pos_x;
	int my_pos_y;

	int direction_x{ 0 };
	int direction_y{ 0 };

	int previous_position_x;
	int previous_position_y;

	CRectangle My_Rectangle;

	int Sprite_ID;

	int frame_width;
	int frame_height;
	int my_numFrames;
	int row_ID{ 0 };
	int my_currentFrame{0};
	std::string my_spriteName;

	bool active{ true };
	int My_Side;
	int Health;
	int Damage;
	int speed;
	bool trigger_bullet;

	enum class ESide
	{
		eSideNeutral = 0,
		eSidePlayer = 1,
		eSideEnemy = 2
	};

};

