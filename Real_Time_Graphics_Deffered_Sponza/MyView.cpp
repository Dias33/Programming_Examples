#include "MyView.hpp"

#include <sponza/sponza.hpp>
#include <tygra/FileHelper.hpp>
#include <tsl/shapes.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>
#include <nvToolsExt.h>

MyView::MyView()
{
}

MyView::~MyView() {
}

void MyView::setScene(const sponza::Context * scene)
{
    scene_ = scene;
}

void MyView::windowViewWillStart(tygra::Window * window)
{
    assert(scene_ != nullptr);

	/*
	* Tutorial: this section of code creates a fullscreen quad to be used
	*           when computing global illumination effects (e.g. ambient)
	*/
	{
		std::vector<glm::vec2> vertices(4);
		vertices[0] = glm::vec2(-1, -1);
		vertices[1] = glm::vec2(1, -1);
		vertices[2] = glm::vec2(1, 1);
		vertices[3] = glm::vec2(-1, 1);

		glGenBuffers(1, &light_quad_mesh_.vertex_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, light_quad_mesh_.vertex_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			vertices.size() * sizeof(glm::vec2),
			vertices.data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &light_quad_mesh_.vao);
		glBindVertexArray(light_quad_mesh_.vao);
		glBindBuffer(GL_ARRAY_BUFFER, light_quad_mesh_.vertex_vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
			sizeof(glm::vec2), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	/*
	* Tutorial: this code creates a sphere to use when deferred shading
	*           with a point light source.
	*/
	{
		tsl::IndexedMeshPtr mesh = tsl::createSpherePtr(1.f, 12);
		mesh = tsl::cloneIndexedMeshAsTriangleListPtr(mesh.get());

		light_sphere_mesh_.element_count = mesh->indexCount();

		glGenBuffers(1, &light_sphere_mesh_.vertex_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, light_sphere_mesh_.vertex_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			mesh->vertexCount() * sizeof(glm::vec3),
			mesh->positionArray(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &light_sphere_mesh_.element_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_sphere_mesh_.element_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->indexCount() * sizeof(unsigned int),
			mesh->indexArray(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &light_sphere_mesh_.vao);
		glBindVertexArray(light_sphere_mesh_.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_sphere_mesh_.element_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, light_sphere_mesh_.vertex_vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(glm::vec3), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	//Cone Mesh

	{
		tsl::IndexedMeshPtr cone_mesh = tsl::createConePtr(16.f, 1, 48);
		cone_mesh = tsl::cloneIndexedMeshAsTriangleListPtr(cone_mesh.get());

		light_cone_mesh_.element_count = cone_mesh->indexCount();

		glGenBuffers(1, &light_cone_mesh_.vertex_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, light_cone_mesh_.vertex_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			cone_mesh->vertexCount() * sizeof(glm::vec3),
			cone_mesh->positionArray(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &light_cone_mesh_.element_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_cone_mesh_.element_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			cone_mesh->indexCount() * sizeof(unsigned int),
			cone_mesh->indexArray(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &light_cone_mesh_.vao);
		glBindVertexArray(light_cone_mesh_.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light_cone_mesh_.element_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, light_cone_mesh_.vertex_vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(glm::vec3), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	Create_Shader_Program("geometry", geometry_program_);
	Create_Shader_Program("global_light", global_light_program_);
	Create_Shader_Program("point_light", point_light_program_);
	Create_Shader_Program("spot_light", spot_light_program_);
	

	sponza::GeometryBuilder builder;
	const auto& scene_meshes = builder.getAllMeshes();

	for (const auto& scene_mesh : scene_meshes)
	{
		sponza_meshes[scene_mesh.getId()] = Create_Mesh(builder.getMeshById(scene_mesh.getId()));
	}

	glGenTextures(1, &gbuffer_position_tex_);
	glGenTextures(1, &gbuffer_normal_tex_);
	glGenTextures(1, &gbuffer_depth_tex_);

	glGenFramebuffers(1, &gbuffer_fbo_);
	glGenRenderbuffers(1, &gbuffer_depth_rbo_);

	glGenFramebuffers(1, &lbuffer_fbo_);
	glGenRenderbuffers(1, &lbuffer_colour_rbo_);

	glGenBuffers(1, &per_model_ubo_);
	glBindBuffer(GL_UNIFORM_BUFFER, per_model_ubo_);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(PerModelUniforms), nullptr, GL_STREAM_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, per_model_ubo_);
	glUniformBlockBinding(geometry_program_, glGetUniformBlockIndex(geometry_program_, "PerModelUniforms"), 1);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE);
}

void MyView::windowViewDidReset(tygra::Window * window,
                                int width,
                                int height)
{
    glViewport(0, 0, width, height);

	GLenum framebuffer_status = 0;

	glBindRenderbuffer(GL_RENDERBUFFER, gbuffer_depth_rbo_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH24_STENCIL8, width, height);

	glGenTextures(1, &gbuffer_position_tex_);
	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_position_tex_);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);

	glGenTextures(1, &gbuffer_normal_tex_);
	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_normal_tex_);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB32F, width, height, GL_TRUE);

	glGenTextures(1, &gbuffer_ambient_tex_);
	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_ambient_tex_);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);

	glGenTextures(1, &gbuffer_kd_tex_);
	glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_kd_tex_);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, lbuffer_colour_rbo_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, lbuffer_fbo_);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuffer_depth_rbo_);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, lbuffer_colour_rbo_);

	framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
		tglDebugMessage(GL_DEBUG_SEVERITY_HIGH_ARB, "framebuffer not complete");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo_);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, gbuffer_position_tex_, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, gbuffer_normal_tex_, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, gbuffer_ambient_tex_, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_RECTANGLE, gbuffer_kd_tex_, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gbuffer_depth_rbo_);

	framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE)
	{
		tglDebugMessage(GL_DEBUG_SEVERITY_HIGH_ARB, "framebuffer not complete");
	}
	
	GLenum bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, bufs);
}

