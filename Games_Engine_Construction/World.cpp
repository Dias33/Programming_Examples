#include "World.h"
#include "Visualisation.h"
#include "Entity.h"
#include "PlayerEntity.h"
#include "EnemyEntity.h"
#include "EntityBullet.h"
#include <stdlib.h>
#include <string>

CWorld::CWorld()
{
	My_Bullets.assign(100, CEntityBullet());
}

CWorld::~CWorld()
{
	for (auto entity : My_Entity_Vector)
	{
		delete entity;
	}

	delete My_Visualisation;
}

void CWorld::Initialise(unsigned int screen_width, unsigned int screen_height)
{
	My_Visualisation = new Visualisation();
	
	My_Visualisation->Initialise(screen_width, screen_height);

}

void CWorld::LoadLevel()
{
	My_Visualisation->ClearScreen();

	Play_sound();

	My_Visualisation->CreateSprite("Claudius.png", Sprite_ID);

	My_Visualisation->CreateSprite("Laila.png", Sprite_ID);

	My_Visualisation->CreateSprite("Arrow.png", Sprite_ID);

	CPlayerEntity* newPlayer = new CPlayerEntity();

	newPlayer->Initialise("Laila.png", (My_Visualisation->Get_Screen_Width()/2), (My_Visualisation->Get_Screen_Height()/2), 24, 32, 64, 1);

	My_Entity_Vector.push_back(newPlayer);

	My_Player_Vector.push_back(newPlayer);

	for (int i = 0; i<10; i++)
	{
		CEnemyEntity* newEnemy = new CEnemyEntity();

		int plus_minus = rand() % 2;

		if (plus_minus = 0)
		{
			plus_minus = -1;
		}
		else
		{
			plus_minus = 1;
		}

		newEnemy->Initialise("Claudius.png", (rand()%My_Visualisation->Get_Screen_Width()+(rand()%201 * plus_minus)), (rand()%My_Visualisation->Get_Screen_Height()+(rand()%201*plus_minus)), 24, 32, 64, 0);

		My_Enemy_Vector.push_back(newEnemy);

		My_Entity_Vector.push_back(newEnemy);

	}
}



void CWorld::CheckForCollisions()
{
	for (auto entity : My_Entity_Vector)
	{
		if (entity->Get_Active() && entity->GetSide() != 0)
		{
			for (auto other_entity : My_Entity_Vector)
			{
				if (entity->IsEnemyOf(other_entity->GetSide()) && other_entity->Get_Active())
				{
					if (entity->Check_Collision(*other_entity))
					{
						entity->Take_Damage(*other_entity);
					}

				}
			}
		}
	}

	for (auto & bullet : My_Bullets)
	{
		if (bullet.Get_Active() && bullet.GetSide() != 0)
		{
			for (auto other_entity : My_Entity_Vector)
			{
				if (bullet.IsEnemyOf(other_entity->GetSide()) && other_entity->Get_Active())
				{
					if (bullet.Check_Collision(*other_entity))
					{
						bullet.Take_Damage(*other_entity);

						if (bullet.GetSide() == 1)
						{
							if (other_entity->Get_Health() < 0)

								My_Player_Vector.back()->Increase_Score();
						}

					}

				}
			}
		}
	}
}

void CWorld::Fire_Bullet(CEntity& this_entity)
{	

	for (auto & bullet : My_Bullets)
	{
		if (bullet.Get_Active() == false)
		{
			bullet.Initialise("Arrow.png", this_entity.Get_Current_Position_x(), this_entity.Get_Current_Position_y(), 12, 32, 48, 2);
			bullet.Update_Directions(this_entity.Get_Current_Direction_x(), this_entity.Get_Current_Direction_y());
			bullet.Set_Side(this_entity.GetSide());
			bullet.Set_Active(true);
			break;
		}

	}
}

void CWorld::Render_Score()
{
	std::string player_score = std::to_string(My_Player_Vector.back()->Get_Score());

	HAPI.RenderText(My_Visualisation->Get_Screen_Width() - 150, 50, HAPI_TColour(255, 0, 0), "Player Score:" + player_score, 18);
}

void CWorld::Render_Player_Health()
{
	std::string player_health = std::to_string(My_Player_Vector.back()->Get_Health());

	HAPI.RenderText(50, My_Visualisation->Get_Screen_Height() - 50, HAPI_TColour(255, 0, 0), "Player Health:" + player_health, 18);
}

void CWorld::Play_sound()
{
	HAPI_TStreamedMediaOptions sound_options;

	sound_options.loop = true;
	sound_options.volume = 2.f;

	unsigned int musicID = 1;

	if (!HAPI.PlayStreamedMedia("Background_Music.wav", sound_options,musicID))
	{	
	
	}

}

void CWorld::Run()
{
	if (HAPI.GetTime() - last_time_update > update_tick)
	{
		last_time_update = HAPI.GetTime();

		My_Visualisation->ClearScreen();

		for (auto entity : My_Player_Vector)
		{
			if (entity->Get_Active())
			{
				entity->Render(*My_Visualisation);

				entity->Update();

				if (entity->Get_Trigger_Bullet())
				{
					Fire_Bullet(*entity);

					HAPI.PlayStreamedMedia("Fire_Arrow.wav");
				}
			}
		}

		for (auto & enemy_entity : My_Enemy_Vector)
		{

			if (enemy_entity->Get_Active())
			{
				enemy_entity->Render(*My_Visualisation);

				for (auto player_entity : My_Player_Vector)
				{
					enemy_entity->Get_Other_Entity_Position(*player_entity);
				}

				enemy_entity->Update();

				if (enemy_entity->Get_Trigger_Bullet())
				{
					Fire_Bullet(*enemy_entity);
				}
			}

		}

		for (auto & bullet : My_Bullets)
		{
			if (bullet.Get_Active())
			{
				if (bullet.Check_Screen_Boundaries(*My_Visualisation))
				{
					bullet.Render(*My_Visualisation);

					bullet.Update();
				}
				else
				{
					bullet.Set_Active(false);
				}
			}

		}

		CheckForCollisions();
		
	}

	Render_Score();
	Render_Player_Health();

}