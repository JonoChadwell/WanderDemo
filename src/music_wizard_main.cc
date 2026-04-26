#include <raylib.h>
#include <raymath.h>
#include <emscripten/emscripten.h>

#include <cmath>
#include <cstdio>
#include <vector>
#include <memory>
#include <vector>

#include "common.h"
#include "vector_math.h"


const int kWidth = 800;
const int kHeight = 600;

Camera gCamera{};
Texture2D gTexture{};


void draw_character() {
	// DrawSphere(gCharacterPos, 1.0f, PINK);
}

void handle_input() {
	// static const float kTargetMouseDistance = 2.0f;
	// static const float kCharacterSpeed = 0.2f;
	
	// Vector2 mouse_pos_screen = GetMousePosition();
	// Ray mouse_pos_world = GetMouseRay(mouse_pos_screen, gCamera);
	
	// float cast_amount = (gCharacterPos.y - mouse_pos_world.position.y)
	// 		/ mouse_pos_world.direction.y;
	// Vector3 mouse_pos_character_plane = {
	// 	mouse_pos_world.position.x + cast_amount * mouse_pos_world.direction.x,
	// 	gCharacterPos.y,
	// 	mouse_pos_world.position.z + cast_amount * mouse_pos_world.direction.z,
	// };
	
	// // Check for fire charge / discharge
	// if (IsMouseButtonPressed(0)) {
	// 	// TODO
	// }
	
	// // Move character.
	// gCharacterPos = v3_move_closer(
	// 		gCharacterPos, mouse_pos_character_plane, kCharacterSpeed,
	// 		kTargetMouseDistance);
	// Vector3 target_pos = v3_move_towards(
	// 		mouse_pos_character_plane, gCharacterPos, kTargetMouseDistance);
	// gCharacterPos = v3_move_towards(
	// 		gCharacterPos, target_pos, kCharacterSpeed);

	// // Move screen.
	// Vector3 character_xz = gCharacterPos;
	// character_xz.y = 0;

	// Vector3 new_target = v3_move_towards(character_xz, gCamera.target, 5.0f);
	// Vector3 camera_delta = Vector3Subtract(new_target, gCamera.target);
	
	// gCamera.target = Vector3Add(gCamera.target, camera_delta);
	// gCamera.position = Vector3Add(gCamera.position, camera_delta);
	
	// static const float kSpeed = 0.4f;
	// if (IsKeyDown(KEY_RIGHT)) {
		// gCamera.position.x += kSpeed;
		// gCamera.target.x += kSpeed;
	// }
	// if (IsKeyDown(KEY_LEFT)) {
		// gCamera.position.x -= kSpeed;
		// gCamera.target.x -= kSpeed;
	// }
	// if (IsKeyDown(KEY_UP)) {
		// gCamera.position.z -= kSpeed;
		// gCamera.target.z -= kSpeed;
	// }
	// if (IsKeyDown(KEY_DOWN)) {
		// gCamera.position.z += kSpeed;
		// gCamera.target.z += kSpeed;
	// }
}

void loop() {
	// handle_input();
	// recenter();

    BeginDrawing();
	
		ClearBackground(RAYWHITE);
		// DrawTexture(gTexture, 0, 0, PINK);
		Vector2 ball_pos = { 100, 100 };
		DrawCircleV(ball_pos, 10, RED);
		
		DrawFPS(10, 10);

    EndDrawing();
}

void load() {
	Image cellular = GenImageCellular(kWidth, kHeight, 60);
	gTexture = LoadTextureFromImage(cellular);

	gCamera.position = Vector3{ 0.0f, 15.0f, 20.0f };
    gCamera.target = Vector3{ 0.0f, 0.0f, 0.0f };
    gCamera.up = Vector3{ 0.0f, 1.0f, 0.0f };
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

