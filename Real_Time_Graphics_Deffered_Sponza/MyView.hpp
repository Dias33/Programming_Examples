#pragma once

#include <sponza/sponza_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>
#include <map>

class MyView : public tygra::WindowViewDelegate
{
public:

    MyView();

    ~MyView();

    void setScene(const sponza::Context * scene);

private:

    void windowViewWillStart(tygra::Window * window) override;

    void windowViewDidReset(tygra::Window * window,
                            int width,
                            int height) override;

    void windowViewDidStop(tygra::Window * window) override;

    void windowViewRender(tygra::Window * window) override;

	GLuint geometry_program_{ 0 };
	GLuint global_light_program_{ 0 };
	GLuint point_light_program_{ 0 };
	GLuint spot_light_program_{ 0 };

    const sponza::Context * scene_;

	GLuint gbuffer_depth_rbo_{0};
	GLuint gbuffer_fbo_{0};

	GLuint gbuffer_position_tex_{ 0 };
	GLuint gbuffer_normal_tex_{ 0 };
	GLuint gbuffer_ambient_tex_{ 0 };
	GLuint gbuffer_kd_tex_{ 0 };

	GLuint gbuffer_depth_tex_{ 0 };

	GLuint lbuffer_fbo_{ 0 };
	GLuint lbuffer_colour_rbo_{ 0 };

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texture_coord;
	};

	struct PerModelUniforms
	{
		glm::mat4 combined_xform;
		glm::mat4 model_xform;
	};

	GLuint per_model_ubo_;

	struct MeshGL
	{
		GLuint vertex_vbo{ 0 };

		GLuint element_vbo{ 0 };

		int element_count{ 0 };

		GLuint vao{ 0 };
	};

	MeshGL light_quad_mesh_;
	MeshGL light_sphere_mesh_;
	MeshGL light_cone_mesh_;

	std::map<sponza::MeshId, MeshGL> sponza_meshes;

	MeshGL Create_Mesh(sponza::Mesh source_mesh);

	void Geometry_Pass(glm::mat4 projection_xform_, glm::mat4 view_xform_, GLuint & program_id);

	void Pass_Light_Data(int light_identifier, int which_one, GLuint & program_id, glm::mat4 projection_xform_, glm::mat4 view_xform_);

	void Create_Shader_Program(std::string name_of_shader_file, GLuint & program_id);

	const static GLuint kNullId = 0;

	enum VertexAttribIndexes {
		kVertexPosition = 0,
		kVertexColour = 1,
		kVertexNormal = 2,
		kVertexTextCoord = 3

	};
	enum FragmentDataIndexes {
		kFragmentColour = 0,
	};
	enum TextureIndexes {
		kTextureTest = 0
	};


};