void MyView::windowViewDidStop(tygra::Window * window)
{
	glDeleteProgram(geometry_program_);
	glDeleteProgram(global_light_program_);
	glDeleteProgram(point_light_program_);
	glDeleteProgram(spot_light_program_);

	for (auto& mit : sponza_meshes)
	{
		auto mesh_id = mit.first;
		auto& mesh = mit.second;
		glDeleteBuffers(1, &mesh.element_vbo);
		glDeleteBuffers(1, &mesh.vertex_vbo);
		glDeleteVertexArrays(1, &mesh.vao);
	}

	glDeleteBuffers(1, &light_quad_mesh_.vertex_vbo);
	glDeleteBuffers(1, &light_quad_mesh_.element_vbo);
	glDeleteVertexArrays(1, &light_quad_mesh_.vao);

	glDeleteBuffers(1, &light_sphere_mesh_.vertex_vbo);
	glDeleteBuffers(1, &light_sphere_mesh_.element_vbo);
	glDeleteVertexArrays(1, &light_sphere_mesh_.vao);

	glDeleteTextures(1, &gbuffer_position_tex_);
	glDeleteTextures(1, &gbuffer_normal_tex_);
	glDeleteTextures(1, &gbuffer_ambient_tex_);
	glDeleteTextures(1, &gbuffer_kd_tex_);
	glDeleteTextures(1, &gbuffer_depth_tex_);

	glDeleteFramebuffers(1, &lbuffer_fbo_);
	glDeleteFramebuffers(1, &gbuffer_fbo_);
	glDeleteRenderbuffers(1, &lbuffer_colour_rbo_);
	glDeleteRenderbuffers(1, &gbuffer_depth_rbo_);


}

