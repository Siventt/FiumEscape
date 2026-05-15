EXEC_NAME ?= FiumEscape.elf

RAYLIB ?= ../raylib/src/

all:
	# Para Linux x64
	#g++ -o $(EXEC_NAME) *.cpp -I $(RAYLIB) -L $(RAYLIB) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

	# Para Raspberry PI 4
	g++ -o $(EXEC_NAME) *.cpp -Wall -D_DEFAULT_SOURCE -Wno-missing-braces -Wno-unused-result -O2 -DEGL_NO_X11 -I. -I$(RAYLIB) -I$(RAYLIB)/external -I/usr/local/include -I/usr/include/libdrm -L. -L$(RAYLIB) -lraylib -lGLESv2 -lEGL -ldrm -lgbm -lpthread -lrt -lm -ldl -latomic -DPLATFORM_DRM -lstdc++

clean:
	rm -f $(EXEC_NAME)