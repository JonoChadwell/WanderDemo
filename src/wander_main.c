#include <raylib.h>
#include <emscripten/emscripten.h>

const int kWidth = 800;
const int kHeight = 600;

void loop(void)
{
    BeginDrawing();
	ClearBackground(RAYWHITE);
	DrawText("Hello World!", 190, 200, 20, LIGHTGRAY);
    EndDrawing();
}

int main(int argc, char** argv)
{
    InitWindow(kWidth, kHeight, "wander demo");

    emscripten_set_main_loop(loop, 0, 1);

    CloseWindow();

    return 0;
}
