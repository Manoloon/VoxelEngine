#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "dos/dos.h"
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define SCALE_FACTOR  100.0
#define ARRAY_LENGTH(x) (sizeof(x)/sizeof(x[0]))

uint8_t* ColorMap = NULL;
uint8_t* HeightMap = NULL;
typedef struct
{
	float x;
	float y;
	float Dof;
	float height;
}camera_t;

camera_t Camera = 
{
	.x = 512,
	.y = 512,
	.Dof = 400,
	.height = 150.0
};

void input(void)
{
	if(keystate(KEY_UP))
		Camera.y++;
	if(keystate(KEY_DOWN))
		Camera.y--;
	if(keystate(KEY_RIGHT))
		Camera.x++;
	if(keystate(KEY_LEFT))
		Camera.x--;
	if(keystate(KEY_Q))
		Camera.height++;
	if(keystate(KEY_A))
		Camera.height--;
}

int main(int argc, char* args[])
{
	setvideomode(videomode_320x200);
	// load gif
	uint8_t Palette[256 * 3];
	int MapWidth;
	int	MapHeight;
	int PalCount;

	ColorMap = loadgif("maps/gif/map0.color.gif",&MapWidth, &MapHeight, &PalCount, Palette);
	if(!ColorMap)
	{
		fprintf(stderr,"Failed to load Colormap file");
		return -1;
	}
	HeightMap = loadgif("maps/gif/map0.height.gif",&MapWidth,&MapHeight,NULL,NULL);
	if(!HeightMap)
	{
		fprintf(stderr,"Failed to load Heightmap file");
		return -1;
	}
	for(int i = 0; i < PalCount; i++)
	{
		setpal(i,Palette[3*i+0],Palette[3*i+1],Palette[3*i+2]);
	}
	// background color
	setpal(0,36,36,56);

	setdoublebuffer(1);
	uint8_t* FrameBuffer = screenbuffer();
	
	while(!shuttingdown())
	{
		waitvbl();
		clearscreen();
		input();
		//// Voxel engine algorithm 
		// View camera
		float plx = -Camera.Dof;
		float ply = +Camera.Dof;
		
		float prx = +Camera.Dof;
		float pry = +Camera.Dof;

		for(int i = 0; i < SCREEN_WIDTH;i++)
		{
			float deltaX = (plx + (prx - plx) / SCREEN_WIDTH * i) / Camera.Dof;
			float deltaY = (ply + (pry - ply) / SCREEN_WIDTH * i) / Camera.Dof;

			float rx = Camera.x;
			float ry = Camera.y;

			// recall that y grows downward
			float MaxHeight = SCREEN_HEIGHT;
			// 1 and not 0 because 0 is where the camera is.
			for(int z = 1; z < Camera.Dof;z++)
			{
				rx += deltaX;
				ry += deltaY;
				// find the offset to fetch from the heightmap the values
				// bitwise clamping
				int MapOffset = ((1024 * ((int)(ry) & 1023)) + ((int)(rx) & 1023));

				int HeightOnScreenProjected = (int)((Camera.height - HeightMap[MapOffset]) / z * SCALE_FACTOR);
				if(HeightOnScreenProjected < 0)
				{
					HeightOnScreenProjected = 0;
				}
				if (HeightOnScreenProjected > SCREEN_HEIGHT)
				{
					HeightOnScreenProjected = SCREEN_HEIGHT -1;
				}
				// remember that y grows downward
				if(HeightOnScreenProjected < MaxHeight)
				{
					for(int y = HeightOnScreenProjected; y < MaxHeight; y++)
					{
						FrameBuffer[(SCREEN_WIDTH * y)+ i] = (uint8_t) ColorMap[MapOffset];
					}
					MaxHeight = HeightOnScreenProjected;
				}
			}
		}
		////////// end of Voxel engine algo
		// Update();
		// Render();
		
		FrameBuffer = swapbuffers();
		if(keystate(KEY_ESCAPE))
		{
			break;
		}
	}
	return EXIT_SUCCESS;
}