/* PRASAD DESHPANDE
  W1113935

  Assignment 3 Raytracing.

  The Following assignment had following parts:
  1) Implementing ray casting
  2) Adding Shadows - Ambient, Diffused  and Specular i.e. Phong's Shading
  3) Adding Refletion
  4) Adding Refraction
  5) Adding Anti-aliasing to the rendered image.

  Following parts are completed fully: 
  1) Implementing ray casting
  2) Adding Shadows - Ambient, Diffused  and Specular i.e. Phong's Shading
  3) Adding Refletion
  5) Adding Anti-aliasing to the rendered image.

  Partially completed Part (90% complete):  Adding Refraction

*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Vect.h"				// Vector Class file. It holds all the important functions like normalize(), negative(), dotProduct(), crossProduct(), vectAdd(), vectMult().
#include "Ray.h"				// Ray Class file. It creates a basic 'ray' object and holds following functions getRayOrigin(), getRayDirecton().
#include "Camera.h"				// Camera class file. It creates an object of the camera class and executes following functions getCameraPosition(), getCameraDirection(), getCameraRight(), getCameraDown().
#include "Color.h"				//  Color class file . It holds following functions: getColorRed(), getColorGreen, getColorBlue, getColorSpecial, setColorRed, setColorGreen, setColorBlue, setColorSpecial.
#include "Source.h"				// Source is a super class to 'light' class and holds methods as getLightColor(), getLightPosition()
#include "Light.h"				// Sub class of Light class
#include "Object.h"				//  This class holds following methods getColor(), getNormalAt(), findIntersections().


#include "Sphere.h"
#include "Plane.h"

#define depth_trace 4			// depth-trace variable is being used for anti-aliasing or supersampling. helps subdivide a pixel into sub-pixels. here we are dividing in 4 to make the effect visible.

using namespace std;

struct RGBType {				// Created a structure for the Color, having Red, Green and Blue components
	double r;
	double g;
	double b;
};

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Name of the Method: saveImg

	Input Parameters: filename, width, height, dots per inch(dpi), color structure RGBType

	Function:	-The objective of this method is to create a resultant image file. It has all the rendered images as the output.
				- It takes width and height as one of the primary inputs and calculates the file size.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
void saveImg(const char *filename, int w, int h , int dpi, RGBType *data) {

	FILE *f;									// File pointer to create a file
	int k = w*h;								// File area. Here variable 'k' is giving the product of width * height
	int s = 4 * k;								// Increazing that area 4 times.

	int filesize = 54 + s;

	double factor = 39.375;

	int m = static_cast<int>(factor);

	int ppm = dpi*m;							// Here we set the portable  pixmap as a product of dots per inch * m

	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};				// 'bmpfileheader' is  a variable which is used to identify what type of file is it and that is not damaged.  First two characters are B,M in ASCII code

	unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0 };			// 'bmpinfoheader' is a variable which contains info about dimensions and color format.

	bmpfileheader[ 2] = (unsigned char)(filesize);
	bmpfileheader[ 3] = (unsigned char)(filesize>>8);			// Assigning parameters
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(w);
	bmpinfoheader[ 5] = (unsigned char)(w>>8);					// Assigning parameters
	bmpinfoheader[ 6] = (unsigned char)(w>>16);
	bmpinfoheader[ 7] = (unsigned char)(w>>24);

	bmpinfoheader[ 8] = (unsigned char)(h);
	bmpinfoheader[ 9] = (unsigned char)(h >> 8);				// Assigning parameters
	bmpinfoheader[ 10] = (unsigned char)(h >> 16);
	bmpinfoheader[ 11] = (unsigned char)(h >> 24);

	bmpinfoheader[21] = (unsigned char)(s);
	bmpinfoheader[22] = (unsigned char)(s >> 8);				// Assigning parameters
	bmpinfoheader[23] = (unsigned char)(s >> 16);
	bmpinfoheader[24] = (unsigned char)(s >> 24);

	bmpinfoheader[25] = (unsigned char)(ppm);
	bmpinfoheader[26] = (unsigned char)(ppm >> 8);				// Assigning parameters
	bmpinfoheader[27] = (unsigned char)(ppm >> 16);
	bmpinfoheader[28] = (unsigned char)(ppm >> 24);

	bmpinfoheader[29] = (unsigned char)(ppm);
	bmpinfoheader[30] = (unsigned char)(ppm >> 8);				// Assigning parameters
	bmpinfoheader[31] = (unsigned char)(ppm >> 16);
	bmpinfoheader[32] = (unsigned char)(ppm >> 24);


	f = fopen(filename, "wb");									// We open the file for rendering the image

	fwrite(bmpfileheader, 1, 14, f);						// Modify the header info
	fwrite(bmpinfoheader, 1, 40, f);						// Modifying header

	for (int i = 0; i < k; i++)
	{
		RGBType rgb = data[i];

		double red = (data[i].r) * 255;						// Calculating the Red component value
		double green = (data[i].g) * 255;					// Calculating the Green component value
		double blue = (data[i].b) * 255;					// Calculating the Blue component value

		unsigned char color[3] = {(int)floor(blue) , (int)floor(green) , (int)floor(red) };				// Maps the color to the smallest following number using the floor function

		fwrite(color, 1, 3, f);									// writes to the file.

	}

	fclose(f);												// Close the file
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Name of the Method: winningObjectIndex

Input Parameters: vector of object intersections named object_intersections

Function:	-The objective of this method is to return index of the winning object.
			- This method  takes vector and then checks for the index of the minimum value.
			- The method returns negative value if there are no intersections.
			- if the intersection at particluar index is greater than 0 ten thats our index of minimum value.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


int winningObjectIndex(vector<double> object_intersections) {

	// returns the index of the winning intersection

	int index_of_minimum_value;

	// prevent unnecessary calculations

	if (object_intersections.size() == 0) {

		// If there are no intersections
		return -1;
	}
	else if(object_intersections.size() == 1) {
		if( object_intersections.at(0) > 0) {
		
			// if that intersection is greater than zero  then its our index of minimum value.
			return 0;
		}
		else {

			// otherwise the only intersection value is negative
			return -1;
		}
	}
	else {

		  // more than one intersection
		// find the maximum value in the vector first

		double max = 0;
		for (int i = 0; i < object_intersections.size(); i++)
		{
			if (max < object_intersections.at(i)) {

				max = object_intersections.at(i);
			}
		}

		// then starting from the maximum value find the minimum position

		if (max > 0) {

			 // we only want positive intersections

			for (int index = 0; index < object_intersections.size(); index++)
			{
				if (object_intersections.at(index) > 0 && object_intersections.at(index) <= max) {			// This condition checks if the object_intersections at index 'index' is a positive
					
					max = object_intersections.at(index);												//  the value is copied in 'max' variable and index is assigned to 'index_of_minimum_value
					index_of_minimum_value = index;
				}
			}

			return index_of_minimum_value;				// returns the resultant value
		}
		else {
			  // all the intersections were negative
			return -1;
		}
	}
}
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Name of the Method: renderColorAt

Input Parameters: intersectinr_ray_direction, scene_objects, index_of_winning_object, light_sources, accuracy, ambient light

Function:	-This is the most important method because all the shading, shadowing, reflection, refraction happens in this method
			- I created the checkerboard tile in this function.
			- The sphere are made reflective in this method
			- Shadows are also rendered in this method
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

// Function for all shadows , reflections and shading
Color renderColorAt(Vect intersection_position, Vect intersecting_ray_direction, vector<Object*> scene_objects, int index_of_winning_object, vector<Source*> light_sources,double accuracy, double ambientlight) {

	Color winning_object_color = scene_objects.at(index_of_winning_object)->getColor();
	Vect winning_object_normal = scene_objects.at(index_of_winning_object)->getNormalAt(intersection_position);

	// Checkerboard tile for the plane

	if (winning_object_color.getColorSpecial() == 2)
	{
		int square = (int)floor(intersection_position.getVectX()) + (int)floor(intersection_position.getVectZ());

		if ((square % 2) == 0) {
		
			// Black tile is created
			winning_object_color.setColorRed(0);
			winning_object_color.setColorGreen(0);
			winning_object_color.setColorBlue(0);
		}
		else {
			// white tile is created
			winning_object_color.setColorRed(1);
			winning_object_color.setColorGreen(1);
			winning_object_color.setColorBlue(1);
		}

	}

	Color final_color = winning_object_color.colorScalar(ambientlight);				// Here we get the color of the winning object after calculating the scalar of the color.

	if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {

		// reflection from objects with specular intensity

		double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());						// Here we calculate the equation for the reflection equation.{ I- 2(I.N)N }
		Vect scalar1 = winning_object_normal.vectMult(dot1);
		Vect add1 = scalar1.vectAdd(intersecting_ray_direction);

		Vect scalar2 = add1.vectMult(2);
		Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);

		Vect reflection_direction = add2.normalize();

		Ray reflection_ray(intersection_position, reflection_direction);					// Creating the ray with the given intersection position and calculated reflection direction

		//dtermine what ray intersects with it first

		vector<double> reflection_intersections;

		for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++) {							//  Then we push all the pixels which intersect with the reflective ray into the vector "reflection_intersections"

			reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));
		}

		int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);					// Assigning the new index_of_winning_reflection_with_reflection


		if (index_of_winning_object_with_reflection != -1) {

			// reflection ray missed everything else
			if (reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy) {
				
				// determine the position and direction at the point of intersection with the reflection
				// the ray only affects the color if it reflected off something

				Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));	
				Vect reflection_intersection_ray_direction = reflection_direction;

				// Now we Recursively call the method  to cover all the surface.

				Color reflection_intersection_color = renderColorAt(reflection_intersection_position, reflection_intersection_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);


				// Here we return the final color that is generated after reflection calculations.
				final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(winning_object_color.getColorSpecial()));


			}
		}

	}
	//----------------------------------------------------------------------------------------------------------------------------------

	// ------------------------------------------------------Adding Refraction code-----------------------------------------------------

/*	Color final_color2 = winning_object_color.colorScalar(ambientlight);

	if (winning_object_color.getColorSpecial() > 1 && winning_object_color.getColorSpecial() < 2) {

		// Implementing Refraction

		double refract_dot1 = winning_object_normal.dotProduct(intersecting_ray_direction);

		double refract_square1 = ((refract_dot1*refract_dot1)*(-1)) +1;

		double refract_square2 = ((0.5*0.5)*refract_square1*(-1))+1;											// Calculating the refraction ray equations taking refractive index into consideration

		double refract_root1 = sqrt(refract_square2);

		double refract_add1 = (0.5*refract_dot1) + refract_root1;

		Vect refractVect1 = winning_object_normal.vectMult(refract_add1).negative();

		Vect refractVect2 = intersecting_ray_direction.vectAdd(refractVect1);

		Vect refract_final = refractVect2.vectMult(0.5);

		Vect refraction_direction = refract_final.normalize();

		Ray refraction_ray(intersection_position, refraction_direction);


		//dtermine what ray intersects with it first

		vector<double> refraction_intersections;

		for (int refraction_index = 0; refraction_index < scene_objects.size(); refraction_index++) {

			refraction_intersections.push_back(scene_objects.at(refraction_index)->findIntersection(refraction_ray));
		}

		int index_of_winning_object_with_refraction = winningObjectIndex(refraction_intersections);

		cout << "before -1 Refraction loop" << endl;
		if (index_of_winning_object_with_refraction != -1) {

			// refraction ray missed everything else
			if (refraction_intersections.at(index_of_winning_object_with_refraction) > accuracy) {

				cout << "after accuracy loop" << endl;
				// determine the position and direction at the point of intersection with the refraction
				// the ray only affects the color if it reflected off something

				Vect refraction_intersection_position = intersection_position.vectAdd(refraction_direction.vectMult(refraction_intersections.at(index_of_winning_object_with_refraction)));
				Vect refraction_intersection_ray_direction = refraction_direction;

				Color refraction_intersection_color = renderColorAt(refraction_intersection_position, refraction_intersection_ray_direction, scene_objects, index_of_winning_object_with_refraction, light_sources, accuracy, ambientlight);

				final_color2 = final_color2.colorAdd(refraction_intersection_color.colorScalar(winning_object_color.getColorSpecial()));


			}
		}

	}

	 final_color1 = final_color1.colorAdd( final_color2.colorScalar(winning_object_color.getColorSpecial()));

	Color final_color = final_color.colorAdd(final_color1.colorScalar(winning_object_color.getColorSpecial()));				*/

	//-----------------------------------------------------------------------------------------------------------------------------------
	  // Refraction code ended here
	//------------------------------------------------------------Ends here---------------------------------------------------------
	
	
	for (int light_index = 0; light_index < light_sources.size(); light_index++)
	{
		Vect light_direction = light_sources.at(light_index) -> getLightPosition().vectAdd(intersection_position.negative()).normalize();

		float cosine_angle = winning_object_normal.dotProduct(light_direction);							// Here we have created the cosine angle to calculate the shadows

		if (cosine_angle > 0)
		{
			// Test for shadows 

			bool shadowed = false;

			Vect distance_to_light = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();  // Distance to light is stored in a vector 

			float distance_to_light_magnitude = distance_to_light.magnitude();  // Distance to light source from our intersection point

			Ray shadow_ray(intersection_position, light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize());			// Here we have a shadow ray to render the shadow

			vector<double> secondary_intersections;																	// Secondary intersections is the vector which contains the secondary intersection points.

			for (int object_index = 0; object_index < scene_objects.size() && shadowed == false; object_index++)
			{
				secondary_intersections.push_back(scene_objects.at(object_index)->findIntersection(shadow_ray));
			}

			for (int  c = 0; c < secondary_intersections.size(); c++)
			{
				if (secondary_intersections.at(c)> accuracy) {

					if (secondary_intersections.at(c) <= distance_to_light_magnitude)
					{
						shadowed = true;												// If the shadow we return the true.
					}
					break;
				}
				
			}

			if (shadowed == false)			// If the shadow is false we modify the final color with winning_object_color after multiplying it with light sources's light color at that particular index.
			{
				final_color = final_color.colorAdd(winning_object_color.colorMultiply(light_sources.at(light_index)->getLightColor()).colorScalar(cosine_angle));
			
				// Adding shine to image
				if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1)
				{  // special value between 0 and 1 refers to shinyness

					double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
					Vect scalar1 = winning_object_normal.vectMult(dot1);
					Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
					
					Vect scalar2 = add1.vectMult(2);
					Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);

					Vect reflection_direction = add2.normalize();

			
					double specular = reflection_direction.dotProduct(light_direction);							// Specular point calculation

					if (specular > 0)
					{
						specular = pow(specular, 10);											// We increase its intensity to make it visible
						final_color = final_color.colorAdd(light_sources.at(light_index)->getLightColor().colorScalar(specular*winning_object_color.getColorSpecial()));			// modified the final_color
					}
				}
			}
		}
	}

	return final_color.clip();
}

