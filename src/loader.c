#include <stdio.h>
#include <string.h>
#include "vec3.h"
#include "phongshader.h"
#include "shape.h"
#include "sphere.h"
#include "pointlight.h"
#include "perspectivecamera.h"
#include "world.h"

#define MAXNAME 16

const char *tokens[] = { "#", "sphere", "triangle", "phongshader", "perspectivecamera", "pointlight", "checkerboard", "background" };
enum { COMMENT = 0, SPHERE, TRIANGLE, PHONG, PERSPECTIVECAMERA, POINTLIGHT, CHECKERBOARD, BACKGROUND };

const int kTokens = (sizeof(tokens) / sizeof(tokens[0]));

static void addLight(World* world, Light* light) {
	if (light) {
		if (world->nLights < MAXLIGHTS) {
			world->lights[world->nLights++] = light;
		} else {
			printf("Too many lights!\n");
		}
	}
}

static void addShape(World* world, Shape* shape) {
	if (shape) {
		if (world->nShapes < MAXSHAPES) {
			world->shapes[world->nShapes++] = shape;
		} else {
			printf("Too many shapes!\n");
		}
	}
}

static Shader* getShader(World* world, const char* shaderName) {
	static Shader* defaultShader = 0;
	for (int i = 0; i < world->nShaders; i++) {
		if (strcmp(shaderName, world->shaderNames[i]) == 0) {
			return world->shaders[i];
		}
	}
	printf("Shader %s not found\n", shaderName);
	if (!defaultShader) {
		Vec3 diffuse; vec3(1,1,1,&diffuse);
		Vec3 specular; vec3(1,1,1,&specular);
		Vec3 ambient; vec3(0,0,0,&ambient);
		defaultShader = createPhongShader(&diffuse, &specular, &ambient, 20.0, 1.0, 0.0, 0.0);
	}
	return defaultShader; 
}

static void addShader(World* world, const char* shaderName, Shader* shader) {
	if (shader) {
		if (world->nShaders < MAXSHADERS) {
			world->shaders[world->nShaders] = shader;
			world->shaderNames[world->nShaders] = shaderName; // allocated in loadPhongShader
			world->nShaders++;
		} else {
			printf("Too many shaders!\n");
		}
	}
}

static Vec3 loadBackground(const char* args) {
	Vec3 tmp;
	sscanf(args, "%f %f %f", &tmp.x, &tmp.y, &tmp.z);
	return tmp;
}

static Shader* loadPhongShader(const char* args, const char** outname) {
	char name[MAXNAME];
	Vec3 diffuse, specular, ambient;
	float exponent, index, reflect, transmit;
 	sscanf(args, "%s  %f %f %f  %f %f %f  %f %f %f  %f %f %f %f", name,
		&diffuse.x, &diffuse.y, &diffuse.z, &specular.x, &specular.y, &specular.z,
		&ambient.x, &ambient.y, &ambient.z, &exponent, &index, &reflect, &transmit);
	*outname = strdup(name);
	return createPhongShader(&diffuse, &specular, &ambient, exponent, index, reflect, transmit);
}

static Shape* loadSphere(World* world, const char* args) {
	char material[MAXNAME];
	float x, y, z, r;
	sscanf(args, "%s %f %f %f %f", material, &x, &y, &z, &r);
	//printf("Got sphere: <%f,%f,%f> r=%f with material '%s'\n", x, y, z, r, material);
	return createSphere(x, y, z, r, getShader(world, material));
}

static Shader* loadPhong(const char* args) {
	char name[MAXNAME];
	Vec3 diffuse;
	Vec3 specular;
	Vec3 ambient;
	float exponent;
	float index;
	float reflect;
	float transmit;

 	sscanf(args, "%s  %f %f %f  %f %f %f  %f %f %f  %f %f %f %f",
		name, &diffuse.x, &diffuse.y, &diffuse.z, &specular.x, &specular.y, &specular.z,
		&ambient.x, &ambient.y, &ambient.z, &exponent, &index, &reflect, &transmit);
	return createPhongShader(&diffuse, &specular, &ambient, exponent, index, reflect, transmit);
}

static Camera* loadPerspectiveCamera(const char* args) {
	Vec3 from, at, up;
	float fov, aspect;
 	sscanf(args, "%f %f %f %f %f %f %f %f %f %f %f",
		&from.x, &from.y, &from.z, &at.x, &at.y, &at.z, &up.x, &up.y, &up.z, &fov, &aspect);
	// TODO: get aspect from world
	return createPerspectiveCamera(&from, &at, &up, fov, aspect);
}

static Light* loadPointLight(const char* args) {
	Vec3 position, color;
 	sscanf(args, "%f %f %f  %f %f %f", &position.x, &position.y, &position.z, &color.x, &color.y, &color.z);
	return createPointLight(&position, &color);
}

World* loadFile(char* fromPath)
{
    char buffer[100];
	World* world = createWorld();

	FILE *fp = fopen(fromPath, "r");
	if (!fp) {
		return 0;
	}
	int line = 0;
	while (!feof(fp)) {
		char* ptr;
		char* linestr = fgets(buffer, sizeof(buffer), fp);
		if (!linestr) {
			break;
		}
		line++;
		int foundToken = -1; // unknown
		for (int i = 0; i < kTokens; i++) {
			ptr = linestr;
			//printf("Looking for token %s\n", tokens[i]);
			const char* token = tokens[i];
			while (*token == *ptr && *token != 0 && *ptr != 0) {
				//printf("%c-", *ptr);
				ptr++;
				token++;
			}
			if (*token == 0) { // found
				foundToken = i;
				break; // token found
			}
		}
		if (foundToken == -1) {
			printf("Unrecognized token on line %d: \n\t%s", line, ptr);
			break; // ignore rest of line
		} else {
			const char* name;
			Shader* shader;
			switch(foundToken) {
				case COMMENT:
					break; // ignore
				case PERSPECTIVECAMERA:
					world->camera = loadPerspectiveCamera(ptr);
					break;
				case SPHERE:
					addShape(world, loadSphere(world, ptr));
					break;
				case POINTLIGHT:
					addLight(world, loadPointLight(ptr));
					break;
				case PHONG:
					shader = loadPhongShader(ptr, &name);
					addShader(world, name, shader);
					break;
				case BACKGROUND:
					world->background = loadBackground(ptr);
					break;
				default:
					printf("Unimplemented token %s, args:%s", tokens[foundToken], ptr);
			}
		}
	}
	printf("done\n");
	return world;
}
