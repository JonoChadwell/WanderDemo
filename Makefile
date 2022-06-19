CARGS = -Os -Wall -Werror -Isrc -isystem C:\raylib\src -DPLATFORM_WEB
LINKARGS = -s USE_GLFW=3 -LC:\raylib\src\libraylib.a

info:
	echo "Common targets: wander, clean"

build/%.o: src/%.c src/*.h
	if not exist build mkdir build
	emcc $(CARGS) -c $< -o $@

wander: build/*.o
	emcc $(CARGS) $(LINKARGS) $? C:\raylib\src\libraylib.a --shell-file C:\emsdk\upstream\emscripten\src\shell.html  -o build\wander_demo.html