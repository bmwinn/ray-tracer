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

int main(int argc, char *argv[]) {
	string result = "";
	int width, height, curGeom;
	float distance;
	Light light;
	Camera camera;
	color_t color, black = {0, 0, 0, 0}, white = {255, 255, 255, 0};
	vector<Geometry *> allGeometry;

	/* Attempt to open .pov file, fill in variables, and create geometry */
	if (fileOps(argc, argv, &width, &height, &allGeometry, &camera, &light))
		/* Otherwise, fileOps prints error message. Quit program. */
		return 1;

	Image img(width, height);

	cout << "Reflection on simple_reflect3" << endl;

	/* Loop through pixels */
	for (int i = 0; i < width; i++){
		for (int j = 0; j < height; j++) {
			Ray ray = Ray(i, j, width, height, &camera);
			if (i == 320 && j == 145) {
				cout << "----" << endl << "Iteration type: Primary" << endl;
				ray.PrintTest();
				result += "Pixel: [" + to_string(i) + ", " + to_string(j) + "]";
				result += " Ray: {" + to_string(ray.start.x) + ", " + to_string(ray.start.y) + ", " + to_string(ray.start.z) + "}";
				result += " -> {" + to_string(ray.direction.x) + ", " + to_string(ray.direction.y) + ", " + to_string(ray.direction.z) + "}";
			}

			curGeom = -1; // reset curGeom
			Storage storage = Storage(10000, &black);

			/* Loop through geometry */
			for (int g = 0; g < allGeometry.size(); g++) {
				/* Find distance along ray to current geometry */
				distance = allGeometry.at(g)->Intersect(i, j, &ray);

				/* Update closest distance from camera to geometry */
				if (distance > 0.001 && distance < storage.distance) {
				   storage.distance = distance;
				   curGeom = g;
				}
			}

			if (i == 320 && j == 145)
				result += " T=" + to_string(storage.distance);

			if (storage.distance == 10000 && curGeom == -1) {
				img.pixel(i, j, black);
			}
			
			else {
				storage.pigment = allGeometry.at(curGeom)->Reflect(i, j, storage.distance, ray, 0);
				storage.pigment.SetColorT(&color);
				
				if (i == 320 && j == 145) {
					result += " Color: (" + to_string(color.r) + ", " + to_string(color.g) + ", " + to_string(color.b) + ")";
				}

				img.pixel(i, j, color);
			}

			for (int g = 0; g < allGeometry.size(); g++)
				allGeometry.at(g)->ResetPigments();
		}
	}

	cout << "----" << endl << result << endl;

	img.WriteTga((char *)"simple_reflect3.tga", true);

	return 0;
}




