#pragma once
#include <HAPI_lib.h>
#include <string>
#include "Rectangle.h"


using namespace HAPISPACE;

class Sprite 
{
public:
	
	Sprite(std::string file_name);

	~Sprite();

	bool Load();

	void Render(int x_position, int y_position, BYTE* screen, unsigned int screen_width, unsigned int screen_height, CRectangle screen_rectangle, int Current_Frame, int Max_Frames, int frame_width_, int frame_height_, int row_ID_);
	
	void Blit(BYTE* screen, unsigned int screen_width);

	const CRectangle Get_Rectangle() const;

	void Blit_Transparent(BYTE* screen, unsigned int screen_width, unsigned int screen_height, unsigned int sprite_width_, unsigned int sprite_height_);

	void ClipBlit(BYTE* screen, unsigned int screen_width, unsigned int screen_height, CRectangle screen_rectangle);

	void Blit_Transparent_With_Clipping(BYTE* screen, unsigned int screen_width, int Clipped_Width, int Clipped_Height, int clippedleft, int clippedtop);


protected:
	
	std::string filename;

	BYTE *spriteData;
	unsigned int sprite_width;
	unsigned int sprite_height;

	int row_ID;
	int frame_width;
	int frame_height;
	int Current_Frame_;
	int Max_Frames_;

	CRectangle sprite_rectangle;

	int current_position_x;
	int current_position_y;
};

