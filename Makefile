SRC = \
	lib/ray/loader.c \
	lib/ray/leaf.c \
	lib/ray/vec2.c \
	lib/ray/vec3.c \
	lib/ray/hit.c \
	lib/ray/branch.c \
	lib/ray/triangle.c \
	lib/ray/perspcam.c \
	lib/ray/dither.c \
	lib/ray/checkshd.c \
	lib/ray/phongshd.c \
	lib/ray/testload.c \
	lib/ray/raytrace.c \
	lib/ray/bvh.c \
	lib/ray/range.c \
	lib/ray/sphere.c \
	lib/ray/world.c \
	lib/ray/raymath.c \
	lib/ray/netpbm.c \
	lib/ray/pointlit.c

OBJ = $(SRC:.c=.o)
INCLUDE = -Ilib/ray
X11INC = -I/usr/X11/include -I/opt/local/include/
#X11INC = -I/usr/include/X11R5 -I/usr/include/Motif1.2/
X11LIB = -L/usr/lib/X11R5 -L/usr/X11R6/lib -L/usr/lib/Motif1.2 -lXm -lXext -lXt -lX11
DEFINES = -DSHOW_PROGRESS

#HP9000
#CCFLAGS = -Aa $(DEFINES)
#CC = CC

CC = gcc

LIBS = -lm 
OPT = -O2

.c.o: 
	$(CC) $(OPT) $(CCFLAGS) $(INCLUDE) -c $< -o $@

all: microray ray11

microray: $(OBJ) apps/microray/microray.c
	$(CC) $(OPT) $(CCFLAGS) -o $@ $(CCFLAGS) $(INCLUDE) $(OBJ) $(LIBS) apps/microray/microray.c

ray11: $(OBJ) apps/x11/ray11.c
	$(CC) $(OPT) $(CCFLAGS) -o $@ $(CCFLAGS) $(INCLUDE) $(X11INC) $(OBJ) $(LIBS) apps/x11/ray11.c $(X11LIB)

clean:
	@rm -rf $(OBJ) microray ray11

zip:
	zip -r microray lib apps makefile.dos Makefile scenes *.EXE
