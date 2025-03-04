COMPILER = clang++

SOURCE_LIBS = -Ilib/

OSX_OPT = -std=c++17 -Llib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib.a

OSX_OUT = -o "bin/build_osx"

CFILES = main.cpp player_state_machine.cpp enemy_state_machine.cpp

build_osx: 
	$(COMPILER) $(CFILES) $(SOURCE_LIBS) $(OSX_OUT) $(OSX_OPT)