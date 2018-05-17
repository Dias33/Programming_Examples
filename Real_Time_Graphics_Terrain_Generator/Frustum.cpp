#include "Frustum.h"


Frustum::Frustum(glm::mat4 projection, glm::mat4 view)
{
	projection_view = projection * view;

	projection_view = glm::transpose(projection_view);

	m_planes[EPlane::eLeft] = projection_view[3] + projection_view[0];
	m_planes[EPlane::eRight] = projection_view[3] - projection_view[0];
	m_planes[EPlane::eTop] = projection_view[3] - projection_view[1];
	m_planes[EPlane::eBottom] = projection_view[3] + projection_view[1];
	m_planes[EPlane::eNear] = projection_view[3] + projection_view[2];
	m_planes[EPlane::eFar] = projection_view[3] - projection_view[2];
}

bool Frustum::Point_in_Frustum(glm::vec3 point)
{
	bool ok = true;

	for (auto & planes : m_planes)
	{
		float distance = planes.x * point.x + planes.y * point.y + planes.z * point.z + planes.w;

		if (distance < 0)
		{
			ok = false;
		}
	}

	if (ok == false)
		return false;
	else
		return true;

}

Frustum::~Frustum()
{
}
