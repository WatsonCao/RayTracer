
#ifndef _SPHERE_H
#define _SPHERE_H

#include "math.h"
#include "Object.h"
#include "Vect.h"
#include "Color.h"

class Sphere : public Object {

	Vect center;
	double radius;
	Color color;

public:
	Sphere();

	Sphere(Vect, double, Color);

	// method functions to return x,y,z co-ordinates

	Vect getSphereCenter() { return center; }
	double getSphereRadius() { return radius; }
	virtual Color getColor() { return color; }

  virtual	Vect getNormalAt(Vect point) {

		// normal always points away from the center of a sphere
		Vect normal_Vect = point.vectAdd(center.negative()).normalize();
		return normal_Vect;
	}
	// to prevent missing array 'virtual' keyword is added
	virtual double findIntersection(Ray ray) {

		Vect ray_origin = ray.getRayOrigin();
		double ray_origin_x = ray_origin.getVectX();
		double ray_origin_y = ray_origin.getVectY();
		double ray_origin_z = ray_origin.getVectZ();


		Vect ray_direction = ray.getRayDirection();
		double ray_direction_x = ray_direction.getVectX();
		double ray_direction_y = ray_direction.getVectY();
		double ray_direction_z = ray_direction.getVectZ();

		Vect sphere_centre = center;
		double sphere_centre_x = sphere_centre.getVectX();
		double sphere_centre_y = sphere_centre.getVectY();
		double sphere_centre_z = sphere_centre.getVectZ();

		double a = 1; // normalized
		double b = (2 * (ray_origin_x - sphere_centre_x)*ray_direction_x) + (2 * (ray_origin_y - sphere_centre_y)*ray_direction_y) + (2 * (ray_origin_z - sphere_centre_z)*ray_direction_z);

		double c = pow(ray_origin_x - sphere_centre_x, 2) + pow(ray_origin_y - sphere_centre_y, 2) + pow(ray_origin_z - sphere_centre_z, 2) - (radius*radius);

		double discriminant = b*b - 4 * c;

		if (discriminant > 0) {

			// the ray intersects sphere if discriminant > 0 

			// the first root 
			double root_1 = ((-1 * b - sqrt(discriminant)) / 2) - 0.0000001; // deals with accuracy errors

			if (root_1 > 0) {

				// the first root is the smallest positive root
				return root_1;
			}
			else {
				// The second root is the smallest positive root

				double root_2 = ((sqrt(discriminant)-b)/2) - 0.0000001;

				return root_2;
			}

		}
		else {

				// the ray missed sphere

			return -1;
		}

	}

};

	
Sphere::Sphere() {

	center = Vect(0, 0, 0);
	radius = 1.0;
	color = Color(0.5, 0.5, 0.5, 0);
}

// This is how  Sphereor values get assigned to x,y,z co-ordinates.

Sphere::Sphere(Vect centerValue, double radiusValue, Color  colorValue) {

	center = centerValue;
	radius = radiusValue;
	color = colorValue;
}

#endif