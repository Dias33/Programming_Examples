#version 330

in vec3 P;
in vec3 N;
in vec3 ambient_light_;
in vec3 kd_;

out vec3 position;
out vec3 normal;
out vec3 ambient_light;
out vec3 kd;

void main(void)
{
	position = P;
	normal = N;

	ambient_light = ambient_light_;
	kd = kd_;
}
