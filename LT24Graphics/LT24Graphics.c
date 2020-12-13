/*
 * LT24Graphics.c
 *
 *  Created on: April 20, 2020
 *      Author: MorseMeow
 *      for: Simple LT24 LCD Graphics Engine
 */
#include "stdio.h"
#include "../DE1SoC_LT24/DE1SoC_LT24.h"
#include "LT24Graphics.h"
#include "../HPS_Watchdog/HPS_Watchdog.h"
#include "../HPS_usleep/HPS_usleep.h" //some useful delay routines
//Include required header files
#include <stdbool.h> //Boolean variable type "bool" and "true"/"false" constants.
#include <stdlib.h>
//successful and error codes
#define LT24_LINE_SUCCESS     		 0
#define LT24_LINE_ERROR      		-1
#define LT24_BOX_SUCCESS      		 0
#define LT24_BOX_ERROR       		-1
#define LT24_BOXFILL_SUCCESS 		 0
#define LT24_BOXFILL_ERROR   		-1
#define LT24_TRIANGLE_SUCCESS		 0
#define LT24_TRIANGLE_ERROR  		-1
#define LT24_TRIANGLEFILL_SUCCESS	 0
#define LT24_TRIANGLEFILL_ERROR  	-1
#define LT24_CIRCLE_SUCCESS    		 0
#define LT24_CIRCLE_ERROR      		-1
//int i = 0;
//int j = 0;
//int a[i][j] = 0;
//int b[i][j] = 0;
//	for (i=0; i<LT24_WIDTH; i++){
//		for (j=0; j<LT24_HEIGHT; j++){
//			 b[i][j] = a[i][j];
//		}
//	}
int Graphics_initialise( unsigned int pio_base_address, unsigned int pio_hw_base_address) {
	//Sanity checks
    if (!LT24_isInitialised()){
    	LT24_initialise(pio_base_address, pio_hw_base_address);
    }
    return LT24_SUCCESS;
}

int Graphics_drawLine(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,short colour) {
//refers to Bresenham's line algorithm
	bool steep = abs(y2 - y1) > abs(x2 - x1); //when steep is 1, slope >1
	int deltax;                               //different of x1 and x2
	int deltay;								  //different of y1 and y2
	int transint = 0;						  //the integer format of error
	int ystep;								  //increment of y, which is 1 or -1
	int x;
	int y;
	int transp;								  //for point transformation
	int retValue;							  //for return value output
	//check whether the points are out of the screen
	if (x1 > LT24_WIDTH || y1 > LT24_HEIGHT || x2 > LT24_WIDTH || y2 > LT24_HEIGHT){
		retValue = LT24_LINE_ERROR;
		return retValue;
	}
	else{
		if (steep){
	//swap the pixel of x1 and y1
			transp = x1;
			x1 = y1;
			y1 = transp;
	//swap the pixel of x2 and y2
			transp = x2;
			x2 = y2;
			y2 = transp;
		}

		if (x1 > x2){
	//swap the pixel of x1 and x2
			transp = x1;
			x1 = x2;
			x2 = transp;
	//swap the pixel of y1 and y2
			transp = y1;
			y1 = y2;
			y2 = transp;
		}

		deltax = x2 - x1;
		deltay = abs(y2 - y1);
		y = y1;
		if (y1 < y2){
			ystep = 1;
		}
		else{
			ystep = -1;
		}
		//draw points
		for (x = x1;x <= x2; x++){
			if (steep) {
				LT24_drawPixel(colour,y,x);
			}
			else {
				LT24_drawPixel(colour,x,y);
			}
			transint = transint + 2*deltay;
			if (transint >= deltax) {
				y = y + ystep;
				transint = transint - 2*deltax;
			}
			ResetWDT();
		}
		//return success
		retValue = LT24_LINE_SUCCESS;
		return retValue;
	}
}

