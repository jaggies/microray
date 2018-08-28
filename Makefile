SRC = \
	./src/amigastubs.c \
	./src/branch.c \
	./src/bvh.c \
	./src/checkerboardshader.c \
	./src/dither.c \
	./src/hit.c \
	./src/leaf.c \
	./src/loader.c \
	./src/netpbm.c \
	./src/perspectivecamera.c \
	./src/phongshader.c \
	./src/pointlight.c \
	./src/range.c \
	./src/raymath.c \
	./src/raytrace.c \
	./src/sphere.c \
	./src/testload.c \
	./src/triangle.c \
	./src/vec2.c \
	./src/vec3.c \
	./src/world.c 

OBJ = $(SRC:.c=.o)
INCLUDE = -Isrc
X11INC = -I/usr/X11/include -I/opt/local/include/
#X11INC = -I/usr/include/X11R5 -I/usr/include/Motif1.2/
X11LIB = -L/usr/lib/X11R5 -L/usr/lib/Motif1.2 -lXm -lXt -lX11
DEFINES = -DSHOW_PROGRESS
CCFLAGS = -Aa $(DEFINES)
CC = CC
LIBS = -lm 
OPT = -O 

.c.o: 
	$(CC) $(OPT) $(CCFLAGS) $(INCLUDE) -c $< -o $@

all: microray ray11

microray: $(OBJ) apps/microray/microray.c
	$(CC) $(OPT) $(CCFLAGS) -o $@ $(CCFLAGS) $(INCLUDE) $(OBJ) $(LIBS) apps/microray/microray.c

ray11: $(OBJ) apps/x11/ray11.c
	$(CC) $(OPT) $(CCFLAGS) -o $@ $(CCFLAGS) $(INCLUDE) $(X11INC) $(OBJ) $(LIBS) apps/x11/ray11.c $(X11LIB)

clean:
	@rm -rf $(OBJ) microray ray11
