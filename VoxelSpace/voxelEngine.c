#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "dos.h"

#define SCREEN_HEIGHT = 200;
#define SCREEN_WIDTH = 320;

uint8_t* ColorMap = NULL;
uint8_t* HeightMap = NULL;

typedef struct 
{
	float x;
	float y;
	float zFar;
} ACamera;

//ACamera Camera ={

int main(int argc, char* args[])
{
	setvideomode(videomode_320x200);
	// load gif
	uint8_t Palette[256 * 3];
	int MapWidth,MapHeight,PalCount;
	
	ColorMap = loadgif("files/colormap.gif",&MapWidth, &MapHeight, &PalCount, Palette);
	HeightMap = loadgif("files/heightmap.gif",&MapWidth,&MapHeight,NULL,NULL);
	
	for(int i = 0;i<PalCount;i++)
	{
		setpal(i,Palette[3 * i +0, Palette[3 * i + 1], Palette[3 * i + 2]);
	}
	setpal(0,36,36,56);
	
	setdoublebuffer(1);
	uint8_t* FrameBuffer = screenbuffer();
	
	while(!shuttingdown())
	{
		waitvbl();
		clearscreen();
		
		// input();
		// Update();
		// Render();
		
		// a test/////////////////////////////////////
		int x = 160;
		int y = 100;
		FrameBuffer[(SCREEN_WIDTH * y) + x) = 0x50;
		//////////////////////////////////////////////
		FrameBuffer = swapbuffers();
		if(keystate(KEY_ESCAPE))
		{
			break;
		}
	}
	return EXIT_SUCCESS;
}