int Graphics_drawBox(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned short colour,bool noFill,unsigned short fillColour){
	int x;
	int y;
	int retValue;//for return value output
	//check whether the points are out of the screen
	if (x1 > LT24_WIDTH || y1 > LT24_HEIGHT || x2 > LT24_WIDTH || y2 > LT24_HEIGHT){
		retValue = LT24_BOX_ERROR;
		return retValue;
	}
	else{
		//draw box without fill
		if (noFill){
			for (x = x1;x <= x2; x++){
				LT24_drawPixel(colour,x,y1);
				LT24_drawPixel(colour,x,y2);
			}
			for (y = y1;y <= y2; y++){
				LT24_drawPixel(colour,x1,y);
				LT24_drawPixel(colour,x2,y);
			}
		ResetWDT();
		retValue = LT24_BOX_SUCCESS;
		return retValue;
		}
		else{
			//draw box fill with colour
			for (x = x1;x <= x2; x++){
				LT24_drawPixel(colour,x,y1);
				LT24_drawPixel(colour,x,y2);
			}
			for (y = y1;y <= y2; y++){
				LT24_drawPixel(colour,x1,y);
				LT24_drawPixel(colour,x2,y);
			}
			for (x = (x1+1);x <= (x2-1); x++){
				for(y = (y1+1);y <= (y2-1); y++){
					LT24_drawPixel(fillColour,x,y);
				}
			}
		ResetWDT();
		//return success
		retValue = LT24_BOXFILL_SUCCESS;
		return retValue;
		}
	}
}

