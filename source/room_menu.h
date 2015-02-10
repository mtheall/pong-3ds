
#ifndef __ROOM_MENU__
#define __ROOM_MENU__

//--------------------------------------------------------------------------------------------------

void Room_Menu_Draw(int screen);

//--------------------------------------------------------------------------------------------------

void Room_Menu_GetBounds(int * xmin, int * xmax, int * ymin, int * ymax, int * zmin, int * zmax);

//--------------------------------------------------------------------------------------------------

void Room_Menu_Init(void);

void Room_Menu_End(void);

void Room_Menu_Handle(void);

int Room_Menu_3DMovementEnabled(void);

//--------------------------------------------------------------------------------------------------

#endif //__ROOM_MENU__