int thisone;					// Variable declared with all visibility

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Name of the Method: main

Input Parameters: argc, *argv[]

Function:	-The main function defines all the instances of the scene_objects, camera and light vectors.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


int main(int argc, char *argv[]) {

	int dpi = 72;
	int width = 640;
	int height = 480;

	int n = width*height;	

	double aspectratio = (double)width / (double)height;
	double ambientlight = 0.2;
	double accuracy = 0.000001;  // if intersection value is little off to get correct color, we use accuracy

	cout << "Rendering .." << endl;

	RGBType *pixels = new RGBType[n];

	double  threshold = 0.1;

	Vect O(0,0,0);
	Vect X(1, 0, 0);
	Vect Y(0, 1, 0);
	Vect Z(0, 0, 1);

	Vect new_sphere_loc(1.75, 0, 0);
	Vect camera_position(3, 1.5, -4);

	// Using vecotr operations

	// defining direction of camera

	Vect look_at(0, 0, 0);

	// this vector of 3 co-ords are difference between x,y,z  of cam position minus look_at vector
	Vect diff_btw (camera_position.getVectX() - look_at.getVectX(), camera_position.getVectY() - look_at.getVectY(), camera_position.getVectZ() - look_at.getVectZ());

	// Defining a point direction for the camera look at , it will be normalized vector in opposite direction
	Vect camera_direction = diff_btw.negative().normalize();

	Vect camright = Y.crossProduct(camera_direction).normalize();

	Vect camdown = camright.crossProduct(camera_direction);

	Camera scene_cam(camera_position, camera_direction, camright, camdown);  // Defining an instance of an camera


	// Creating colors n lights

	Color white_light(1.0, 1.0, 1.0, 0);
	Color pretty_green(0.5, 1.0, 0.5, 0.3); // special shinyness value
	Color gray(0.5, 0.5, 0.5, 0);
	Color black(0.0, 0.0, 0.0, 0);
	Color maroon(0.5,0.25,0.25,0.9);
	Color tile_floor(0,0,0,2);

	Vect light_position(-7, 10, -10);

	Light scene_light (light_position, white_light);
	vector<Source*> light_sources;
	light_sources.push_back(dynamic_cast<Source*>(&scene_light)); 

	// scene objects
	Sphere scene_sphere(O, 1, pretty_green);
	Sphere scene_sphere2(new_sphere_loc, 0.5, maroon);
	Plane scene_plane (Y, -1, tile_floor);

	// Stacking scene objects 

	vector<Object*> scene_objects;
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere));			// Pushing the all scene objects into the vector scene_objects
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere2));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_plane));

	int thisone, aa_index;
	double xamnt, yamnt;

	//  double tempRed[depth_trace*depth_trace], tempGreen[16], tempBlue[16];

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			thisone = y*width + x;

			//start with a blank pixel
		double	 tempRed[depth_trace*depth_trace];						// Temporary used to store sub-pixels for anti-aliasing
		double	 tempGreen[depth_trace*depth_trace];
		double	 tempBlue[depth_trace*depth_trace];

			for (int aax = 0; aax < depth_trace; aax++) {

				for (int aay = 0; aay < depth_trace; aay++) {


					aa_index = aay*depth_trace + aax;

					srand(time(0));

					// create the ray from the camera to this pixel

					if (depth_trace == 1) {

						// start with no anti-aliasing

						if (width > height) {

							// the image is wider than it is tall

							xamnt = ((x + 0.5) / width)*aspectratio - (((width - height) / (double)height) / 2);

							yamnt = ((height - y) + 0.5) / height;
						}
						else if (height > width) {

							// the image is taller than it is wide

							xamnt = (x + 0.5) / width;
							yamnt = (((height - y) + 0.5) / height) / aspectratio - (((height - width) / (double)width) / 2);
						}
						else {

							// the image is square
							xamnt = (x + 0.5) / width;
							yamnt = ((height - y) + 0.5) / height;

						}
					}
					else {
					
						//Anti- aliasing 
						if (width > height) {

							// the image is wider than it is tall

							xamnt = ((x + (double)aax /((double)depth_trace-1))/ width)*aspectratio - (((width - height) / (double)height) / 2);

							yamnt = ((height - y) + (double)aax / ((double)depth_trace - 1)) / height;
						}
						else if (height > width) {

							// the image is taller than it is wide

							xamnt = (x + (double)aax / ((double)depth_trace - 1)) / width;
							yamnt = (((height - y) + (double)aax / ((double)depth_trace - 1)) / height) / aspectratio - (((height - width) / (double)width) / 2);
						}
						else {

							// the image is square
							xamnt = (x + (double)aax / ((double)depth_trace - 1)) / width;
							yamnt = ((height - y) + (double)aax / ((double)depth_trace - 1)) / height;

						}
						 
					}

			Vect cam_ray_origin = scene_cam.getCameraPosition();
			Vect cam_ray_direction = camera_direction.vectAdd(camright.vectMult(xamnt - 0.5).vectAdd(camdown.vectMult(yamnt - 0.5))).normalize();

			Ray cam_ray(cam_ray_origin, cam_ray_direction);

			// vector to track intersections ...................

			vector<double> intersections;

			for (int index = 0; index < scene_objects.size(); index++) {

				intersections.push_back(scene_objects.at(index)->findIntersection(cam_ray));
			}

			int index_of_winning_object = winningObjectIndex(intersections);

			// return the color
			if (index_of_winning_object == -1) {

				// setting bgcolor black to show that ray missed the object
				tempRed[aa_index] = 0;
				tempGreen[aa_index] = 0;
				tempBlue[aa_index] = 0;
			}
			else {
				
				// Adding shadow parameters
				if (intersections.at(index_of_winning_object)> accuracy) {

					// determining the position and directions vectors at the point of intersection

					Vect intersection_position = cam_ray_origin.vectAdd(cam_ray_direction.vectMult(intersections.at(index_of_winning_object)));
					Vect intersecting_ray_direction = cam_ray_direction;

					// index corresponds to object in our scene
					Color intersection_color = renderColorAt(intersection_position, intersecting_ray_direction, scene_objects, index_of_winning_object, light_sources, accuracy, ambientlight);

					tempRed[aa_index] = intersection_color.getColorRed();				// Assigning the new color 
					tempGreen[aa_index] = intersection_color.getColorGreen();
					tempBlue[aa_index] = intersection_color.getColorBlue();

				}
			}
		}
	}

	// Average the pixel color

	double totalRed = 0;
	double totalGreen = 0;
	double totalBlue = 0;

	for (int iRed = 0; iRed < depth_trace*depth_trace; iRed++) {
																							
		totalRed = totalRed + tempRed[iRed];								//  Calculates the total avereged color
	}

	for (int iGreen = 0; iGreen < depth_trace*depth_trace; iGreen++) {
		
		totalGreen = totalGreen + tempGreen[iGreen];
	}

	for (int iBlue = 0; iBlue < depth_trace*depth_trace; iBlue++) {
	
		totalBlue = totalBlue + tempBlue[iBlue];
	}
		
	double avgRed = totalRed / (depth_trace*depth_trace);					// Taking the  average of the colors
	double avgGreen = totalGreen / (depth_trace*depth_trace);
	double avgBlue = totalBlue / (depth_trace*depth_trace);


	pixels[thisone].r = avgRed;								// Assigning this averaged color to final color components
	pixels[thisone].g = avgGreen;
	pixels[thisone].b = avgBlue; 

		}
	}

	saveImg("scene_anti.bmp", width, height, dpi, pixels);


	return 0;
}