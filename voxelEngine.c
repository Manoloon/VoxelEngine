#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "dos/dos.h"
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define SCALE_FACTOR  100.0
#define PI 3.141592
#define ARRAY_LENGTH(x) (sizeof(x)/sizeof(x[0]))

uint8_t* ColorMap = NULL;
uint8_t* HeightMap = NULL;
typedef struct
{
	float x;
	float y;
	float dof;
	float height;
	float yaw; // yaw angle in radians clockwise.
	float pitch;
	float roll;
}camera_t;

camera_t Camera = 
{
	.x = 512,
	.y = 512,
	.dof = 800,
	.height = 150.0,
	.yaw = 1.5 * PI,
	.pitch = 100,
	.roll = 0.0
};

void input(void)
{
	if(keystate(KEY_UP))
	{
		Camera.x += cos(Camera.yaw);
		Camera.y += sin(Camera.yaw);
		Camera.pitch -= 001.5;
	}		
	if(keystate(KEY_DOWN))
		{
			Camera.x -= cos(Camera.yaw);
			Camera.y -= sin(Camera.yaw);
			Camera.pitch +=001.5;
		}
	if(keystate(KEY_RIGHT))
	{	
		Camera.x++;
		if(Camera.roll > -1.0)
			Camera.roll -=0.05;
	}
	if(keystate(KEY_LEFT))
	{
		Camera.x--;
		if(Camera.roll < 1.0)
			Camera.roll +=0.05;
	}
	if(keystate(KEY_W))
		Camera.height++;
	if(keystate(KEY_S))
		Camera.height--;
	if(keystate(KEY_A))
		Camera.yaw -=0.005;
	if(keystate(KEY_D))
		Camera.yaw +=0.005;
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
		float YawSine = sin(Camera.yaw);
		float YawCosine = cos(Camera.yaw); 
		// View camera Left point
		float plx = YawCosine * Camera.dof + YawSine * Camera.dof;
		float ply = YawSine * Camera.dof - YawCosine * Camera.dof;
		// View camera Right point
		float prx = YawCosine * Camera.dof - YawSine * Camera.dof;
		float pry = YawSine * Camera.dof + YawCosine * Camera.dof;

		for(int i = 0; i < SCREEN_WIDTH;i++)
		{
			float deltaX = (plx + (prx - plx) / SCREEN_WIDTH * i) / Camera.dof;
			float deltaY = (ply + (pry - ply) / SCREEN_WIDTH * i) / Camera.dof;

			float rx = Camera.x;
			float ry = Camera.y;

			// recall that y grows downward
			float MaxHeight = SCREEN_HEIGHT;
			// 1 and not 0 because 0 is where the camera is.
			for(int z = 1; z < Camera.dof;z++)
			{
				rx += deltaX;
				ry += deltaY;
				// find the offset to fetch from the heightmap the values
				// bitwise clamping -> this wrap the world! 
				int MapOffset = ((1024 * ((int)(ry) & 1023)) + ((int)(rx) & 1023));

				int HeightProjected = (int)((Camera.height - HeightMap[MapOffset]) / z * SCALE_FACTOR + Camera.pitch);
				if(HeightProjected < 0)
				{
					HeightProjected = 0;
				}
				if (HeightProjected > SCREEN_HEIGHT)
				{
					HeightProjected = SCREEN_HEIGHT -1;
				}
				// remember that y grows downward
				if(HeightProjected < MaxHeight)
				{
					float leaning = (Camera.roll * (i / (float)SCREEN_WIDTH - 0.5)+0.5) * SCREEN_HEIGHT /6;
					for(int y = (HeightProjected + leaning) ; y < (MaxHeight + leaning) ; y++)
					{
						FrameBuffer[(SCREEN_WIDTH * y)+ i] = (uint8_t) ColorMap[MapOffset];
					}
					MaxHeight = HeightProjected;
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