SRC = \
	./lib/amiga.c \
	./lib/branch.c \
	./lib/bvh.c \
	./lib/checkshd.c \
	./lib/dither.c \
	./lib/hit.c \
	./lib/leaf.c \
	./lib/loader.c \
	./lib/netpbm.c \
	./lib/perspcam.c \
	./lib/phongshd.c \
	./lib/pointlit.c \
	./lib/range.c \
	./lib/raymath.c \
	./lib/raytrace.c \
	./lib/sphere.c \
	./lib/testload.c \
	./lib/triangle.c \
	./lib/vec2.c \
	./lib/vec3.c \
	./lib/world.c 

OBJ = $(SRC:.c=.o)
INCLUDE = -Ilib
X11INC = -I/usr/X11/include -I/opt/local/include/
#X11INC = -I/usr/include/X11R5 -I/usr/include/Motif1.2/
X11LIB = -L/usr/lib/X11R5 -L/usr/lib/Motif1.2 -lXm -lXt -lX11
DEFINES = -DSHOW_PROGRESS

#HP9000
#CCFLAGS = -Aa $(DEFINES)
#CC = CC

CC = gcc

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
