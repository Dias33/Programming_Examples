#include "Visualisation.h"
#include "Sprite.h"

Visualisation::Visualisation()
{

}


Visualisation::~Visualisation()
{
	for (auto p : m_spriteVector)
	{
		delete p;
	}
}

void Visualisation::Initialise(unsigned int screen_width_, unsigned int screen_height_)
{
	screen_width = screen_width_;

	screen_height = screen_height_;

	screen_rectangle.Update_Rectangle(0, screen_width, 0, screen_height);

	if (!HAPI.Initialise(screen_width, screen_height))
		return;

	screen = HAPI.GetScreenPointer();

	SetColour(0, 0, 0);
}

void Visualisation::ClearScreen()
{
	for (unsigned int y = 0; y < screen_height; y++)
	{
		for (unsigned int x = 0; x < screen_width; x++)
		{
			SetPixel(x, y, screen_colour, screen, screen_width);
		}
	}

}

int Visualisation::Get_Screen_Width()
{
	return screen_width;
}

int Visualisation::Get_Screen_Height()
{
	return screen_height;
}

const CRectangle Visualisation::Get_Rectangle(int Sprite_ID_) const
{
	return m_spriteVector[Sprite_ID_]->Get_Rectangle();
}

bool Visualisation::CreateSprite(std::string File_Name, int &Sprite_ID)
{
	Sprite *My_Sprite = new Sprite(File_Name);

	if (!My_Sprite->Load())
	{
		delete My_Sprite;
		return false;
	}

	m_spriteVector.push_back(My_Sprite);

	Sprite_ID =(int) m_spriteVector.size() - 1;

	return true;

}

const void Visualisation::RenderSprite(int Sprite_ID, int pos_x, int pos_y, int Current_Frame, int Max_Frames, int frame_width, int frame_height, int row_ID) const
{
	m_spriteVector[Sprite_ID]->Render(pos_x, pos_y, screen, screen_width, screen_height, screen_rectangle, Current_Frame , Max_Frames, frame_width, frame_height, row_ID);
}


void Visualisation::SetColour(int r, int b, int g)
{
	screen_colour.red = r;
	screen_colour.blue = b;
	screen_colour.green = g;

}

void Visualisation::SetPixel(unsigned int x, unsigned int y, HAPI_TColour col, BYTE * screen, unsigned int width)
{
	unsigned int offset = (y*width + x) * 4;
	memcpy(screen + offset, &col, sizeof(col));
}
