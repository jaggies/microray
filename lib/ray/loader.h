/*
 * loader.h
 *
 *  Created on: Feb 26, 2018
 *      Author: jmiller
 */

#ifndef LOADER_H_
#define LOADER_H_

World* loadFile(const char* fromPath);
void dumpStats(FILE* fp);

#endif /* LOADER_H_ */
