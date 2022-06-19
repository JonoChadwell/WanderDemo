#include <raylib.h>
#include <emscripten/emscripten.h>

#include <stdio.h>

#include "noise.h"


const int kWidth = 800;
const int kHeight = 600;

void loop(void)
{
    BeginDrawing();
	ClearBackground(RAYWHITE);
	DrawText("What a World!", 190, 200, 20, LIGHTGRAY);
    EndDrawing();
}

int main(int argc, char** argv)
{
    InitWindow(kWidth, kHeight, "wander demo");

	printf("Noise: %f\n", noise(123,123));
	
    emscripten_set_main_loop(loop, 0, 1);

    CloseWindow();

    return 0;
}
