#include "parse.h"
#include "objs.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
using namespace std;

/* Check argc and usage, fill in variables, attempt to open povray file */
int fileOps(int argc, char *argv[], int *width, int *height, string *test, vector<Geometry *> *allGeometry, Camera *camera, Light *light) {
	string fileName;
	fstream povray;

	if (argc < 4) {
		cout << "Error. Usage: ./raytrace <width> <height> <input_filename>" << endl;
		return 1;
	}

	fileName = argv[3];
	povray.open(fileName, fstream::in);

	/* Attempt to open and parse povray file */
	if (povray.is_open()) {
		parse(&povray, allGeometry, camera, light);
		povray.close();
	}
	else {
		cout << "Error opening file." << endl;
		return 1;
	}

	*width = stoi(argv[1], NULL);
	*height = stoi(argv[2], NULL);
	*test = string(argv[3]);

	return 0;
}

/* Parse through povray file, create setting and geometry */
void parse(fstream *povray, vector<Geometry *> *allGeometry, Camera *camera, Light *light) {
	Sphere *sphere;
	Plane *plane;
	int rgbf;
	char line[100], *token;

	/* While povray file still contains unread lines */
	while (povray->getline(line, 99)) {
		/* Ignore comments in povray file */
		if (line[0] != '/') {
			/* Fill "token" with key words from file */
			token = strtok(line, " \t\n");

			if (token) {
				if (!strcmp(token, "camera")) {
					povray->getline(line, 99);
					*camera = Camera();

					/* Fill in Camera center point */
					token = strtok(line, " \tlocation<,");
					camera->center.x = strtof(token, NULL);
					token = strtok(NULL, ", ");
					camera->center.y = strtof(token, NULL);
					token = strtok(NULL, ", >");
					camera->center.z = strtof(token, NULL);

					/* Fill in Camera up vector */
					povray->getline(line, 99);
					token = strtok(line, " \tup<,");
					camera->up.x = strtof(token, NULL);
					token = strtok(NULL, ", ");
					camera->up.y = strtof(token, NULL);
					token = strtok(NULL, ", >");
					camera->up.z = strtof(token, NULL);

					/* Fill in Camera right vector */
					povray->getline(line, 99);
					token = strtok(line, " \tright<,");
					camera->right.x = strtof(token, NULL);
					token = strtok(NULL, ", ");
					camera->right.y = strtof(token, NULL);
					token = strtok(NULL, ", >");
					camera->right.z = strtof(token, NULL);

					/* Fill in Camera lookat point */
					povray->getline(line, 99);
					token = strtok(line, " \tlook_at<,");
					camera->lookat.x = strtof(token, NULL);
					token = strtok(NULL, ", ");
					camera->lookat.y = strtof(token, NULL);
					token = strtok(NULL, ", >");
					camera->lookat.z = strtof(token, NULL);

					/* Initialize magnitude of up and right vectors */
					camera->up.SetMagnitude(camera->up.x, camera->up.y, camera->up.z);
					camera->right.SetMagnitude(camera->right.x, camera->right.y, camera->right.z);
				}
				else if (!strcmp(token, "light_source")) {
					*light = Light();

					/* Fill in Light center point */
					token = strtok(NULL, " {<,");
					light->center.x = strtof(token, NULL);
					token = strtok(NULL, " {<,");
					light->center.y = strtof(token, NULL);
					token = strtok(NULL, " {<,");
					light->center.z = strtof(token, NULL);

					strtok(NULL, " ");
					token = strtok(NULL, " ");

					/* Determine if Pigment color is rgb or rgbf */
					rgbf = !strcmp(token, "rgbf");

					/* Fill in Light Pigment */
					token = strtok(NULL, "< ,");
					light->pigment.r = strtof(token, NULL);
					token = strtok(NULL, " ,");
					light->pigment.g = strtof(token, NULL);
					token = strtok(NULL, " ,");
					light->pigment.b = strtof(token, NULL);
					
					/* Fill in Light Pigment f value */
					if (rgbf) {
						token = strtok(NULL, " >}");
						light->pigment.f = strtof(token, NULL);
					}
					else
						light->pigment.f = 1;
				}
				else if (!strcmp(token, "sphere")) {
					sphere = new Sphere();

					/* Fill in sphere center point */
					token = strtok(NULL, " {<,");
					sphere->center.x = strtof(token, NULL);
					token = strtok(NULL, " ,");
					sphere->center.y = strtof(token, NULL);
					token = strtok(NULL, " >");
					sphere->center.z = strtof(token, NULL);

					/* Fill in sphere radius */
					token = strtok(NULL, " ,");
					sphere->radius = strtof(token, NULL);

					/* Fill in sphere Pigment */
					povray->getline(line, 99);
					token = strtok(line, " pigment{");
					token = strtok(NULL, " ");

					/* Determine if Pigment color is rgb or rgbf */
					rgbf = !strcmp(token, "rgbf");

					token = strtok(NULL, " <,");
					sphere->pigment.r = strtof(token, NULL);
					token = strtok(NULL, " ,");
					sphere->pigment.g = strtof(token, NULL);
					token = strtok(NULL, " ,>}");
					sphere->pigment.b = strtof(token, NULL);

					if (rgbf) {
						token = strtok(NULL, " ,>}");
						sphere->pigment.f = strtof(token, NULL);
					}
					else
						sphere->pigment.f = 1;

					/* Fill in sphere Finish */
					povray->getline(line, 99);
					token = strtok(line, "finish {ambient");
					sphere->finish.ambient = strtof(token, NULL);
					token = strtok(NULL, "diffuse ");
					sphere->finish.diffuse = strtof(token, NULL);

					/* Add sphere to vector list of geometry */
					allGeometry->push_back(sphere);
				}
				else if (!strcmp(token, "plane")) {
					plane = new Plane();

					/* Fill in plane normal vector */
					token = strtok(NULL, " {<,");
					plane->normal.x = strtof(token, NULL);
					token = strtok(NULL, " ,");
					plane->normal.y = strtof(token, NULL);
					token = strtok(NULL, " ,>");
					plane->normal.z = strtof(token, NULL);

					/* Fill in distance along plane normal */
					token = strtok(NULL, " ,");
					plane->distance = strtof(token, NULL);

					/* Fill in plane Pigment */
					povray->getline(line, 99);
					token = strtok(line, "pigment {");
					token = strtok(NULL, " ");

					/* Determine if Pigment is rgb or rgbf */
					rgbf = !strcmp(token, "rgbf");

					token = strtok(NULL, " <,");
					plane->pigment.r = strtof(token, NULL);
					token = strtok(NULL, " ,");
					plane->pigment.g = strtof(token, NULL);
					token = strtok(NULL, " ,>}");
					plane->pigment.b = strtof(token, NULL);

					if (rgbf) {
						token = strtok(NULL, " ,>}");
						plane->pigment.f = strtof(token, NULL);
					}
					else
						plane->pigment.f = 1;

					/* Fill in plane Finish */
					povray->getline(line, 99);
					token = strtok(line, "finish {ambient");
					plane->finish.ambient = strtof(token, NULL);
					token = strtok(NULL, "diffuse ");
					plane->finish.diffuse = strtof(token, NULL);

					/* Set Magnitude of normal vector */
					plane->normal.SetMagnitude(plane->normal.x, plane->normal.y, plane->normal.z);
					
					/* Add plane to vector list of Geometry */
					allGeometry->push_back(plane);
				}
			}
		}
	}
}