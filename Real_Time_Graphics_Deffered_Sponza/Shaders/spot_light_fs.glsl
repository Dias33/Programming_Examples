#version 330

uniform sampler2DRect sampler_world_position;
uniform sampler2DRect sampler_world_normal;

struct Spot_Light
{
	vec3 light_position;
	vec3 light_intensity;
	vec3 light_direction;

	float cone_angle;
	float light_range;

};

uniform Spot_Light One_Spot_Light;
uniform int type_of_light;

out vec3 reflected_light;

in vec4 gl_FragCoord;

vec3 Calculate_Spot_Light(vec3 light_position, vec3 light_direction, float light_range, float cone_angle, vec3 P, vec3 N)
{
	vec3 intensity = vec3(0, 0, 0);
	vec3 kd = vec3(0.8f, 0.8f, 0.8f);

	vec3 L = normalize(light_position - P);
	float d = distance(light_position, P);
	float fd = 1.0 - smoothstep(0, light_range, d);

	//float fc = float(dot(-L, light_direction) > cos(0.5 * cone_angle));
	//float fc = step(cos(0.5 * cone_angle), dot(-L, light_direction));
	float fc = smoothstep(cos(0.5 * cone_angle), 1, dot(-L, light_direction));

	float LN = clamp(dot(L, normalize(N)), 0.0, 1.0);

	vec3 Id = fd * fc * LN * kd;

	return Id;

}

void main(void)
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	vec4 position_ = texelFetch(sampler_world_position, pixel_coord);
	vec4 normal_ = texelFetch(sampler_world_normal, pixel_coord);

	vec3 P = vec3(position_);
	vec3 N = normalize(vec3(normal_));

	vec3 spot_light = Calculate_Spot_Light(One_Spot_Light.light_position, One_Spot_Light.light_direction, One_Spot_Light.light_range, One_Spot_Light.cone_angle, P, N);

    reflected_light = spot_light * One_Spot_Light.light_intensity;
}