void MyView::windowViewRender(tygra::Window * window)
{
    assert(scene_ != nullptr);

	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer_fbo_);

	glEnable(GL_CULL_FACE);

	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glDisable(GL_BLEND);
	glClearColor(0.25f, 0.f, 0.25f, 0.f);
	glClearStencil(128);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	
	glUseProgram(geometry_program_);

	glEnable(GL_MULTISAMPLE);

	GLint viewport_size[4];
	glGetIntegerv(GL_VIEWPORT, viewport_size);
	const float aspect_ratio = viewport_size[2] / (float)viewport_size[3];
	const auto& Camera = scene_->getCamera();

	glm::mat4 projection_xform = glm::perspective(glm::radians(Camera.getVerticalFieldOfViewInDegrees()),
		aspect_ratio,
		Camera.getNearPlaneDistance(),
		Camera.getFarPlaneDistance());
	glm::mat4 view_xform = glm::lookAt((const glm::vec3 &)Camera.getPosition(),
		(const glm::vec3 &)Camera.getPosition() + (const glm::vec3 &)Camera.getDirection(),
		(const glm::vec3 &)scene_->getUpDirection());

	Geometry_Pass(projection_xform, view_xform, geometry_program_);

	glBindFramebuffer(GL_FRAMEBUFFER, lbuffer_fbo_);
	glClearColor(0.25f, 0.f, 0.25f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_MULTISAMPLE);

	// Global Light Shader

	glUseProgram(global_light_program_);

	glDisable(GL_DEPTH_TEST);
	bool first_pass = false;

	int number_of_lights = scene_->getAllDirectionalLights().size();

	for (int i = 0; i < number_of_lights; i++)
	{
		if (first_pass == true)
		{
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);
			glEnable(GL_BLEND);
		}

		Pass_Light_Data(2, i, global_light_program_, projection_xform, view_xform);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_position_tex_);

		GLuint world_position_id = glGetUniformLocation(global_light_program_, "sampler_world_position");
		glUniform1i(world_position_id, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_normal_tex_);

		GLuint world_normal_id = glGetUniformLocation(global_light_program_, "sampler_world_normal");
		glUniform1i(world_normal_id, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_ambient_tex_);

		GLuint ambient_light_id = glGetUniformLocation(global_light_program_, "ambient_light");
		glUniform1i(ambient_light_id, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_kd_tex_);

		GLuint kd_id = glGetUniformLocation(global_light_program_, "kd");
		glUniform1i(kd_id, 3);

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 128, ~0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glBindVertexArray(light_quad_mesh_.vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		first_pass = true;
	}

	// Point Light Shader

	glUseProgram(point_light_program_);

	glDepthMask(GL_FALSE);
	glDepthFunc(GL_EQUAL);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	number_of_lights = scene_->getAllPointLights().size();

	for (int i = 0; i < number_of_lights; i++)
	{

		Pass_Light_Data(1, i, point_light_program_, projection_xform, view_xform);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_position_tex_);

		GLuint world_position_id = glGetUniformLocation(point_light_program_, "sampler_world_position");
		glUniform1i(world_position_id, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_normal_tex_);

		GLuint world_normal_id = glGetUniformLocation(point_light_program_, "sampler_world_normal");
		glUniform1i(world_normal_id, 1);

		glBindVertexArray(light_sphere_mesh_.vao);
		glDrawElements(GL_TRIANGLES, light_sphere_mesh_.element_count, GL_UNSIGNED_INT, 0);
	}

	// Spot Light Shader

	glUseProgram(spot_light_program_);

	number_of_lights = scene_->getAllSpotLights().size();

	for (int i = 0; i < number_of_lights; i++)
	{
		Pass_Light_Data(0, i, spot_light_program_, projection_xform, view_xform);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_position_tex_);

		GLuint world_position_id = glGetUniformLocation(spot_light_program_, "sampler_world_position");
		glUniform1i(world_position_id, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_normal_tex_);

		GLuint world_normal_id = glGetUniformLocation(spot_light_program_, "sampler_world_normal");
		glUniform1i(world_normal_id, 1);

		glBindVertexArray(light_cone_mesh_.vao);
		glDrawElements(GL_TRIANGLES, light_cone_mesh_.element_count, GL_UNSIGNED_INT, 0);
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, lbuffer_fbo_);
	glBlitFramebuffer(0, 0, 1280, 720, 0, 0, 1280, 720, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

MyView::MeshGL MyView::Create_Mesh(sponza::Mesh source_mesh)
{
	MeshGL object_mesh;

	std::vector<Vertex> Vertices;

	const auto& vertex_positions = source_mesh.getPositionArray();
	const auto& triangles_elements = source_mesh.getElementArray();
	const auto& vertex_normal = source_mesh.getNormalArray();
	const auto& texture_coordinates_ = source_mesh.getTextureCoordinateArray();

	//  Create OpenGL buffers to hold the geometry data. 

	Vertices.resize(vertex_positions.size());

	for (int i = 0; i < vertex_positions.size(); i++)
	{
		Vertices[i].position = (const glm::vec3 &) vertex_positions[i];
		Vertices[i].normal = (const glm::vec3 &) vertex_normal[i];
	}

	for (int i = 0; i < texture_coordinates_.size(); i++)
	{
		Vertices[i].texture_coord = (const glm::vec2 &)texture_coordinates_[i];
	}

	glGenBuffers(1, &object_mesh.vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, object_mesh.vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex),
		Vertices.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, kNullId);

	glGenBuffers(1, &object_mesh.element_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object_mesh.element_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		triangles_elements.size() * sizeof(unsigned int),
		triangles_elements.data(),
		GL_STATIC_DRAW);

	object_mesh.element_count = triangles_elements.size();

	glGenVertexArrays(1, &object_mesh.vao);

	glBindVertexArray(object_mesh.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object_mesh.element_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, object_mesh.vertex_vbo);
	glEnableVertexAttribArray(kVertexPosition);
	glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), TGL_BUFFER_OFFSET_OF(Vertex, position));

	glEnableVertexAttribArray(kVertexNormal);
	glVertexAttribPointer(kVertexNormal, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), TGL_BUFFER_OFFSET_OF(Vertex, normal));

	glEnableVertexAttribArray(kVertexTextCoord);
	glVertexAttribPointer(kVertexTextCoord, 2, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), TGL_BUFFER_OFFSET_OF(Vertex, texture_coord));
	glBindBuffer(GL_ARRAY_BUFFER, kNullId);

	glBindVertexArray(kNullId);

	return object_mesh;
}

