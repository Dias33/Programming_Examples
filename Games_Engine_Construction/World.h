#pragma once

#include <HAPI_lib.h>

class Visualisation;
class CEntity;
class CPlayerEntity;
class CEnemyEntity;
class CEntityBullet;

class CWorld

{
public:

	CWorld();

	~CWorld();


	void Initialise(unsigned int screen_width, unsigned int screen_height);

	void LoadLevel();

	void CheckForCollisions();

	void Fire_Bullet(CEntity& this_entity);

	void Render_Score();

	void Render_Player_Health();

	void Play_sound();

	void Run();

private:

	Visualisation *My_Visualisation{nullptr};
	
	int Sprite_ID = 0;

	std::vector <CEntity*> My_Entity_Vector;

	std::vector <CPlayerEntity*> My_Player_Vector;

	std::vector <CEnemyEntity*> My_Enemy_Vector;

	std::vector <CEntityBullet> My_Bullets;

	HAPISPACE::DWORD last_time_update{ 0 };

	float update_tick{ 35.f };
	
};

