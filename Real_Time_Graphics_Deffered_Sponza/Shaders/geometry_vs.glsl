#version 330

layout(std140) uniform PerModelUniforms
{
	mat4 combined_xform;
    mat4 model_xform;
};

in vec3 vertex_position;
in vec3 vertex_normal;

uniform vec3 amb_light;
uniform vec3 object_colour;

out vec3 P;
out vec3 N;
out vec3 ambient_light_;
out vec3 kd_;

void main(void)
{
	P = mat4x3(model_xform) * vec4(vertex_position, 1.0);
	N = normalize(mat3(model_xform) * vertex_normal);

	ambient_light_ = amb_light;
	kd_ = object_colour;

	gl_Position = combined_xform * vec4(vertex_position, 1.0);
}