void MyView::Geometry_Pass(glm::mat4 projection_xform_, glm::mat4 view_xform_, GLuint & program_id)
{
	for (const auto& instance : scene_->getAllInstances())
	{
		auto material_data = scene_->getMaterialById(instance.getMaterialId());

		const MeshGL& mesh = sponza_meshes[instance.getMeshId()];

		glm::mat4 model_xform = (const glm::mat4x3 &)(instance.getTransformationMatrix());

		glm::mat4 combined_xform = projection_xform_ * view_xform_ * model_xform;

		PerModelUniforms per_model_uniforms;
		per_model_uniforms.model_xform = model_xform;
		per_model_uniforms.combined_xform = combined_xform;

		glBindBuffer(GL_UNIFORM_BUFFER, per_model_ubo_);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(per_model_uniforms), &per_model_uniforms);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_position_tex_);

		GLuint world_position_id = glGetUniformLocation(program_id, "sampler_world_position");
		glUniform1i(world_position_id, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_normal_tex_);

		GLuint world_normal_id = glGetUniformLocation(program_id, "sampler_world_normal");
		glUniform1i(world_normal_id, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_ambient_tex_);

		GLuint ambient_light_id = glGetUniformLocation(program_id, "amb_light");
		glUniform3fv(ambient_light_id, 1, glm::value_ptr((const glm::vec3 &)scene_->getAmbientLightIntensity()));

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbuffer_kd_tex_);

		GLuint kd_id = glGetUniformLocation(program_id, "object_colour");
		glUniform3fv(kd_id, 1, glm::value_ptr((const glm::vec3 &)material_data.getDiffuseColour()));

		glBindVertexArray(mesh.vao);
		glDrawElements(GL_TRIANGLES, mesh.element_count, GL_UNSIGNED_INT, 0);
	}
}

