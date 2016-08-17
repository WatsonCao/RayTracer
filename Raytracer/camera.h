#ifndef _CAMERA_H
#define _CAMERA_H

#include "Vect.h"

 class Camera{
    
   Vect camera_position, camera_direction, camright, camdown;

	public:

	Camera();

	Camera(Vect, Vect,Vect, Vect);

	// method functions to return x,y,z co-ordinates

	Vect getCameraPosition() { return camera_position; }
	Vect getCameraDirection() { return camera_direction;}
	Vect getCameraRight() { return camright; }
	Vect getCameraDown() { return camdown;}

};


 Camera:: Camera(){

	camera_position = Vect(0,0,0);
	camera_direction = Vect(0,0,1);
	camright = Vect(0,0,0);
	camdown = Vect(0,0,0);
}

// This is how  Vector values get assigned to x,y,z co-ordinates.

 Camera:: Camera(Vect pos, Vect dir, Vect right, Vect down){

	camera_position = pos;
	camera_direction = dir;
	camright= right;
	camdown = down;
}

#endif