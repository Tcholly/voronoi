#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
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

Image display;

float animation_distance = 0.0f;

float random_float(float min, float max)
{
	int r = rand();
	printf("%d\n", r);

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

float distance(Vector2 first, Vector2 second)
{
	Vector2 diff;
	diff.x = second.x - first.x;
	diff.y = second.y - first.y;

	return sqrtf(diff.x * diff.x + diff.y * diff.y);
}

void voronoi(void)
{
	for (int y = 0; y < display.height ; y++)
	{
		for (int x = 0; x < display.width; x++)
		{
			Color image_pixel = GetImageColor(display, x, y);
			if (image_pixel.r != 255 || image_pixel.g != 255 || image_pixel.b != 255)
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
				ImageDrawPixel(&display, x, y, seeds[seed_index].color);
		}
	}

}

int main(void)
{
	srand(time(NULL));

	InitWindow(800, 480, "Voronoi diagram");
	SetTargetFPS(60);

	display = GenImageColor(800, 480, WHITE);

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

			UnloadImage(display);
			display = GenImageColor(GetScreenWidth(), GetScreenHeight(), WHITE);
			animation_distance = 0.0f;
		}

		animation_distance += GetFrameTime() * 100;
		voronoi();

		Texture2D display_texture = LoadTextureFromImage(display);
		BeginDrawing();
		ClearBackground(WHITE);
		
		DrawTexture(display_texture, 0, 0, WHITE);

		for (int i = 0; i < SEEDS_COUNT; i++)
			DrawCircleV(seeds[i].position, SEED_RADIUS, SEED_COLOR);

		EndDrawing();
		UnloadTexture(display_texture);
	}

	CloseWindow();
	return 0;
}
