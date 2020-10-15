
//-------------------------------------------------------------------------------------------------------

/*
    Pong 3DS. Just a pong for the Nintendo 3DS.
    Copyright (C) 2015 Antonio Ni�o D�az

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//-------------------------------------------------------------------------------------------------------

#include <3ds.h>

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "png_utils.h"

//----------------------------------------------------------------------------------

#define TICKS_PER_SEC (268123480)
#define TICKS_PER_VBL (TICKS_PER_SEC/60)

#define MAX_CORES (2)

static int frame_count[MAX_CORES] = { 0, 0 };
static u64 last_ticks[MAX_CORES] = { 0, 0 };
static int _FPS[MAX_CORES] = { 0, 0 };
static u64 cpu_last_ticks[MAX_CORES] = { 0, 0 };
static float _CPU[MAX_CORES] = { 0, 0 };

void Timing_Start(int cpu_core)
{
	last_ticks[cpu_core] = svcGetSystemTick();
}

void Timing_EndFrame(int cpu_core)
{
	frame_count[cpu_core] ++;
	u64 inc_ticks = svcGetSystemTick() - cpu_last_ticks[cpu_core];
	_CPU[cpu_core] = (float)inc_ticks*((float)100.0f/(float)TICKS_PER_VBL);
}

void Timing_StartFrame(int cpu_core)
{
	if(svcGetSystemTick() >= last_ticks[cpu_core] + TICKS_PER_SEC)
	{
		last_ticks[cpu_core] += TICKS_PER_SEC;
		_FPS[cpu_core] = frame_count[cpu_core];
		frame_count[cpu_core] = 0;	
	}
	
	cpu_last_ticks[cpu_core] = svcGetSystemTick();
}

int Timing_GetFPS(int cpu_core)
{
	return _FPS[cpu_core];
}

float Timing_GetCPUUsage(int cpu_core)
{
	return _CPU[cpu_core];
}


//----------------------------------------------------------------------------------

//Xorshift RNGs, by George Marsaglia (The Florida State University)

static uint64_t x=123456789,y=362436069,z=521288629,w=88675123;

static inline uint64_t xor128(void)
{
    uint64_t t;
    t=(x^(x<<11)); x=y; y=z; z=w;
    return ( w=(w^(w>>19))^(t^(t>>8)) );
}

void fast_srand(uint64_t seed)
{
    x = seed; // Some way to seed the RNG is needed, even if in the paper it's not specified...
}

uint32_t fast_rand(void)
{
    return xor128()&FAST_RAND_MAX;
}

//-------------------------------------------------------------------------------------------------------

#define CONFIG_3D_SLIDERSTATE (*(float*)0x1FF81080) //this should be in ctrulib...

void PNGScreenshot_Top(void)
{
	u32 * dst = malloc(400*240*sizeof(u32));
	if(dst)
	{
		char filename[60];
		u64 time = osGetTime();
		int i,j;
		
		if(CONFIG_3D_SLIDERSTATE == 0.0f)
		{
			u8 * src = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
			for(i = 0; i < 400; i++) for(j = 240-1; j >= 0; j--)
			{
				int b = *src++;
				int g = *src++;
				int r = *src++;
				dst[j*400+i] = (b<<16)|(g<<8)|r;
			}
			
			snprintf(filename,sizeof(filename),"%llu_top.png",time); // long long unsigned int
			Save_PNG(filename, 400, 240, (void*)dst,0);
		}
		else
		{
			u8 * src = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
			for(i = 0; i < 400; i++) for(j = 240-1; j >= 0; j--)
			{
				int b = *src++;
				int g = *src++;
				int r = *src++;
				dst[j*400+i] = (b<<16)|(g<<8)|r;
			}
			snprintf(filename,sizeof(filename),"%llu_top_l.png",time); // long long unsigned int
			Save_PNG(filename, 400, 240, (void*)dst,0);
			
			src = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);
			for(i = 0; i < 400; i++) for(j = 240-1; j >= 0; j--)
			{
				int b = *src++;
				int g = *src++;
				int r = *src++;
				dst[j*400+i] = (b<<16)|(g<<8)|r;
			}
			snprintf(filename,sizeof(filename),"%llu_top_r.png",time); // long long unsigned int
			Save_PNG(filename, 400, 240, (void*)dst,0);
		}
		
		free(dst);
	}
}

void PNGScreenshot_Bottom(void)
{
	u32 * dst = malloc(320*240*sizeof(u32));
	if(dst)
	{
		char filename[60];
		u64 time = osGetTime();
		int i,j;
		
		u8 * src = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
		for(i = 0; i < 320; i++) for(j = 240-1; j >= 0; j--)
		{
			int b = *src++;
			int g = *src++;
			int r = *src++;
			dst[j*320+i] = (b<<16)|(g<<8)|r;
		}
		
		snprintf(filename,sizeof(filename),"%llu_bottom.png",time); // long long unsigned int
		Save_PNG(filename, 320, 240, (void*)dst,0);
		
		free(dst);
	}
}

//-------------------------------------------------------------------------------------------------------
