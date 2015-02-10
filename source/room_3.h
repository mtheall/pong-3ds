
#ifndef __ROOM_3__
#define __ROOM_3__

//--------------------------------------------------------------------------------------------------

void Room_3_Draw(int screen);

//--------------------------------------------------------------------------------------------------

void Room_3_GetBounds(int * xmin, int * xmax, int * ymin, int * ymax, int * zmin, int * zmax);

//--------------------------------------------------------------------------------------------------

void Room_3_Init(void);

void Room_3_End(void);

void Room_3_Handle(void);

int Room_3_3DMovementEnabled(void);

//--------------------------------------------------------------------------------------------------

#endif //__ROOM_3__
