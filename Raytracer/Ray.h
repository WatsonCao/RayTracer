#ifndef _RAY_H
#define _RAY_H

#include "Vect.h"

 class Ray{
    
   Vect origin, direction;

	public:

	Ray();

	Ray(Vect, Vect);

	// method functions to return x,y,z co-ordinates

	Vect getRayOrigin() { return origin;}
	Vect getRayDirection() { return direction;}

};


 Ray:: Ray(){

	origin = Vect(0,0,0);
	direction = Vect(1, 0, 0);
}

// This is how  Rayor values get assigned to x,y,z co-ordinates.

 Ray:: Ray(Vect o, Vect d){

	origin =o;
	direction = d;
}

#endif