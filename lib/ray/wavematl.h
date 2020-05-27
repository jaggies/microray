/*
 * wavematl.h
 *
 *  Created on: May 26, 2020
 *      Author: jmiller
 */

#ifndef LIB_RAY_WAVEMATL_H_
#define LIB_RAY_WAVEMATL_H_

extern bool loadMaterialLibrary(World* world, const char* path, const char* name);
extern Shader* findShader(const char* name, World* world);
extern PhongShader* createDefaultShader();

#endif /* LIB_RAY_WAVEMATL_H_ */
