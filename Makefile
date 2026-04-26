CARGS = -Os -Wall -Isrc -isystem C:\raylib\src -DPLATFORM_WEB
LINKARGS = -s USE_GLFW=3 -LC:\raylib\src\libraylib.a
SRCFILES = music_wizard_main vector_math

info:
	@echo Common targets: music_wizard, clean

build/%.o: src/%.cc src/*.h
	@if not exist build mkdir build
	em++ $(CARGS) -c $< -o $@

music_wizard: $(addprefix build/, $(addsuffix .o, $(SRCFILES)))
	em++ $(CARGS) $(LINKARGS) $? C:\raylib\src\libraylib.a --shell-file C:\emsdk\upstream\emscripten\src\shell.html  -o build\music_wizard.html