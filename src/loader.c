#include <stdio.h>
#include "world.h"

const char *tokens[] = {"sphere", "triangle", "phong", "camera", "light"};
const int kTokens = (sizeof(tokens) / sizeof(tokens[0]));

#define EPSILON 1.0e-7

enum {
	SPHERE = 0,
	TRIANGLE,
	PHONG,
	CAMERA,
	LIGHT
};

World* loadFile(char* fromPath)
{
    char buffer[80];
	World* world = createWorld();

	FILE *fp = fopen(fromPath, "r");
	if (!fp) {
		return 0;
	}
	while (!feof(fp)) {
		char* ptr = fgets(buffer, 80, fp);
		for (int i = 0; i < kTokens; i++) {
			const char* token = tokens[i];
			while (*token != 0 && *token == *ptr++)
				token++;
			if (*(token-1) == 0) {
				// found
				switch(i) {
					case SPHERE:
					case TRIANGLE:
					case PHONG:
					case CAMERA:
					case LIGHT:
						printf("Token:%s, args:%s\n", tokens[i], ptr);
				}
			}
		}
	}
	return world;
}
			
