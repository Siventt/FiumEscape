EXEC_NAME ?= fium_escape.elf

RAYLIB ?= ../raylib/src/

all:
	# Compilacion para Raspberry PI 4
	g++ -o $(EXEC_NAME) *.cpp -Wall -std=c++17 -DEGL_NO_X11 -I. -I$(RAYLIB) -I$(RAYLIB)/external -I/usr/local/include -I/usr/include/libdrm -L. -L$(RAYLIB) -lraylib -lGLESv2 -lEGL -ldrm -lgbm -lpthread -lrt -lm -ldl -latomic -DPLATFORM_DRM

clean:
	rm -f $(EXEC_NAME)

