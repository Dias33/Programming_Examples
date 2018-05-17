#pragma once
#include <HAPI_lib.h>
using namespace HAPISPACE;

class CRectangle
{
	public:

	CRectangle();

	~CRectangle();

	int Width() const;

	int Height() const;

	void Update_Rectangle(int left_, int right_, int top_, int bottom_);
	
	void Translate(int posX,int posY);

	void ClipTo(const CRectangle &other);

	bool CompletelyInside(const CRectangle &other);

	bool CompletelyOutside(const CRectangle &other);

	bool Check_Collision(const CRectangle &other);

	bool FindCenter(const CRectangle &other);

	int left{ 0 };
	
	int right{ 0 };
	
	int top{ 0 };
	
	int bottom{ 0 };


};

