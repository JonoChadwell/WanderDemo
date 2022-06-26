#include <raylib.h>
#include <emscripten/emscripten.h>

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "noise.h"
#include "tilegen.h"


const int kWidth = 800;
const int kHeight = 600;

Camera gCamera = { 0 };
Texture2D gTexture = { 0 };
struct TileChunk gChunk = { 0 };

void draw_centered_grid(Vector3 pos) {
	static const int kCount = 5;
	static const double kSpacing = 1.0;
	static const Color kColor = GRAY;

	double x = round(pos.x);
	double y = pos.y;
	double z = round(pos.z);
	for (int row = -kCount; row <= kCount; row++) {
		Vector3 start = {x + row * kSpacing, y, z - kCount * kSpacing};
		Vector3 end = {x + row * kSpacing, y, z + kCount * kSpacing};
		DrawLine3D(start, end, kColor);
	}
	for (int col = -kCount; col <= kCount; col++) {
		Vector3 start = {x - kCount * kSpacing, y, z + col * kSpacing};
		Vector3 end = {x + kCount * kSpacing, y, z + col * kSpacing};
		DrawLine3D(start, end, kColor);
	}
}

void draw_color_splotches(Vector3 pos) {
	static const int kCount = 5;
	static const double kSpacing = 1.0;
	static const double kRadius = 0.4;
	static const double kNoiseScale = 0.1;

	double x = round(pos.x);
	double y = pos.y;
	double z = round(pos.z);
	
	for (int row = -kCount; row <= kCount; row++) {
		for (int col = -kCount; col <= kCount; col++) {
			Vector3 center = {x + row * kSpacing, y, z + col * kSpacing};
			double noiseVal = noise(center.x * kNoiseScale, center.z * kNoiseScale);
			Color color = {
				.r = noiseVal > 0 ? 0 : (char) (noiseVal * -255),
				.g = noiseVal < 0 ? 0 : (char) (noiseVal * 255), 
				.b = 0,//0,
				.a = 255,
			};
			
			DrawCylinder(center, kRadius, kRadius, 0.01, 10, color);
		}
	}
}

void draw_outlined_cube(Vector3 center, float w, float h, float l, Color color) {
	DrawCube(center, w, h, l, color);
	Vector3 nw = {center.x - w / 2, center.y + h / 2 + .01, center.z - l / 2};
	Vector3 ne = {center.x - w / 2, center.y + h / 2 + .01, center.z + l / 2};
	Vector3 sw = {center.x + w / 2, center.y + h / 2 + .01, center.z - l / 2};
	Vector3 se = {center.x + w / 2, center.y + h / 2 + .01, center.z + l / 2};
	DrawLine3D(nw, ne, BLACK);
	DrawLine3D(ne, se, BLACK);
	DrawLine3D(se, sw, BLACK);
	DrawLine3D(sw, nw, BLACK);
}

void draw_chunk() {
	for (int row = 0; row < TILE_CHUNK_SIZE; row++) {
		for (int col = 0; col < TILE_CHUNK_SIZE; col++) {
			Vector3 center = {col - 16, 0, row - 16};
			
			switch (chunk_get(&gChunk, row, col)) {
				case DEEP:
					draw_outlined_cube(center, 1, 1, 1, DARKBLUE);
					break;
				case WATER:
					draw_outlined_cube(center, 1, 1, 1, BLUE);
					break;
				case SAND:
					draw_outlined_cube(center, 1, 1.5, 1, BEIGE);
					break;
				case GRASS:
					draw_outlined_cube(center, 1, 2, 1, GREEN);
					break;
				case TREE:
					draw_outlined_cube(center, 1, 2, 1, GREEN);
					DrawCylinder(center, 0.1, 0.5, 5, 6, DARKGREEN);
					break;
				case HILL:
					draw_outlined_cube(center, 1, 4, 1, BROWN);
					break;
				case TILE_NULL:
					draw_outlined_cube(center, 0.5, 0.5, 0.5, MAGENTA);
					break;
				default:
					break;
			}
			
			
		}
	}
}

