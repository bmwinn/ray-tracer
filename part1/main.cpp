#include "parse.h"
#include "objs.h"
#include "Image.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <cmath>

using namespace std;

/* Print unit test information for spheres.pov and planes.pov */
void printUnitTest(string *test, int i, int j, float closestDistance, Ray *ray, Pigment *pixelPigment, color_t *color) {
	if ((!test->compare("planes.pov") && ((i == 320 && j == 50) || (i == 50 && j == 240) || (i == 590 && j == 240))) || 
		(!test->compare("spheres.pov") && ((i == 320 && j == 239) || (i == 360 && j == 219) || (i == 230 && j == 239) || (i == 120 && j == 349) || (i == 490 && j == 119)))) {
		cout << "Pixel: [" << i << ", " << j << "]  Ray: {" << ray->start.x << " " << ray->start.y << " " << ray->start.z << "}";
		cout << " -> {" << ray->direction.x << " " << ray->direction.y << " " << ray->direction.z << "}";
		
		if (closestDistance == 10000)
			cout << "\tno hit       no hit" << endl;
		else {
			cout << "\tt = " << closestDistance;

			if (!test->compare("spheres.pov"))
				cout << "  Color: (" << (int) color->r << " " << (int) color->g << " " << (int) color->b << ")" << endl;
			else
				cout << "  Color: (" << pixelPigment->r << " " << pixelPigment->g << " " << pixelPigment->b << ")" << endl;
		}
	}
}

/* Fill color_t variable (with Image.cpp compatibility) from my own Pigment class */
void setColor(color_t *color, Pigment *pixelPigment) {
	color->r = pixelPigment->r * 255;
	color->g = pixelPigment->g * 255;
	color->b = pixelPigment->b * 255;
	color->f = pixelPigment->f;
}

int main(int argc, char *argv[]) {
	int width, height;
	float distance, closestDistance;
	string test;
	Light light;
	Camera camera;
	color_t color, black = {0, 0, 0, 0};
	vector<Geometry *> allGeometry;

	/* Attempt to open .pov file, fill in variables, and create geometry */
	if (fileOps(argc, argv, &width, &height, &test, &allGeometry, &camera, &light))
		/* Otherwise, fileOps prints error message. Quit program. */
		return 1;

	Image img(width, height);

	/* Loop through pixels */
	for (int i = 0; i < width; i++){
		for (int j = 0; j < height; j++) {
			Ray ray = Ray(i, j, width, height, &camera);
			Pigment pixelPigment = Pigment();
			closestDistance = 10000;

			/* Initialize current pixel to background color */
			img.pixel(i, j, black);

			/* Loop through geometry */
			for (int g = 0; g < allGeometry.size(); g++) {
				/* Find distance along ray to current geometry */
				distance = allGeometry.at(g)->Intersect(&ray, &camera);

				/* Update closest distance from camera to geometry */
				if (distance > 0 && distance < closestDistance) {
				   closestDistance = distance;
				   pixelPigment = allGeometry.at(g)->pigment;
					setColor(&color, &pixelPigment);

					/* Update current pixel color to geometry color */
					img.pixel(i, j, color);
				}
			}

			/* Print unit test results */ 
			printUnitTest(&test, i, j, closestDistance, &ray, &pixelPigment, &color);
		}
	}

	img.WriteTga((char *)"raytrace.tga", true);

	return 0;
}




