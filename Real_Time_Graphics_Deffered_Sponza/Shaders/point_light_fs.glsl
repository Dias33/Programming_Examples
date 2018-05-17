#version 330

uniform sampler2DRect sampler_world_position;
uniform sampler2DRect sampler_world_normal;

struct Point_Light
{
	vec3 light_intensity;
	vec3 light_position;
	float light_range;
};

uniform Point_Light One_Point_Light;
uniform int type_of_light;

out vec3 reflected_light;

in vec4 gl_FragCoord;

vec3 Calculate_Point_Light(vec3 light_position_, float light_range, vec3 P, vec3 N)
{
	vec3 kd = vec3(0.8f, 0.8f, 0.8f);

	vec3 L = normalize(light_position_ - P); // direction to incoming light
	float LN = clamp(dot(L, normalize(N)), 0.0, 1.0);

	float d = distance(light_position_, P);

	float fd = 1.0 - smoothstep(light_range - (light_range / 4), light_range, d);

	vec3 Id = LN * fd * kd;

	return Id;
}

void main(void)
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	vec4 position_ = texelFetch(sampler_world_position, pixel_coord);
	vec4 normal_ = texelFetch(sampler_world_normal, pixel_coord);

	vec3 P = vec3(position_);
	vec3 N = normalize(vec3(normal_));

	vec3 point_light = Calculate_Point_Light(One_Point_Light.light_position, One_Point_Light.light_range, P, N);

    reflected_light = point_light * One_Point_Light.light_intensity;
}