void loop(void)
{
	const float kSpeed = 0.20f;
	if (IsKeyDown(KEY_RIGHT)) {
		gCamera.position.x += kSpeed;
		gCamera.target.x += kSpeed;
	}
	if (IsKeyDown(KEY_LEFT)) {
		gCamera.position.x -= kSpeed;
		gCamera.target.x -= kSpeed;
	}
	if (IsKeyDown(KEY_UP)) {
		gCamera.position.z -= kSpeed;
		gCamera.target.z -= kSpeed;
	}
	if (IsKeyDown(KEY_DOWN)) {
		gCamera.position.z += kSpeed;
		gCamera.target.z += kSpeed;
	}
	if (IsKeyPressed(KEY_W)) {
		struct TileChunk nc;
		chunk_north(&gChunk, &nc);
		gChunk = nc;
	}
	if (IsKeyPressed(KEY_A)) {
		struct TileChunk nc;
		chunk_west(&gChunk, &nc);
		gChunk = nc;
	}
	if (IsKeyPressed(KEY_S)) {
		struct TileChunk nc;
		chunk_south(&gChunk, &nc);
		gChunk = nc;
	}
	if (IsKeyPressed(KEY_D)) {
		struct TileChunk nc;
		chunk_east(&gChunk, &nc);
		gChunk = nc;
	}
	

    BeginDrawing();
	
		ClearBackground(RAYWHITE);
		DrawTexture(gTexture, 0, 0, PINK);
		
			BeginMode3D(gCamera);
			
				// DrawCube((Vector3){-4.0f, 0.0f, 2.0f}, 2.0f, 5.0f, 2.0f, RED);
				// DrawCubeWires((Vector3){-4.0f, 0.0f, 2.0f}, 2.0f, 5.0f, 2.0f, GOLD);
				// DrawCubeWires((Vector3){-4.0f, 0.0f, -2.0f}, 3.0f, 6.0f, 2.0f, MAROON);

				// DrawSphere((Vector3){-1.0f, 0.0f, -2.0f}, 1.0f, GREEN);
				// DrawSphereWires((Vector3){1.0f, 0.0f, 2.0f}, 2.0f, 16, 16, LIME);

				// DrawCylinder((Vector3){4.0f, 0.0f, -2.0f}, 1.0f, 2.0f, 3.0f, 4, SKYBLUE);
				// DrawCylinderWires((Vector3){4.0f, 0.0f, -2.0f}, 1.0f, 2.0f, 3.0f, 4, DARKBLUE);
				// DrawCylinderWires((Vector3){4.5f, -1.0f, 2.0f}, 1.0f, 1.0f, 2.0f, 6, BROWN);

				// DrawCylinder((Vector3){1.0f, 0.0f, -4.0f}, 0.0f, 1.5f, 3.0f, 8, GOLD);
				// DrawCylinderWires((Vector3){1.0f, 0.0f, -4.0f}, 0.0f, 1.5f, 3.0f, 8, PINK);
				
				
				// draw_centered_grid(gCamera.target);
				// draw_color_splotches(gCamera.target);
				draw_chunk();
			
			EndMode3D();
		
		DrawFPS(10, 10);

    EndDrawing();
}

void load(void) {
	Image cellular = GenImageCellular(kWidth, kHeight, 60);
	gTexture = LoadTextureFromImage(cellular);
	chunk_generate_root(&gChunk);

	gCamera.position = (Vector3){ 0.0f, 45.0f, 10.0f };
    gCamera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    gCamera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    gCamera.fovy = 45.0f;
    gCamera.projection = CAMERA_PERSPECTIVE;
}

int main(int argc, char** argv) {
    InitWindow(kWidth, kHeight, "wander demo");
	load();
	SetTargetFPS(60);

    emscripten_set_main_loop(loop, 0, 1);
    CloseWindow();
    return 0;
}
