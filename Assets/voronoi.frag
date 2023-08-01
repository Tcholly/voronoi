#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

// NOTE: Add here your custom variables

struct Seed
{
	vec2 position;
	vec4 color;
};
#define MAX_SEEDS 64 
uniform Seed seeds[MAX_SEEDS];
uniform int seeds_count;
uniform float screen_height;
uniform float animation_time;
uniform int distance_type;

const vec4 seed_color = vec4(0.1, 0.1, 0.1, 1.0);

float manhattan_distance(vec2 first, vec2 second)
{
	return abs(second.x - first.x) + abs(second.y - first.y);
}

float chebyshev_distance(vec2 first, vec2 second)
{
	float x_diff = abs(first.x - second.x);
	float y_diff = abs(first.y - second.y);

	return max(x_diff, y_diff);
}

#define MINKOWSKI_ORDER 4
float minkowski_distance(vec2 first, vec2 second)
{
	return pow(pow(abs(first.x - second.x), MINKOWSKI_ORDER) + pow(abs(first.y - second.y), MINKOWSKI_ORDER), 1.0f / MINKOWSKI_ORDER);
}


void main()
{
    vec2 position = vec2(gl_FragCoord.x, screen_height - gl_FragCoord.y);

	float min_distance = 1000000000.0;
	int seed_index = 0;
	for (int i = 0; i < seeds_count; i++)
	{
		float dist = 0.0;
		switch (distance_type)
		{
			case 0:
				dist = distance(position, seeds[i].position);
				break;
			case 1:
				dist = manhattan_distance(position, seeds[i].position);
				break;
			case 2:
				dist = chebyshev_distance(position, seeds[i].position);
				break;
			case 3:
				dist = minkowski_distance(position, seeds[i].position);
				break;

			default:
				dist = distance(position, seeds[i].position);
				break;
		}

		if (dist < min_distance)
		{
			min_distance = dist;
			seed_index = i;
		}
	}

	if (min_distance < 3.0)
		finalColor = seed_color;
	else if (min_distance < animation_time)
		finalColor = seeds[seed_index].color;
	else
		finalColor = vec4(1.0);
}
