CARGS = -Os -Wall -Isrc -isystem C:\raylib\src -DPLATFORM_WEB
LINKARGS = -s USE_GLFW=3 -LC:\raylib\src\libraylib.a
SRCFILES = noise wander_main tilegen vector_math

info:
	@echo Common targets: wander, clean

build/%.o: src/%.cc src/*.h
	@if not exist build mkdir build
	em++ $(CARGS) -c $< -o $@

wander: $(addprefix build/, $(addsuffix .o, $(SRCFILES)))
	em++ $(CARGS) $(LINKARGS) $? C:\raylib\src\libraylib.a --shell-file C:\emsdk\upstream\emscripten\src\shell.html  -o build\wander_demo.html