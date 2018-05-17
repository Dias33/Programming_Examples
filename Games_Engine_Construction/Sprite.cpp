#include "Sprite.h"



Sprite::Sprite(std::string file_name)
{	
	filename = file_name;
}


Sprite::~Sprite()
{
	delete spriteData;
}

bool Sprite::Load()
{
	if (!HAPI.LoadTexture(filename, &spriteData, sprite_width, sprite_height))
	{
		HAPI.UserMessage("File Not Found!", "Error");
		return false;
	}

	sprite_rectangle.Update_Rectangle(0, sprite_width, 0, sprite_height);

	return true;
}

void Sprite::Render(int x_position, int y_position, BYTE* screen, unsigned int screen_width, unsigned int screen_height, CRectangle screen_rectangle,  int Current_Frame , int Max_Frames, int frame_width_, int frame_height_, int row_ID_)
{
	row_ID = row_ID_;
	frame_width = frame_width_;
	frame_height = frame_height_;
	Current_Frame_ = Current_Frame;
	Max_Frames_ = Max_Frames;

	current_position_x = x_position;
	current_position_y = y_position;

	ClipBlit(screen, screen_width, screen_height, screen_rectangle);
}

void Sprite::Blit(BYTE* screen, unsigned int screen_width)
{
	BYTE *destPointer = screen + (current_position_x + current_position_y*screen_width) * 4;
	BYTE *sourcePointer = spriteData;

	for (unsigned int y = 0; y < sprite_height; y++)
	{
		memcpy(destPointer, sourcePointer, sprite_width * 4);

		destPointer += screen_width * 4;

		sourcePointer += sprite_width * 4;

	}
}

const CRectangle Sprite::Get_Rectangle() const
{
	return sprite_rectangle;
}

void Sprite::Blit_Transparent(BYTE* screen, unsigned int screen_width, unsigned int screen_height, unsigned int sprite_width_, unsigned int sprite_height_)
{
	BYTE *destPointer = screen + (current_position_x + current_position_y*screen_width) * 4;
	BYTE *sourcePointer = spriteData;

	int endOfLineDestIncrement = (screen_width - sprite_width_) * 4;

	if (
		((current_position_x + sprite_width_ / 2) > screen_width / 2 - 15 && (current_position_x + sprite_width_ / 2) < screen_width / 2 + 15)
		&&
		((current_position_y + sprite_height_ / 2) > screen_height / 2 - 15 && (current_position_y + sprite_height_ / 2) < screen_height / 2 + 15)
		)
	{
		WORD left{ 30000 };
		WORD right{ 65535 };
		HAPI.SetControllerRumble(0, left, right);
	}
	else
	{
		HAPI.SetControllerRumble(0, 0, 0);
	}

	for (unsigned int y = 0; y < sprite_height_; y++)
	{

		for (unsigned int x = 0; x < sprite_width_; x++)
		{
			BYTE alpha = sourcePointer[3];

			if (alpha == 0)
			{

			}

			else
			{
				BYTE blue = sourcePointer[0];
				BYTE green = sourcePointer[1];
				BYTE red = sourcePointer[2];

				if (alpha == 255)
				{
					destPointer[0] = blue;
					destPointer[1] = green;
					destPointer[2] = red;

				}

				else
				{

					float mod = alpha / 255.0f;

					destPointer[0] = (BYTE)(mod*blue + (1.0f - mod)*destPointer[0]);
					destPointer[1] = (BYTE)(mod*green + (1.0f - mod)*destPointer[1]);
					destPointer[2] = (BYTE)(mod*red + (1.0f - mod)*destPointer[2]);
				}
			}
			destPointer += 4;
			sourcePointer += 4;
		}

		destPointer += endOfLineDestIncrement;

	}
}

void Sprite::ClipBlit(BYTE* screen, unsigned int screen_width, unsigned int screen_height, CRectangle screen_rectangle)
{
	CRectangle clippedRect;

	if (Max_Frames_ != 0)
	{	
		CRectangle frame;

		frame.left = Current_Frame_* frame_width;
		frame.right = frame.left + frame_width-1;
		frame.top = row_ID * frame_height;
		frame.bottom = frame.top + frame_height-1;

		clippedRect.Update_Rectangle(0, frame.Width(), 0, frame.Height());
	}

	else
	{
		clippedRect = sprite_rectangle;
	}


	clippedRect.Translate(current_position_x, current_position_y);

	if (clippedRect.CompletelyOutside(screen_rectangle))
	{

	}

	else
	{
		clippedRect.ClipTo(screen_rectangle);

		clippedRect.Translate(-current_position_x, -current_position_y);

		if (Max_Frames_ != 0)
		{
			clippedRect.Translate(Current_Frame_*frame_width, row_ID*frame_height);
		}

		if (current_position_x < 0)
		{
			current_position_x = 0;
		}
		if (current_position_y < 0)
		{
			current_position_y = 0;
		}

		Blit_Transparent_With_Clipping(screen, screen_width, clippedRect.Width(), clippedRect.Height(), clippedRect.left, clippedRect.top);

	}
	
}


void Sprite::Blit_Transparent_With_Clipping(BYTE* screen, unsigned int screen_width, int Clipped_Width, int Clipped_Height, int clippedleft, int clippedtop)
{
	BYTE *destPointer = screen + (current_position_x + current_position_y*screen_width) * 4;
	BYTE *sourcePointer = spriteData + (clippedleft + clippedtop * sprite_width) * 4;

	int endOfLineDestIncrement = (screen_width - Clipped_Width) * 4;
	int endOfLineSourceIncrement = (sprite_width - Clipped_Width) * 4;

	for (int y = 0; y < Clipped_Height; y++)
	{

		for (int x = 0; x < Clipped_Width; x++)
		{
			BYTE alpha = sourcePointer[3];

			if (alpha == 0)
			{
			}

			else
			{
				BYTE blue = sourcePointer[0];
				BYTE green = sourcePointer[1];
				BYTE red = sourcePointer[2];

				if (alpha == 255)
				{
					destPointer[0] = blue;
					destPointer[1] = green;
					destPointer[2] = red;

				}

				else
				{

					float mod = alpha / 255.0f;

					destPointer[0] = (BYTE)(mod*blue + (1.0f - mod)*destPointer[0]);
					destPointer[1] = (BYTE)(mod*green + (1.0f - mod)*destPointer[1]);
					destPointer[2] = (BYTE)(mod*red + (1.0f - mod)*destPointer[2]);
				}
			}
			destPointer += 4;
			sourcePointer += 4;
		}

		destPointer += endOfLineDestIncrement;
		sourcePointer += endOfLineSourceIncrement;

	}
}