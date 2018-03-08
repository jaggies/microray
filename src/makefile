SRC = branch.c bvh.c checkerboardshader.c hit.c leaf.c loader.c main.c netpbm.c perspectivecamera.c phongshader.c pointlight.c range.c raymath.c raytrace.c sphere.c testload.c triangle.c vec2.c vec3.c world.c
OBJ = $(SRC:.c=.o)
ray : $(OBJ)
	cc -o $@ $(OBJ)
.c.o : 
	cc -c $<
clean :
	rm -f $(OBJ) ray out.ppm
