#include "Terrain_Mesh.h"
#include <cassert>
#include "PerlinNoise.h"
#include "Frustum.h"

Terrain_Mesh::Terrain_Mesh()
{
}

Terrain_Mesh::Terrain_Mesh(int subU_, int subV_)
{
	number_of_vertices = (subU_ + 1)*(subV_ + 1);
	number_of_quads = subU_ * subV_;
	number_of_triangles = number_of_quads * 2;
	number_of_elements = 3 * number_of_triangles;

	subU = subU_;
	subV = subV_;

	positions.resize(number_of_vertices);
	normals.resize(number_of_vertices);
	elements.reserve(number_of_elements);
	patch_elements.reserve(number_of_elements);
	UVcoordinates.resize(number_of_vertices);
}

void Terrain_Mesh::Generate_Elements()
{
	bool odd = true;

	for (int n = 0; n < subV; n++)
	{
		if (n % 2 == 0)
		{
			odd = true;
		}
		else
		{
			odd = false;
		}

		for (int m = 0; m < subU; m++)
		{
			if (odd == false)
			{
				elements.push_back(m + n * (subU + 1));
				elements.push_back(m + 1 + n * (subU + 1));
				elements.push_back(m + subU + 2 + n * (subU + 1));
				elements.push_back(m + n * (subU + 1));
				elements.push_back(m + subU + 2 + n * (subU + 1));
				elements.push_back(m + subU + 1 + n * (subU + 1));

				odd = true;
			}
			else
			{
				elements.push_back(m + 1 + n * (subU + 1));
				elements.push_back(m + subU + 1 + n * (subU + 1));
				elements.push_back(m + n * (subU + 1));
				elements.push_back(m + 1 + n * (subU + 1));
				elements.push_back(m + subU + 2 + n * (subU + 1));
				elements.push_back(m + subU + 1 + n * (subU + 1));

				odd = false;
			}

		}
	}

}

void Terrain_Mesh::Generate_Elements_2(int base_vertex)
{
	bool odd = true;

	for (int n = 0; n < Size_of_Patch; n++)
	{
		if (n % 2 == 0)
		{
			odd = false;
		}
		else
		{
			odd = true;
		}

		for (int m = base_vertex; m < (base_vertex + Size_of_Patch); m++)
		{
			if (odd == false)
			{
				patch_elements.push_back(m + n * (subU + 1));
				patch_elements.push_back(m + 1 + n * (subU + 1));
				patch_elements.push_back(m + subU + 2 + n * (subU + 1));
				patch_elements.push_back(m + n * (subU + 1));
				patch_elements.push_back(m + subU + 2 + n * (subU + 1));
				patch_elements.push_back(m + subU + 1 + n * (subU + 1));

				odd = true;
			}
			else
			{
				patch_elements.push_back(m + 1 + n * (subU + 1));
				patch_elements.push_back(m + subU + 1 + n * (subU + 1));
				patch_elements.push_back(m + n * (subU + 1));
				patch_elements.push_back(m + 1 + n * (subU + 1));
				patch_elements.push_back(m + subU + 2 + n * (subU + 1));
				patch_elements.push_back(m + subU + 1 + n * (subU + 1));

				odd = false;
			}

		}
	}
}

void Terrain_Mesh::Generate_Patches(int size_of_patch)
{	
	Size_of_Patch = size_of_patch;

	int counter = 0;
	Patches.resize(((subU / Size_of_Patch)) * ((subV/ Size_of_Patch)));

	for (int n = 0; n < (subV - Size_of_Patch); n = n + Size_of_Patch)
	{

		for (int m = 0; m < (subU - Size_of_Patch); m = m + Size_of_Patch)
		{
			int number_of_quads_patch = size_of_patch * size_of_patch;
			int number_of_triangles_patch = number_of_quads_patch * 2;
			int number_of_elements_patch = 3 * number_of_triangles_patch;

			Patches[counter].start_vertex = m + n * (subU + 1);
			Patches[counter].number_of_elements = number_of_elements_patch;
			Patches[counter].last_vertex = m + Size_of_Patch * subU + Size_of_Patch * 2 + n * (subU + 1);


			counter++;
		}
	}
}

void Terrain_Mesh::Initialise_Normals()
{
	int counter = 0;

	for (int n = 0; n <= subV; n++)
	{
		for (int m = 0; m <= subU; m++)
		{
			normals[counter].x = 0;
			normals[counter].y = 0;
			normals[counter].z = 0;

			counter++;
		}
	}


}

void Terrain_Mesh::Calculate_Normals()
{
	for (int i = 0; i < number_of_elements; i += 3)
	{
		glm::vec3 p1 = positions[patch_elements[i]];
		glm::vec3 p2 = positions[patch_elements[i + 1]];
		glm::vec3 p3 = positions[patch_elements[i + 2]];

		glm::vec3 edge1 = p2 - p1;
		glm::vec3 edge2 = p3 - p1;
		glm::vec3 normal = glm::cross(edge1, edge2);

		normals[patch_elements[i]] += normal;
		normals[patch_elements[i + 1]] += normal;
		normals[patch_elements[i + 2]] += normal;
	}

	for (auto & p : normals)
	{
		p = glm::normalize(p);
	}
}

