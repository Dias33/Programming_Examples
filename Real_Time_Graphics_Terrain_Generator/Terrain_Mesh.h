#pragma once

#include <glm/glm.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tygra/FileHelper.hpp>
#include <tgl/tgl.h>
#include <vector>

class Frustum;



struct Patch
{
	/*
	* Structure for a patch. It stores the start vertex indice, the last vertex indice, the number of elements in the patch and the max value of y in the patch.
	*/

	int start_vertex;
	int last_vertex;
	int number_of_elements;

	int max_y;
};

	/*
	* Base class to be used to create a grid and apply effects to it.
	*/

class Terrain_Mesh

{
public:

	Terrain_Mesh();

	/* 
	* Constructor class for the terrain mesh. 
	* Input are two ints which represent the number of subdivions for the grid (on x & z axes).
	* These two numbers must be a multiple for the **size of the patch**.
	*/
	Terrain_Mesh(int subU, int subV);

	/*
	* Function to generate the elements of a given grid with subdivisions.
	* It will create a diamond pattern on the grid, allowing for better calculations for its normals.
	*/

	void Generate_Elements();
	
	/*
	* Function to generate the elements of a given grid with subdivions, split into patches.
	* It will create a diamond pattern on the grid, allowing for better calculations for its normals.
	* In order to work out the elements for each patch, it needs as an input the start vertex of each patch.
	*/

	void Generate_Elements_2(int base_vertex);

	/*
	* Function to split the given grid with subdivisions in patches of size of given_input * given_input.
	* The total number of subdivisions **must be a multiple** of the size of the patch in order to work.
	*/

	void Generate_Patches(int size_of_patch);

	/*
	* Initialise the normals of the grid with the 0 value for x , y and z.
	*/

	void Initialise_Normals();

	/*
	* Calculate the normals after applying an effect to the grid (e.g: Noise)
	* Creates a normal for each triangle by having a cross product between two edges of the triangle.
	*/

	void Calculate_Normals();

	/*
	* Function used in BezierSurface. It is used to return a position on a Bezier curve.
	* Inputs: four positions to interpolate through and a global U or V coordinate.
	*/

	glm::vec3 CubicBezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float u);

	/*
	* Function used in Calculate_Initial_Positions. It is used to calculate a position on a Bezier surface.
	* Inputs: a vector of control points for a single patch and the global U and V coordinates for the given position.
	*/

	glm::vec3 Terrain_Mesh::BezierSurface(std::vector<glm::vec3> cps, float u, float v);

	/*
	* Function to calculate the initial positions of each vertex in the grid by using Bezier Surfaces.
	* Inputs: two vectors which hold the control points of two patches.
	*/

	void Calculate_Initial_Positions(std::vector<glm::vec3> control_points_1, std::vector<glm::vec3> control_points_2);

	/*
	* Function to apply displacement to the grid. 
	* Inputs: an image (heightmap) to be applied to the grid.
	*/

	const void Apply_Displacement(tygra::Image & displace_image);

	/*
	* Function to apply Fractional Brownian Noise. It uses the PerlinNoise class to generate a noise.
	* Then, based on the inputs, it creates fractional brownian noise.
	* Inputs:
	* Octaves: How many layers of noise you are putting together.
	* Frequency: How many points fit into the space you have created.
	* Amplitude: How tall the features should be.
	* Lacunarity: What makes the frequency grow. Standard value should be 2.0f.
	* Gain: What makes the amplitude shrink (or not shrink). Standard value should be 0.5f.
	*/

	void Apply_Noise(int octaves_, float initial_frequency, float amplitude_, float lacunarity_, float gain_);

	/*
	* Function to find the maximum value of y in a patch.
	* Input: the start vertex of a patch.
	*/

	float Find_Max_Y(int base_vertex);

	/*
	* Function to check if a patch is in the frustum.
	* This is what determines the visibility of the patch.
	* It builds an AABB (Axis aligned bounded box) of the patch and checks its corners against the frustum.
	* Inputs: a patch and the frustum.
	*/

	bool Check_Patch(Patch the_patch, Frustum & the_frustum_);

	/*
	* Get functions to return vectors of positions, normals, elements and patches.
	*/

	const std::vector<glm::vec3> Get_Positions() const;

	const std::vector<glm::vec3> Get_Normals() const;

	const std::vector<GLuint> Get_Elements() const;

	std::vector<Patch> Get_Patches();

	~Terrain_Mesh();

private:

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVcoordinates;
	std::vector<GLuint> elements;
	std::vector<GLuint> patch_elements;

	int subU;
	int subV;
	int Size_of_Patch;

	int number_of_vertices;
	int number_of_quads;
	int number_of_triangles;
	int number_of_elements;

	std::vector<Patch> Patches;
};

