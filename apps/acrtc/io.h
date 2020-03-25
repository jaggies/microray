/*
 * io.h
 *
 *  Created on: Mar 14, 2020
 *      Author: jmiller
 */

#ifndef IO_H_
#define IO_H_

extern void out8(uint16_t addr, uint8_t data);
extern uint8_t in8(uint16_t addr);
extern void out16(uint16_t addr, uint16_t data);
extern uint16_t in16(uint16_t addr);

#endif /* IO_H_ */
