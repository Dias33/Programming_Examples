#version 330

uniform sampler2DRect sampler_world_position;
uniform sampler2DRect sampler_world_normal;
uniform sampler2DRect ambient_light;
uniform sampler2DRect kd;

struct Directional_Light
{
	vec3 light_intensity;
	vec3 light_direction;
};

uniform Directional_Light One_Directional_Light;
uniform int type_of_light;

out vec3 reflected_light;

in vec4 gl_FragCoord;

vec3 Calculate_Directional_Light(vec3 light_direction_, vec3 N_, vec3 object_colour)
{
	//vec3 kd = vec3(0.8f, 0.8f, 0.8f);

	vec3 L = normalize(-light_direction_);

	float LN = clamp(dot(L, normalize(N_)), 0.0, 1.0);

	vec3 reflected_light = object_colour * LN;

	return reflected_light;

}

void main(void)
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	vec4 position_ = texelFetch(sampler_world_position, pixel_coord);
	vec4 normal_ = texelFetch(sampler_world_normal, pixel_coord);
	vec3 ambient_light_ = vec3(texelFetch(ambient_light, pixel_coord));
	vec3 kd_ = vec3(texelFetch(kd, pixel_coord));

	vec3 P = vec3(position_);
	vec3 N = normalize(vec3(normal_));

	vec3 final_light = Calculate_Directional_Light(One_Directional_Light.light_direction, N, kd_);

    reflected_light = final_light * One_Directional_Light.light_intensity + ambient_light_ * kd_;
}