int Graphics_drawTriangle(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned int x3,unsigned int y3,unsigned short colour,bool noFill,unsigned short fillColour){
	int retValue;//for return value output
	bool steep = 0; //when steep is 1, slope >1
	int deltax;                               //different of x1 and x2
	int deltay;								  //different of y1 and y2
	int transint = 0;						  //the integer format of error
	int ystep;								  //increment of y, which is 1 or -1
	int ya;
	int yb;
	int xa;
	int xb;
	int x0;
	int y0;
	int x;
	int y;
	int transp;								  //for point transformation
	int Xmax[LT24_HEIGHT];
	int Xmin[LT24_HEIGHT];

	for (y = 0;y < LT24_HEIGHT; y++) {
		Xmax[y] = -5;
		Xmin[y] = -5;
	}

	//check whether the points are out of the screen
	if (x1 > LT24_WIDTH || y1 > LT24_HEIGHT || x2 > LT24_WIDTH || y2 > LT24_HEIGHT || x3 > LT24_WIDTH || y3 > LT24_HEIGHT){
		retValue = LT24_TRIANGLE_ERROR;
		return retValue;
	}
	else{
		if (noFill){
			//draw triangle without fill
			Graphics_drawLine(x1,y1,x2,y2,colour);
			Graphics_drawLine(x1,y1,x3,y3,colour);
			Graphics_drawLine(x2,y2,x3,y3,colour);
			ResetWDT();
			//return success
			retValue = LT24_TRIANGLE_SUCCESS;
			return retValue;
		}
		else {
			//refers to Scanline rendering
			//draw triangle fill with colour

			//the first line
			//to find the Maximum and Minimum values of x, and compare with the basic x = -5
			//if x is the basic x, x = -5,
			//else to judge the minimum and maximum x
			xa = x1;
			ya = y1;
			xb = x2;
			yb = y2;
			steep = (abs(yb-ya) > abs(xb-xa));
			if (steep){
				//swap the pixel of x1 and y1
				transp = xa;
				xa = ya;
				ya = transp;
				//swap the pixel of x2 and y2
				transp = xb;
				xb = yb;
				yb = transp;
			}
			if (xa > xb){
				//swap the pixel of x1 and x2
				transp = xa;
				xa = xb;
				xb = transp;
				//swap the pixel of y1 and y2
				transp = ya;
				ya = yb;
				yb = transp;
			}
				deltax = xb - xa;
				deltay = abs(yb - ya);
				y = ya;
				if (ya < yb){
					ystep = 1;
				}
				else{
					ystep = -1;
				}
				//draw points
				for (x = xa;x <= xb; x++){
					if (steep) {
						x0=y;
						y0=x;
					}
					else {
						x0=x;
						y0=y;
					}
					if (Xmax[y0] == -5) {
						Xmax[y0] = x0;
					}
					else if (x0 > Xmax[y0]) {
						Xmax[y0] = x0;
					}
					if (Xmin[y0] == -5) {
						Xmin[y0] = x0;
					}
					else if (x0 < Xmin[y0]) {
						Xmin[y0] = x0;
					}
					transint = transint + 2*deltay;
					if (transint >= deltax) {
						y = y + ystep;
						transint = transint - 2*deltax;
					}
					ResetWDT();
				}

				//the second line
				//to find the Maximum and Minimum values of x, and compare with the basic x = -5
				//if x is the basic x, x = -5,
				//else to judge the minimum and maximum x
				xa = x1;
				ya = y1;
				xb = x3;
				yb = y3;
				steep = (abs(yb-ya) > abs(xb-xa));
				if (steep){
					//swap the pixel of x1 and y1
					transp = xa;
					xa = ya;
					ya = transp;
					//swap the pixel of x2 and y2
					transp = xb;
					xb = yb;
					yb = transp;
				}
				if (xa > xb){
					//swap the pixel of x1 and x2
					transp = xa;
					xa = xb;
					xb = transp;
					//swap the pixel of y1 and y2
					transp = ya;
					ya = yb;
					yb = transp;
				}
					deltax = xb - xa;
					deltay = abs(yb - ya);
					y = ya;
					if (ya < yb){
						ystep = 1;
					}
					else{
						ystep = -1;
					}
					//draw points
					for (x = xa;x <= xb; x++){
						if (steep) {
							x0=y;
							y0=x;
						}
						else {
							x0=x;
							y0=y;
						}
						if (Xmax[y0] == -5) {
							Xmax[y0] = x0;
						}
						else if (x0 > Xmax[y0]) {
							Xmax[y0] = x0;
						}
						if (Xmin[y0] == -5) {
							Xmin[y0]=x0;
						}
						else if (x0 < Xmin[y0]) {
							Xmin[y0] = x0;
						}
						transint = transint + 2*deltay;
						if (transint >= deltax) {
							y = y + ystep;
							transint = transint - 2*deltax;
						}
						ResetWDT();
					}

					//the third line
					//to find the Maximum and Minimum values of x, and compare with the basic x = -5
					//if x is the basic x, x = -5,
					//else to judge the minimum and maximum x
					xa = x2;
					ya = y2;
					xb = x3;
					yb = y3;
					steep = (abs(yb-ya) > abs(xb-xa));
					if (steep){
						//swap the pixel of x1 and y1
						transp = xa;
						xa = ya;
						ya = transp;
						//swap the pixel of x2 and y2
						transp = xb;
						xb = yb;
						yb = transp;
					}
					if (xa > xb){
						//swap the pixel of x1 and x2
						transp = xa;
						xa = xb;
						xb = transp;
						//swap the pixel of y1 and y2
						transp = ya;
						ya = yb;
						yb = transp;
					}
						deltax = xb - xa;
						deltay = abs(yb - ya);
						y = ya;
						if (ya < yb){
							ystep = 1;
						}
						else{
							ystep = -1;
						}
						//draw lines
						for (x = xa;x <= xb; x++){
							if (steep) {
								x0=y;
								y0=x;
							}
							else {
								x0=x;
								y0=y;
							}
							if (Xmax[y0] == -5) {
								Xmax[y0] = x0;
							}
							else if (x0 > Xmax[y0]) {
								Xmax[y0] = x0;
							}
							if (Xmin[y0] == -5) {
								Xmin[y0] = x0;
							}
							else if (x0 < Xmin[y0]) {
								Xmin[y0] = x0;
							}
							transint = transint + 2*deltay;
							if (transint >= deltax) {
								y = y + ystep;
								transint = transint - 2*deltax;
							}
							ResetWDT();
						}


				for (y = 0;y < LT24_HEIGHT; y++) {
					if (!(Xmax[y] == -5 || Xmin[y] == -5)) {
						Graphics_drawLine(Xmin[y],y,Xmax[y],y,fillColour);
					}
				}
		}
	}

	//draw triangle without fill
	Graphics_drawLine(x1,y1,x2,y2,colour);
	Graphics_drawLine(x1,y1,x3,y3,colour);
	Graphics_drawLine(x2,y2,x3,y3,colour);
	ResetWDT();
	//return success
	retValue = LT24_TRIANGLE_SUCCESS;
	return retValue;

}

