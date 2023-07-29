#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <assert.h>
#include <raylib.h>

#define SEED_RADIUS 5.0f
#define SEED_COLOR (Color){33, 144, 171, 255}

typedef struct
{
	Vector2 position;
	Color color;
} voronoi_seed;

#define SEEDS_COUNT 10
voronoi_seed seeds[SEEDS_COUNT] = {0};

typedef char display_underlying_type;
// expect less than 127 seeds
display_underlying_type* display = NULL;
int display_width = 0;
int display_height = 0;

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

void realloc_display(int width, int height)
{
	if (width == display_width && height == display_height)
		goto reset_values;

	if (display != NULL)
		free(display);

	display = (display_underlying_type*)malloc(width * height * sizeof(display_underlying_type));

	display_width = width;
	display_height = height;

reset_values:
	for (int i = 0; i < display_height * display_width; i++)
		display[i] = -1;
}

int get_display_value(int x, int y)
{
	return display[y * display_width + x];
}

void set_display_value(int x, int y, int value)
{
	display[y * display_width + x] = value;
}

float euclidean_distance(Vector2 first, Vector2 second)
{
	Vector2 diff;
	diff.x = second.x - first.x;
	diff.y = second.y - first.y;

	return sqrtf(diff.x * diff.x + diff.y * diff.y);
}

float manhattan_distance(Vector2 first, Vector2 second)
{
	return fabsf(second.x - first.x) + fabsf(second.y - first.y);
}

float chebyshev_distance(Vector2 first, Vector2 second)
{
	float x_diff = fabsf(first.x - second.x);
	float y_diff = fabsf(first.y - second.y);

	return fmaxf(x_diff, y_diff);
}


#define MINKOWSKI_ORDER 4
float minkowski_distance(Vector2 first, Vector2 second)
{
	return powf(powf(fabsf(first.x - second.x), MINKOWSKI_ORDER) + powf(fabsf(first.y - second.y), MINKOWSKI_ORDER), 1.0f / MINKOWSKI_ORDER);
}

float distance(Vector2 first, Vector2 second)
{
	switch (current_distance_type)
	{
		case EUCLIDEAN_DISTANCE:
			return euclidean_distance(first, second);
		case MANHATTAN_DISTANCE:
			return manhattan_distance(first, second);
		case CHEBYSHEV_DISTANCE:
			return chebyshev_distance(first, second);
		case MINKOWSKI_DISTANCE:
			return minkowski_distance(first, second);

		default:
			printf("Unreachable: distance");
			break;
	}

	return 0.0f;
}


void voronoi(void)
{
	for (int y = 0; y < display_height ; y++)
	{
		for (int x = 0; x < display_width; x++)
		{
			int pixel_index = get_display_value(x, y);
			if (pixel_index >= 0)
				continue;

			float min_distance = FLT_MAX;
			int seed_index = 0;
			for (int i = 0; i < SEEDS_COUNT; i++)
			{
				float dist = distance((Vector2){x, y}, seeds[i].position);
				if (dist < min_distance)
				{
					min_distance = dist;
					seed_index = i;
				}
			}

			if (min_distance < animation_distance)
				set_display_value(x, y, seed_index);
		}
	}
}

int main(void)
{
	srand(time(NULL));

	InitWindow(800, 480, "Voronoi diagram");
	SetTargetFPS(60);

	realloc_display(800, 480);

	while (!WindowShouldClose())
	{
		if (IsKeyPressed(KEY_H))
		{
			for (int i = 0; i < SEEDS_COUNT; i++)
			{
				seeds[i].position.x = random_float(0.0f, GetScreenWidth());
				seeds[i].position.y = random_float(0.0f, GetScreenHeight());
				seeds[i].color = random_color();
			}

			realloc_display(GetScreenWidth(), GetScreenHeight());
			animation_distance = 0.0f;
		}

		if (IsKeyPressed(KEY_SPACE))
		{
			current_distance_type += 1;
			if (current_distance_type == DISTANCE_TYPE_MAX)
				current_distance_type = 0;
			realloc_display(GetScreenWidth(), GetScreenHeight());
			animation_distance = 0.0f;
		}

		animation_distance += GetFrameTime() * 100;
		voronoi();

		BeginDrawing();
		ClearBackground(WHITE);
		
		for (int y = 0; y < display_height; y++)
		{
			for (int x = 0; x < display_width; x++)
			{
				int index = get_display_value(x, y);
				if (index >= 0)
					DrawPixel(x, y, seeds[index].color);
			}
		}

		for (int i = 0; i < SEEDS_COUNT; i++)
			DrawCircleV(seeds[i].position, SEED_RADIUS, SEED_COLOR);

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

	free(display);
	CloseWindow();
	return 0;
}
