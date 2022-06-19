wander:
	if not exist build mkdir build
	emcc -o build\wander_demo.html src\wander_main.c -Os -Wall C:\raylib\src\libraylib.a -Isrc -isystem C:\raylib\src -L. -LC:\raylib\src\libraylib.a -s USE_GLFW=3 --shell-file C:\emsdk\upstream\emscripten\src\shell.html -DPLATFORM_WEB
