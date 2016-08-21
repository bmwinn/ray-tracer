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
void printUnitTest1(string *test, int i, int j, float closestDistance, Ray *ray, Pigment *pixelPigment, color_t *color) {
	if ((!test->compare("planes.pov") && ((i == 320 && j == 50) || (i == 50 && j == 240) || (i == 590 && j == 240))) ||
		(!test->compare("spheres.pov") && ((i == 320 && j == 239) || (i == 360 && j == 219) || (i == 230 && j == 239) || (i == 120 && j == 349) || (i == 490 && j == 119)))) {
		cout << "Pixel: [" << i << ", " << j << "]  Ray: {" << ray->start.x << " " << ray->start.y << " " << ray->start.z << "}";
		cout << " -> {" << ray->direction.x << " " << ray->direction.y << " " << ray->direction.z << "}";
		
		if (closestDistance < 0 || closestDistance == 10000)
			cout << "\tno hit       no hit" << endl;
		else {
			cout << "\tT=" << closestDistance;

			if (!test->compare("spheres.pov"))
				cout << "  Color: (" << (int) color->r << " " << (int) color->g << " " << (int) color->b << ")" << endl;
			else
				cout << "  Color: (" << pixelPigment->r << " " << pixelPigment->g << " " << pixelPigment->b << ")" << endl;
		}
	}
}

void printUnitTest2(string *test, int i, int j, int curGeom, float closestDistance, Ray *ray, Geometry *geometry, Light *light, Camera *camera) {
	if (!test->compare("simple.pov") && ((i == 420 && j == 130) || (i == 120 && j == 120) || (i == 295 && j == 265))) {
		if (i == 420 && j == 130)
			cout << "Shadow" << endl;
		else if (i == 120 && j == 120)
			cout << "Diffuse plane" << endl;
		else
			cout << "Specular highlight on sphere" << endl;

		cout << "Pixel: [" << i << ", " << j << "] Ray: {" << ray->start.x << " " << ray->start.y << " " << ray->start.z << "}";
		cout << " -> {" << ray->direction.x << " " << ray->direction.y << " " << ray->direction.z << "}";

		if (closestDistance < 0)
			cout << "\tnot hit\tno hit" << endl;
		else {
			cout << "\tT=" << closestDistance << endl;
		}

		cout << "Ambient: (" << geometry->pigmentA.r * 255 << ", " << geometry->pigmentA.g * 255 << ", " << geometry->pigmentA.b * 255 << ") ";
		cout << "Diffuse: (" << geometry->pigmentD.r * 255 << ", " << geometry->pigmentD.g * 255 << ", " << geometry->pigmentD.b * 255 << ") ";
		cout << "Specular: (" << geometry->pigmentS.r * 255 << ", " << geometry->pigmentS.g * 255 << ", " << geometry->pigmentS.b * 255 << ")" << endl;
		cout << "Feeler: {" << geometry->feeler.direction.x << " " << geometry->feeler.direction.y << " " << geometry->feeler.direction.z << "}" << endl;
		cout << endl;
	}
}

int main(int argc, char *argv[]) {
	int width, height, curGeom;
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
			closestDistance = 10000;
			curGeom = 0;
			/* Initialize current pixel to background color */
			img.pixel(i, j, black);

			/* Loop through geometry */
			for (int g = 0; g < allGeometry.size(); g++) {
				/* Find distance along ray to current geometry */
				distance = allGeometry.at(g)->Intersect(&ray, &camera.center);

				/* Update closest distance from camera to geometry */
				if (distance > 0 && distance < closestDistance) {
				   closestDistance = distance;
				   curGeom = g;
				   Pigment pixelPigment = Pigment(); //Don't add colors up from previous objects
				   allGeometry.at(g)->BlinnPhong(g, &ray, closestDistance, &pixelPigment, &light, &camera, &allGeometry);
				   pixelPigment.SetColorT(&color);

					/* Update current pixel color to geometry color */
					img.pixel(i, j, color);
				}
			}
			/* Print unit test results */ 
			printUnitTest2(&test, i, j, curGeom, closestDistance, &ray, allGeometry.at(curGeom), &light, &camera);

			for (int g = 0; g < allGeometry.size(); g++) {
				allGeometry.at(g)->ResetPigments();
			}
		}
	}

	img.WriteTga((char *)"planes.tga", true);

	return 0;
}




