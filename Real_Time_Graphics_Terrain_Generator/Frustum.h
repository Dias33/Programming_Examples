#pragma once
#include <glm/glm.hpp>
#include "Terrain_Mesh.h"

enum EPlane
{
	eLeft = 0,
	eRight = 1,
	eTop = 2,
	eBottom = 3,
	eNear = 4,
	eFar = 5
};

	/*
	* Class to create a Frustum in order to determine the visibility of a patch. 
	*/

class Frustum
{
public:

	/*
	* Constructor for the Frustum.
	* Inputs: the projection and view matrices of the camera.
	*/

	Frustum(glm::mat4 projection, glm::mat4 view);

	/*
	* Function to check if the given position is in the Frustum.
	* Inputs: a glm::vec3 position.
	*/

	bool Point_in_Frustum(glm::vec3 point);

	~Frustum();

private:

	glm::mat4 projection_view;

	glm::vec4 m_planes[6];

};

