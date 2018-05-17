#include "Rectangle.h"
#include <algorithm>

CRectangle::CRectangle()
{
}

CRectangle::~CRectangle()
{
}

int CRectangle::Height() const
{
	return bottom - top;
}

void CRectangle::Update_Rectangle(int left_, int right_, int top_, int bottom_)
{
	left = left_;
	right = right_;
	top = top_;
	bottom = bottom_;
}

int CRectangle::Width() const
{
	return right - left;

}

void CRectangle::Translate(int posX,int posY)
{
	left += posX;
	right += posX;
	top += posY;
	bottom += posY;

}

void CRectangle::ClipTo(const CRectangle & other)
{
	left = std::max(left, other.left);
	right = std::min(right, other.right);
	top = std::max(top, other.top);
	bottom = std::min(bottom, other.bottom);
}

bool CRectangle::CompletelyInside(const CRectangle & other)
{
	if (top > other.top && left > other.left && right < other.right && bottom < other.bottom)
	{
		return true;
	}
	else
		return false;
}

bool CRectangle::CompletelyOutside(const CRectangle & other)
{
	if (right < other.left || bottom < other.top || top > other.bottom || left > other.right)
	{
		return true;
	}
	else
		return false;
}

bool CRectangle::Check_Collision(const CRectangle & other)
{
	if ( ( (right < other.left) || (left > other.right) )
		||
		 ( (bottom < other.top) || (top > other.bottom) )
	   )
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CRectangle::FindCenter(const CRectangle & other)
{
		return true;

}



