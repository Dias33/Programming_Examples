#include "my_view.hpp"
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include "PerlinNoise.h"
#include "Terrain_Mesh.h"
#include "Frustum.h"

MyView::MyView()
{

}

MyView::~MyView() {
}

void MyView::setScene(const scene::Context * scene)
{
    scene_ = scene;
}

void MyView::toggleShading()
{
    shade_normals_ = !shade_normals_;
}

void MyView::windowViewWillStart(tygra::Window * window)
{
    assert(scene_ != nullptr);

    GLint compile_status = 0;
    GLint link_status = 0;

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertex_shader_string
        = tygra::createStringFromFile("resource:///terrain_vs.glsl");
    const char *vertex_shader_code = vertex_shader_string.c_str();
    glShaderSource(vertex_shader, 1,
                   (const GLchar **)&vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length] = "";
        glGetShaderInfoLog(vertex_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragment_shader_string
        = tygra::createStringFromFile("resource:///terrain_fs.glsl");
    const char *fragment_shader_code = fragment_shader_string.c_str();
    glShaderSource(fragment_shader, 1,
                   (const GLchar **)&fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length] = "";
        glGetShaderInfoLog(fragment_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }

    terrain_sp_ = glCreateProgram();
    glAttachShader(terrain_sp_, vertex_shader);
    glDeleteShader(vertex_shader);
    glAttachShader(terrain_sp_, fragment_shader);
    glDeleteShader(fragment_shader);
    glLinkProgram(terrain_sp_);

    glGetProgramiv(terrain_sp_, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length] = "";
        glGetProgramInfoLog(terrain_sp_, string_length, NULL, log);
        std::cerr << log << std::endl;
    }

    // X and -Z are on the ground, Y is up
    const float sizeX = scene_->getTerrainSizeX();
    const float sizeY = scene_->getTerrainSizeY();
    const float sizeZ = scene_->getTerrainSizeZ();


    const auto& bezier_patches = scene_->getTerrainPatches();
    const size_t number_of_patches = bezier_patches.size();

	std::vector<glm::vec3> cp1;
	std::vector<glm::vec3> cp2;

	cp1.resize(16);
	cp2.resize(16);

	int count = 0;

	/*
	* Converting the control points for the two patches provided.
	*/

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			cp1[count] = bezier_patches[0].at(x, y);
			cp2[count] = bezier_patches[1].at(x, y);
			count++;
		}

	}

    tygra::Image displace_image =
        tygra::createImageFromPngFile(scene_->getTerrainDisplacementMapName());

	/*
	* The order for creating the grid and applying effects to it is strict.
	* Use the following order: 
	* 1) Use the Constructor with two parameters, then Generate_Patches.
	* 2) Loop through each patch and generate its elements(Generate_Elements_2).
	* 3) Initialise_Normals.
	* 4) Calculate_Initial_Positions.
	* 5) Calculate_Normals.
	* 6) Apply_Displacement.
	* 7) Recalculate Normals.
	* 8) Apply_Noise.
	* 9) Calculate_Normals.
	* 10) Optional: If determining visibility with frustum get the max value of Y for each patch(Find_Max_Y)
	*/

	My_Mesh = Terrain_Mesh(252, 252);

	My_Mesh.Generate_Patches(4);

	for (auto & patches : My_Mesh.Get_Patches())
	{
		My_Mesh.Generate_Elements_2(patches.start_vertex);
	}

	My_Mesh.Initialise_Normals();

	My_Mesh.Calculate_Initial_Positions(cp1, cp2);

	My_Mesh.Calculate_Normals();

	My_Mesh.Apply_Displacement(displace_image);

	My_Mesh.Calculate_Normals();

	My_Mesh.Apply_Noise(5, 0.01f, 35, 2.0f, 0.5f);

	My_Mesh.Calculate_Normals();

	for (auto & p : My_Mesh.Get_Patches())
	{
		p.max_y = My_Mesh.Find_Max_Y(p.start_vertex);
	}

    // below is indicative code for initialising a terrain VAO.

    glGenBuffers(1, &terrain_mesh_.element_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_mesh_.element_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        My_Mesh.Get_Elements().size() * sizeof(GLuint),
		My_Mesh.Get_Elements().data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    terrain_mesh_.element_count = My_Mesh.Get_Elements().size();

    glGenBuffers(1, &terrain_mesh_.position_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.position_vbo);
    glBufferData(GL_ARRAY_BUFFER, My_Mesh.Get_Positions().size() * sizeof(glm::vec3),
		My_Mesh.Get_Positions().data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &terrain_mesh_.normal_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.normal_vbo);
    glBufferData(GL_ARRAY_BUFFER, My_Mesh.Get_Normals().size() * sizeof(glm::vec3),
		My_Mesh.Get_Normals().data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &terrain_mesh_.vao);
    glBindVertexArray(terrain_mesh_.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_mesh_.element_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.position_vbo);
    glEnableVertexAttribArray(kVertexPosition);
    glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, terrain_mesh_.normal_vbo);
    glEnableVertexAttribArray(kVertexNormal);
    glVertexAttribPointer(kVertexNormal, 3, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MyView::windowViewDidReset(tygra::Window * window,
                                int width,
                                int height)
{
    glViewport(0, 0, width, height);
}

void MyView::windowViewDidStop(tygra::Window * window)
{
    glDeleteProgram(terrain_sp_);
    glDeleteBuffers(1, &terrain_mesh_.position_vbo);
    glDeleteBuffers(1, &terrain_mesh_.normal_vbo);
    glDeleteBuffers(1, &terrain_mesh_.element_vbo);
    glDeleteVertexArrays(1, &terrain_mesh_.vao);
}

void MyView::windowViewRender(tygra::Window * window)
{
    assert(scene_ != nullptr);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const float aspect_ratio = viewport[2] / (float)viewport[3];

    const auto& camera = scene_->getCamera();
    glm::mat4 projection_xform = glm::perspective(
        glm::radians(camera.getVerticalFieldOfViewInDegrees()),
        aspect_ratio,
        camera.getNearPlaneDistance(),
        camera.getFarPlaneDistance());
    glm::vec3 camera_pos = camera.getPosition();
    glm::vec3 camera_at = camera.getPosition() + camera.getDirection();
    glm::vec3 world_up{ 0, 1, 0 };
    glm::mat4 view_xform = glm::lookAt(camera_pos, camera_at, world_up);


    glClearColor(0.f, 0.f, 0.25f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, shade_normals_ ? GL_FILL : GL_LINE);

    glUseProgram(terrain_sp_);

    GLuint shading_id = glGetUniformLocation(terrain_sp_, "use_normal");
    glUniform1i(shading_id, shade_normals_);

    glm::mat4 world_xform = glm::mat4(1);
    glm::mat4 view_world_xform = view_xform * world_xform;

    GLuint projection_xform_id = glGetUniformLocation(terrain_sp_,
                                                      "projection_xform");
    glUniformMatrix4fv(projection_xform_id, 1, GL_FALSE,
                       glm::value_ptr(projection_xform));

    GLuint view_world_xform_id = glGetUniformLocation(terrain_sp_,
                                                      "view_world_xform");
    glUniformMatrix4fv(view_world_xform_id, 1, GL_FALSE,
                       glm::value_ptr(view_world_xform));

	Frustum The_Frustum(projection_xform, view_world_xform);

    if (terrain_mesh_.vao) {
        glBindVertexArray(terrain_mesh_.vao);

		// This count was created to prove that the visibility with frustum works. It counts the number of patches drawn. 
		int count_patches = 0;

		for (auto & patches : My_Mesh.Get_Patches())
		{
			if (My_Mesh.Check_Patch(patches, The_Frustum) == true)
			{
				count_patches++;
				glDrawElementsBaseVertex(GL_TRIANGLES, patches.number_of_elements, GL_UNSIGNED_INT, 0, patches.start_vertex);
			}
		}	

		// Uncomment the next line for this count to be shown.
		std::cout << count_patches << std::endl;;
    }
}
