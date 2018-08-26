SRC = \
	./src/amigastubs.c \
	./src/branch.c \
	./src/bvh.c \
	./src/checkerboardshader.c \
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
	./src/world.c \
	./apps/microray/microray.c 

OBJ = $(SRC:.c=.o)
CCFLAGS = -Aa
INCLUDE = -Isrc
CC = CC
LIBS = -lm 
OPT = -O 

.c.o: 
	$(CC) $(OPT) $(CCFLAGS) $(INCLUDE) -c $< -o $@

microray: $(OBJ)
	$(CC) $(OPT) -o $@ $(CCFLAGS) $(OBJ) $(LIBS)

clean:
	@rm -rf $(OBJ) microray