glm::vec3 Terrain_Mesh::CubicBezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float u)
{
	return (1 - u) * (1 - u) * (1 - u) * p0
		+ 3 * u * (1 - u) * (1 - u) * p1
		+ 3 * u * u * (1 - u) * p2
		+ u * u * u * p3;
}glm::vec3 Terrain_Mesh::BezierSurface(std::vector<glm::vec3> cps, float u, float v)
{
	std::vector<glm::vec3> curve{ 4 };

	for (int j = 0; j < 4; ++j)
	{
		curve[j] = CubicBezier(cps[j * 4], cps[j * 4 + 1], cps[j * 4 + 2], cps[j * 4 + 3], u);
	}
	return CubicBezier(curve[0], curve[1], curve[2], curve[3], v);
}

void Terrain_Mesh::Calculate_Initial_Positions(std::vector<glm::vec3> control_points_1, std::vector<glm::vec3> control_points_2)
{
	int counter = 0;

	for (int n = 0; n <= subV; n++)
	{
		for (int m = 0; m <= subU; m++)
		{
			float u = 0.f;
			float v = 0.f;

			u = ((float)m) / subU;
			v = ((float)n) / subV;

			UVcoordinates[counter].x = u;
			UVcoordinates[counter].y = v;


			if (u < 0.5f)
			{
				positions[counter] = BezierSurface(control_points_1, u * 2, v);
			}
			else
			{
				positions[counter] = BezierSurface(control_points_2, u * 2 - 1, v);
			}

			counter++;
		}
	}
}

const void Terrain_Mesh::Apply_Displacement(tygra::Image & displacement_image)
{
	int counter = 0;

	for (int n = 0; n <= subV; n++)
	{
		for (int m = 0; m <= subU; m++)
		{
			float scale = 150.f;

			uint8_t displacement = *(uint8_t*)displacement_image.pixel(UVcoordinates[counter].x * (subU-1), UVcoordinates[counter].y * (subV - 1));


			positions[counter] += (displacement / 255.0f) * normals[counter] * scale;

			counter++;
		}
	}
}

void Terrain_Mesh::Apply_Noise(int octaves_, float initial_frequency, float amplitude_, float lacunarity_, float gain_)
{
	PerlinNoise perlin_noise;

	int octaves = octaves_;
	float frequency = 1.0f;

	for (int i = 0; i < number_of_vertices; i++)
	{
		float amplitude = amplitude_;
		float lacunarity = lacunarity_;
		float gain = gain_;

		float total = 0.f;
		frequency = initial_frequency;

		auto& p = positions[i];
		auto& n = normals[i];

		for (int j = 0; j < octaves; j++)
		{
			total += (float)perlin_noise.noise((float)p.x * frequency, (float)p.y * frequency, (float)p.z * frequency) * amplitude;
			frequency *= lacunarity;
			amplitude *= gain;
		}

		p += n * total;
	}
}

float Terrain_Mesh::Find_Max_Y(int base_vertex)
{	
	float max_y = positions[base_vertex].y;

	for (int n = 0; n < 2; n++)
	{
		for (int m = base_vertex; m < (base_vertex + 2); m++)
		{
			if (positions[m + n * (2 + 1)].y > max_y)
			{
				max_y = positions[m + n * (2 + 1)].y;
			}
		}
	}

	return max_y;
}

bool Terrain_Mesh::Check_Patch(Patch the_patch, Frustum & the_frustum_)
{
	bool ok_to_render = false;
	if (the_patch.last_vertex != 0)
	{

		if ((the_frustum_.Point_in_Frustum(positions[the_patch.start_vertex]) == true)
			|| (the_frustum_.Point_in_Frustum(positions[the_patch.start_vertex + 2]) == true)
			|| (the_frustum_.Point_in_Frustum(positions[the_patch.last_vertex - 2]) == true)
			|| (the_frustum_.Point_in_Frustum(positions[the_patch.last_vertex]) == true)
			)
		{
			ok_to_render = true;
		}

		glm::vec3 start_vertex_top = positions[the_patch.start_vertex];
		glm::vec3 start_vertex_top_2 = positions[the_patch.start_vertex + 2];
		glm::vec3 last_vertex_top = positions[the_patch.last_vertex];
		glm::vec3 last_vertex_top_2 = positions[the_patch.last_vertex - 2];

		start_vertex_top.y = the_patch.max_y;
		start_vertex_top_2.y = the_patch.max_y;
		last_vertex_top.y = the_patch.max_y;
		last_vertex_top_2.y = the_patch.max_y;

		if ((the_frustum_.Point_in_Frustum(start_vertex_top) == true)
			|| (the_frustum_.Point_in_Frustum(start_vertex_top_2) == true)
			|| (the_frustum_.Point_in_Frustum(last_vertex_top) == true)
			|| (the_frustum_.Point_in_Frustum(last_vertex_top_2) == true)
			)
		{
			ok_to_render = true;
		}
	}
	return ok_to_render;
}

const std::vector<glm::vec3> Terrain_Mesh::Get_Positions() const
{
	return positions;
}

const std::vector<glm::vec3> Terrain_Mesh::Get_Normals() const
{
	return normals;
}

const std::vector<GLuint> Terrain_Mesh::Get_Elements() const
{
	return patch_elements;
}

std::vector<Patch> Terrain_Mesh::Get_Patches() 
{
	return Patches;
}


Terrain_Mesh::~Terrain_Mesh()
{
}
