#pragma once
#include <HAPI_lib.h>
#include <string>
#include "Rectangle.h"

using namespace HAPISPACE;

class CRectangle;
class Sprite;

class Visualisation
{
public:
	
	Visualisation();
	
	~Visualisation();

	void Initialise(unsigned int screen_width_, unsigned int screen_height_);

	void ClearScreen();

	int Get_Screen_Width();

	int Get_Screen_Height();

	const CRectangle Get_Rectangle(int Sprite_ID_) const;

	bool CreateSprite(std::string File_Name, int &Sprite_ID);

	const void RenderSprite(int Sprite_ID, int pos_x, int pos_y, int Current_Frame, int Max_Frames, int frame_width, int frame_height, int row_ID) const;

	void SetColour(int r, int b, int g);

	void SetPixel(unsigned int x, unsigned int y, HAPI_TColour col, BYTE *screen, unsigned int width);

protected:

	HAPI_TColour screen_colour;
	unsigned int screen_width;
	unsigned int screen_height;
	BYTE *screen;

	CRectangle screen_rectangle;

	std::vector<Sprite*> m_spriteVector;

};

