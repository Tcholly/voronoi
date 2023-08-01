#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <raylib.h>

#define SEED_RADIUS 3.0f
#define SEED_COLOR (Color){252, 201, 171, 255}

typedef struct
{
	int position_loc;
	int color_loc;
} voronoi_seed;

#define SEEDS_COUNT 64
voronoi_seed seeds[SEEDS_COUNT] = {0};

typedef enum
{
	EUCLIDEAN_DISTANCE = 0,
	MANHATTAN_DISTANCE,
	CHEBYSHEV_DISTANCE,
	MINKOWSKI_DISTANCE,

	DISTANCE_TYPE_MAX
} distance_type;
static_assert(DISTANCE_TYPE_MAX == 4, "Remember to implement all distances types");

distance_type current_distance_type = EUCLIDEAN_DISTANCE;
float animation_distance = 0.0f;

float random_float(float min, float max)
{
	int r = rand();

	return ((float)r / (float)RAND_MAX) * (max - min) + min;
}

Color random_color()
{
	Color result = {.a = 255};
	result.r = rand() % 256;
	result.g = rand() % 256;
	result.b = rand() % 256;

	return result;
}

int main(void)
{
	srand(time(NULL));

	InitWindow(800, 480, "Voronoi diagram");
	SetTargetFPS(60);

	Shader voronoi_shader = LoadShader(NULL, "Assets/voronoi.frag");
	for (int i = 0; i < SEEDS_COUNT; i++)
	{
		const char* pos_name = TextFormat("seeds[%i].position", i);
		seeds[i].position_loc = GetShaderLocation(voronoi_shader, pos_name);

		const char* color_name = TextFormat("seeds[%i].color", i);
		seeds[i].color_loc = GetShaderLocation(voronoi_shader, color_name);
	}

	int screen_height_loc = GetShaderLocation(voronoi_shader, "screen_height");
	int distance_type_loc = GetShaderLocation(voronoi_shader, "distance_type");
	SetShaderValue(voronoi_shader, distance_type_loc, &current_distance_type, SHADER_UNIFORM_INT);
	int animation_time_loc = GetShaderLocation(voronoi_shader, "animation_time");
	int seeds_count_loc = GetShaderLocation(voronoi_shader, "seeds_count");
	int seeds_count = SEEDS_COUNT;
	SetShaderValue(voronoi_shader, seeds_count_loc, &seeds_count, SHADER_UNIFORM_INT);

	while (!WindowShouldClose())
	{
		if (IsKeyPressed(KEY_H))
		{
			for (int i = 0; i < SEEDS_COUNT; i++)
			{
				Vector2 position;
				position.x = random_float(0.0f, GetScreenWidth());
				position.y = random_float(0.0f, GetScreenHeight());
				Color color = random_color();

				SetShaderValue(voronoi_shader, seeds[i].position_loc, &position, SHADER_UNIFORM_VEC2);
				float color_float[4] = {
					(float)color.r / 255.0f,
					(float)color.g / 255.0f,
					(float)color.b / 255.0f,
					(float)color.a / 255.0f
				};
				SetShaderValue(voronoi_shader, seeds[i].color_loc, &color_float, SHADER_UNIFORM_VEC4);
			}

			animation_distance = 0.0f;
		}

		if (IsKeyPressed(KEY_SPACE))
		{
			current_distance_type += 1;
			if (current_distance_type == DISTANCE_TYPE_MAX)
				current_distance_type = 0;
			animation_distance = 0.0f;

			SetShaderValue(voronoi_shader, distance_type_loc, &current_distance_type, SHADER_UNIFORM_INT);
		}

		if (IsKeyPressed(KEY_S))
		{
			time_t timer;
    		char buffer[26];
    		struct tm* tm_info;

    		timer = time(NULL);
    		tm_info = localtime(&timer);

    		strftime(buffer, 26, "%d-%m-%Y_%H:%M:%S", tm_info);

			TakeScreenshot(TextFormat("Screenshots/%s.png", buffer));
		}

		animation_distance += GetFrameTime() * 100;

		BeginDrawing();
		ClearBackground(WHITE);

		float screen_height = GetScreenHeight();
		SetShaderValue(voronoi_shader, screen_height_loc, &screen_height, SHADER_UNIFORM_FLOAT);
		SetShaderValue(voronoi_shader, animation_time_loc, &animation_distance, SHADER_UNIFORM_FLOAT);
		BeginShaderMode(voronoi_shader);
		DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), RED);
		EndShaderMode();
		

		const char* distance_type_text;

		switch (current_distance_type)
		{
			case EUCLIDEAN_DISTANCE:
				distance_type_text = "Euclidean distance";
				break;
			case MANHATTAN_DISTANCE:
				distance_type_text = "Manhattan distance";
				break;
			case CHEBYSHEV_DISTANCE:
				distance_type_text = "Chebyshev distance";
				break;
			case MINKOWSKI_DISTANCE:
				distance_type_text = "Minkowski distance";
				break;

			default:
				distance_type_text = "Unknown distance";
				break;
		}

		DrawText(distance_type_text, 10, 10, 20, LIGHTGRAY);


		EndDrawing();
	}

	UnloadShader(voronoi_shader);

	CloseWindow();
	return 0;
}
