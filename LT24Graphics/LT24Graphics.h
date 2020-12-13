/*
 * LT24Graphics.h
 *
 *  Created on: April 20, 2020
 *      Author: MorseMeow
 *      for: Simple LT24 LCD Graphics Engine
 */

#ifndef LT24GRAPHICS_LT24GRAPHICS_H_
#define LT24GRAPHICS_LT24GRAPHICS_H_

//Include required header files
#include <stdbool.h> //Boolean variable type "bool" and "true"/"false" constants.

#define SWAP_SUCCESS  0

int Graphics_initialise( unsigned int pio_base_address, unsigned int pio_hw_base_address);

int Graphics_drawLine(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,short colour);

int Graphics_drawBox(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned short colour,bool noFill,unsigned short fillColour);

int Graphics_drawTriangle(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int x3,unsigned int y3,unsigned short colour,bool noFill,unsigned short fillColour);

int Graphics_drawCircle(unsigned int x,unsigned int y,unsigned int r,unsigned short colour,bool noFill,unsigned short fillColour);

#endif /* LT24GRAPHICS_LT24GRAPHICS_H_ */