void MyView::Pass_Light_Data(int light_identifier, int which_one, GLuint & program_id, glm::mat4 projection_xform_, glm::mat4 view_xform_)
{
	auto& The_Light = scene_->getAllSpotLights()[0];

	if (light_identifier == 0)
	{
		const auto& The_Light = scene_->getAllSpotLights()[which_one];

		GLuint light_intensity_id = glGetUniformLocation(program_id, std::string("One_Spot_Light.light_intensity").c_str());
		GLuint light_position_id = glGetUniformLocation(program_id, std::string("One_Spot_Light.light_position").c_str());
		GLuint light_range_id = glGetUniformLocation(program_id, std::string("One_Spot_Light.light_range").c_str());
		GLuint light_direction_id = glGetUniformLocation(program_id, std::string("One_Spot_Light.light_direction").c_str());
		GLuint light_cone_angle_id = glGetUniformLocation(program_id, std::string("One_Spot_Light.cone_angle").c_str());
		GLuint light_type_id = glGetUniformLocation(program_id, "type_of_light");

		float cone_angle_ = glm::radians(The_Light.getConeAngleDegrees());

		glm::vec3 light_position = (const glm::vec3 &)The_Light.getPosition();
		glm::vec3 light_direction = (const glm::vec3 &)The_Light.getDirection();
		sponza::Vector3 up = sponza::Vector3{ 0, 1, 0 };
		float radius = The_Light.getRange() * glm::tan(glm::radians(cone_angle_ / 2));

		glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(0, 0, -1));
		glm::mat4 rotate = glm::inverse(glm::lookAt((const glm::vec3 &)light_position, (const glm::vec3 &)light_position + (const glm::vec3 &)light_direction, (const glm::vec3 &)up));
		glm::mat4 scale = glm::scale(glm::mat4(), glm::vec3(radius, radius, The_Light.getRange()));
 
		glm::mat4 model_xform = rotate * scale * translate;

		glm::mat4 combined_xform = projection_xform_ * view_xform_ * model_xform;

		GLuint combined_xform_id = glGetUniformLocation(program_id, "combined_xform");
		glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

		glUniform3fv(light_intensity_id, 1, glm::value_ptr((const glm::vec3 &)The_Light.getIntensity()));
		glUniform3fv(light_position_id, 1, glm::value_ptr((const glm::vec3 &)The_Light.getPosition()));
		glUniform1f(light_range_id, The_Light.getRange());
		glUniform3fv(light_direction_id, 1, glm::value_ptr((const glm::vec3 &)The_Light.getDirection()));
		glUniform1f(light_cone_angle_id, cone_angle_);
		glUniform1i(light_type_id, 0);

	}
	if (light_identifier == 1)
	{
		const auto& The_Light = scene_->getAllPointLights()[which_one];

		GLuint light_intensity_id = glGetUniformLocation(program_id, std::string("One_Point_Light.light_intensity").c_str());
		GLuint light_position_id = glGetUniformLocation(program_id, std::string("One_Point_Light.light_position").c_str());
		GLuint light_range_id = glGetUniformLocation(program_id, std::string("One_Point_Light.light_range").c_str());
		GLuint light_type_id = glGetUniformLocation(program_id, "type_of_light");

		glm::mat4 identity_matrix = { 1.0f,									0.0f,						0.0f,			 0.0f,
			0.0f,									1.0f,						0.0f,			 0.0f,
			0.0f,									0.0f,						1.0f,			 0.0f,
			0.0f,									0.0f,						0.0f,			 1.0f };

		glm::vec3 light_position = glm::vec3(The_Light.getPosition().x, The_Light.getPosition().y, The_Light.getPosition().z);

		glm::mat4 model_xform = glm::translate(identity_matrix, light_position) * glm::scale(identity_matrix, glm::vec3(The_Light.getRange(), The_Light.getRange(), The_Light.getRange()));

		glm::mat4 combined_xform = projection_xform_ * view_xform_ * model_xform;

		GLuint combined_xform_id = glGetUniformLocation(program_id, "combined_xform");
		glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

		glUniform3fv(light_intensity_id, 1, glm::value_ptr((const glm::vec3 &)The_Light.getIntensity()));
		glUniform3fv(light_position_id, 1, glm::value_ptr((const glm::vec3 &)The_Light.getPosition()));
		glUniform1f(light_range_id, The_Light.getRange());
		glUniform1i(light_type_id, 1);
	}

	if (light_identifier == 2)
	{
		const auto& The_Light = scene_->getAllDirectionalLights()[which_one];

		GLuint light_direction_id = glGetUniformLocation(program_id, std::string("One_Directional_Light.light_direction").c_str());
		GLuint light_intensity_id = glGetUniformLocation(program_id, std::string("One_Directional_Light.light_intensity").c_str());
		GLuint light_type_id = glGetUniformLocation(program_id, "type_of_light");

		glUniform3fv(light_direction_id, 1, glm::value_ptr((const glm::vec3 &)The_Light.getDirection()));
		glUniform3fv(light_intensity_id, 1, glm::value_ptr((const glm::vec3 &)The_Light.getIntensity()));
		glUniform1i(light_type_id, 2);

	}
}

void MyView::Create_Shader_Program(std::string name_of_shader_file, GLuint & program_id)
{
	GLint compile_status = 0;

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertex_shader_string
		= tygra::createStringFromFile("resource:///" + name_of_shader_file + "_vs.glsl");
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
	std::string fragment_shader_string =
		tygra::createStringFromFile("resource:///" + name_of_shader_file + "_fs.glsl");
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

	program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader);
	glBindAttribLocation(program_id, kVertexPosition, "vertex_position");
	glBindAttribLocation(program_id, kVertexNormal, "vertex_normal");
	glDeleteShader(vertex_shader);
	glAttachShader(program_id, fragment_shader);
	glBindFragDataLocation(program_id, kFragmentColour, "reflected_light");
	glDeleteShader(fragment_shader);
	glLinkProgram(program_id);

	GLint link_status = 0;
	glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetProgramInfoLog(program_id, string_length, NULL, log);
		std::cerr << log << std::endl;
	}


}