void CirclePlot(unsigned int const x,unsigned int const y,unsigned int const x1,unsigned int const y1, unsigned short colour){
	//draw 8 points
	LT24_drawPixel(colour,  x1 + x,  y1 + y);
	LT24_drawPixel(colour, -x1 + x,  y1 + y);
	LT24_drawPixel(colour,  y1 + x,  x1 + y);
	LT24_drawPixel(colour, -y1 + x,  x1 + y);
	LT24_drawPixel(colour, -y1 + x, -x1 + y);
	LT24_drawPixel(colour,  y1 + x, -x1 + y);
	LT24_drawPixel(colour,  x1 + x, -y1 + y);
	LT24_drawPixel(colour, -x1 + x, -y1 + y);
}
int Graphics_drawCircle(unsigned int x,unsigned int y,unsigned int r,unsigned short colour,bool noFill,unsigned short fillColour){
//refers to Midpoint circle algorithm
	int x1 = r;
	int y1 = 0;
	int i;
	int decisionparam = 1 - x1;        //radius error
	int retValue;//for return value output
	//check whether the points are out of the screen
	if (x > LT24_WIDTH || y > LT24_HEIGHT){
		retValue = LT24_CIRCLE_ERROR;
		return retValue;
	}
	while(x1 >= y1) {

		if(noFill == false) {
			//draw circle fill with colour
			// (-x1,+y1)->(+x1,+y1)
			for(i = -x1+x; i <= x1+x; i++)
			LT24_drawPixel(fillColour, i,  y1+y);
			//
			// (+y1,-x1)->(+y1,+x1)
			for(i = -x1+y; i <= x1+y; i++)
			LT24_drawPixel(fillColour, y1+x, i);
			//
			// (-x1,-y1)-> (+x1,-y1)
			for(i = -x1+x; i <= x1+x; i++)
			LT24_drawPixel(fillColour, i,  -y1+y);
			// (-y1,-x1)->(-y1,+x1)
			for(i = -x1+y; i <= x1+y; i++)
			LT24_drawPixel(fillColour, -y1+x, i);
		}
		else {
			//only draw the points of circle without fill
			CirclePlot(x, y, x1, y1, colour);
		}
		y1++;
		if(decisionparam < 0) {
			decisionparam += 2 * y1 + 1;
		}
		else {
			x1--;
			decisionparam += 2 * (y1 - x1) + 1;
		}
	}
	// rewrite the circle without fill
	x1 = r;
	y1 = 0;
	decisionparam = 1 - x1;
	while(x1 >= y1) {
		CirclePlot(x, y, x1, y1, colour);
		y1++;
		if(decisionparam < 0) {
			decisionparam = decisionparam + 2 * y1 + 1;
		}
		else {
			x1--;
			decisionparam = decisionparam + 2 * (y1 - x1) + 1;
		}
	}
	//return success
	retValue = LT24_CIRCLE_SUCCESS;
	return retValue;

}


