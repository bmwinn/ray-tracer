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
int fileOps(int argc, char *argv[], int *width, int *height, vector<Geometry *> *allGeometry, Camera *camera, Light *light) {
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

	for (int g = 0; g < allGeometry->size(); g++) {
		allGeometry->at(g)->light = light;
		allGeometry->at(g)->camera = camera;
		allGeometry->at(g)->allGeometry = allGeometry;
	}

	return 0;
}

/* Parse through povray file, create setting and geometry */
void parse(fstream *povray, vector<Geometry *> *allGeometry, Camera *camera, Light *light) {
	Sphere *sphere;
	Plane *plane;
	Triangle *triangle;
	int rgbf, vals;
	char line[100], finishLine[100], *token;
	string finishVals[6];


	/* While povray file still contains unread lines */
	while (povray->getline(line, 99)) {

		/* Ignore comments in povray file */
		if (line[0] != '/') {
			/* Fill "token" with key words from file */
			/* Watch out for empty lines */
			token = strtok(line, " \t\n}");

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
						light->pigment.f = 0;
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
					fillPigment(line, sphere);

					/* Fill in sphere Finish */
					povray->getline(line, 99);
					fillFinish(line, sphere);

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

					/* Set Magnitude of normal vector */
					plane->normal.SetMagnitude(plane->normal.x, plane->normal.y, plane->normal.z);
					plane->normal.Normalize();

					/* Fill in distance along plane normal */
					token = strtok(NULL, " ,");
					plane->distance = strtof(token, NULL);

					plane->SetOnGeom();

					/* Fill in plane Pigment */
					povray->getline(line, 99);
					fillPigment(line, plane);

					/* Fill in plane Finish */
					povray->getline(line, 99);
					fillFinish(line, plane);

					/* Add plane to vector list of Geometry */
					allGeometry->push_back(plane);
				}
				else if (!strcmp(token, "triangle")) {
					triangle = new Triangle();

					/* Fill in triangle vertexA */
					povray->getline(line, 99);
					token = strtok(line, " \t{<,");
					triangle->vertexA.x = strtof(token, NULL);
					token = strtok(NULL, " ,");
					triangle->vertexA.y = strtof(token, NULL);
					token = strtok(NULL, " ,>");
					triangle->vertexA.z = strtof(token, NULL);

					/* Fill in triangle vertexB */
					povray->getline(line, 99);
					token = strtok(line, " \t{<,");
					triangle->vertexB.x = strtof(token, NULL);
					token = strtok(NULL, " ,");
					triangle->vertexB.y = strtof(token, NULL);
					token = strtok(NULL, " ,>");
					triangle->vertexB.z = strtof(token, NULL);

					/* Fill in triangle vertexC */
					povray->getline(line, 99);
					token = strtok(line, " \t{<,");
					triangle->vertexC.x = strtof(token, NULL);
					token = strtok(NULL, " ,");
					triangle->vertexC.y = strtof(token, NULL);
					token = strtok(NULL, " ,>");
					triangle->vertexC.z = strtof(token, NULL);

					triangle->SetVectors();

					/* Fill in triangle Pigment */
					povray->getline(line, 99);
					fillPigment(line, triangle);

					/* Fill in plane Finish */
					povray->getline(line, 99);
					fillFinish(line, triangle);

					/* Add plane to vector list of Geometry */
					allGeometry->push_back(triangle);
				}
			}
		}
	}
}

void fillFinish(char *line, Geometry *geom) {
	char finishLine[100], *token;

	for (int i = 0, j = 0; i < 100; i++) {
		if (line[i] != '{' && line[i] != '}')
			finishLine[j++] = line[i];
	}

	token = strtok(finishLine, " \t");

	while ((token = strtok(NULL, " \t"))) {
		if (!geom->finish.ambient && !strcmp(token, "ambient")) {
			token = strtok(NULL, " \t");
			geom->finish.ambient = strtof(token, NULL);
		}
		else if (!geom->finish.diffuse && !strcmp(token, "diffuse")) {
			token = strtok(NULL, " \t");
			geom->finish.diffuse = strtof(token, NULL);
		}
		else if (!geom->finish.specular && !strcmp(token, "specular")) {
			token = strtok(NULL, " \t");
			geom->finish.specular = strtof(token, NULL);
		}
		else if (!strcmp(token, "roughness")) {
			token = strtok(NULL, " \t");
			geom->finish.roughness = strtof(token, NULL);
		}						
		else if (!geom->finish.refract && !strcmp(token, "refraction")) {
			token = strtok(NULL, " \t");
			geom->finish.refract = strtof(token, NULL);
		}
		else if (!geom->finish.reflect && !strcmp(token, "reflection")) {
			token = strtok(NULL, " \t");
			geom->finish.reflect = strtof(token, NULL);
		}
		else if (!geom->finish.ior && !strcmp(token, "ior")) {
			token = strtok(NULL, " \t");
			geom->finish.ior = strtof(token, NULL);
		}
	}
}

void fillPigment(char *line, Geometry *geom) {
	int rgbf;
	char *token;

	token = strtok(line, "pigment {");
	token = strtok(NULL, " ");

	/* Determine if Pigment is rgb or rgbf */
	rgbf = !strcmp(token, "rgbf");

	token = strtok(NULL, " <,");
	geom->pigment.r = strtof(token, NULL);
	token = strtok(NULL, " ,");
	geom->pigment.g = strtof(token, NULL);
	token = strtok(NULL, " ,>}");
	geom->pigment.b = strtof(token, NULL);

	if (rgbf) {
		token = strtok(NULL, " ,>}");
		geom->pigment.f = strtof(token, NULL);
	}
	else
		geom->pigment.f = 0;
}



