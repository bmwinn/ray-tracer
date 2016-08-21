#include "objs.h"
#include "Image.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
using namespace std;

/*                 *                Basic Geometry             *                 */

Point::Point() {
	x = y = z = 0;
}

Point::Point(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

/* Return distance between this and other Point */
float Point::Distance(Point *other) {
	return sqrt(pow((other->x - x), 2) + pow((other->y - y), 2) + pow((other->z - z), 2));
}

/* Print this Point, povray style */
void Point::Print() {
	cout << "Point {" << x << ", " << y << ", " << z << "}" << endl;
}

Vector::Vector() {
	x = y = z = magnitude = 0;
}

Vector::Vector(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
	magnitude = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

/* Comes in handy when xyz of Vector are initialized separately (ex. during parsing) */
void Vector::SetMagnitude(float x, float y, float z) {
	magnitude = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

 /* Return dot product between this and other Vector objects */
float Vector::Dot(Vector *other) {
	return (x * other->x) + (y * other->y) + (z * other->z);
}

 /* Fill in result Vector object with cross product between this and other Vector objects */
void Vector::Cross(Vector *other, Vector *result) {
	result->x = (y * other->z) - (z * other->y);
	result->y = (z * other->x) - (x * other->z);
	result->z = (x * other->y) - (y * other->x);
	result->SetMagnitude(result->x, result->y, result->z);
}

/* Normalize this Vector object */
void Vector::Normalize() {
	if (magnitude <= 0) {
		cout << "bad magnitude" << endl;
		Print();
	}

	if (magnitude > 0) {
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
		magnitude = 1;
	}
}

/* Multiply each vector component by scalar, update magnitude */
void Vector::Scale(float scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
	SetMagnitude(x, y, z);
}

/* Print Vector povray style */
void Vector::Print() {
	cout << "Vector <" << x << ", " << y << ", " << z << ">" << endl;
}

Ray::Ray() {
	start = Point();
	direction = Vector();
}

Ray::Ray(Point *start, Vector *direction) {
	this->start = Point(start->x, start->y, start->z);
	this->direction = Vector(direction->x, direction->y, direction->z);
}

/* Constructor comes in handy during main pixel loop */
Ray::Ray(int i, int j, int width, int height, Camera *camera) {
	float us, vs, ws, right, left, bottom, top;
	start = Point(camera->center.x, camera->center.y, camera->center.z);
	
	right = camera->right.magnitude / 2.0;
	left = -1 * camera->right.magnitude / 2.0;
	bottom = -1 * camera->up.magnitude / 2.0;
	top = camera->up.magnitude / 2.0;

	us = left + (right - left) * (i + 0.5) / (float) width;
	vs = bottom + (top - bottom) * (j + 0.5) / (float) height;
	ws = -1;

	/* Find, normalize new basis vectors */
	Vector vectorW = Vector(camera->center.x - camera->lookat.x, camera->center.y - camera->lookat.y, camera->center.z - camera->lookat.z);
	vectorW.Normalize();

	Vector vectorU = Vector(camera->right.x, camera->right.y, camera->right.z);
	vectorU.Normalize();
	
	Vector vectorV = Vector();
	vectorW.Cross(&vectorU, &vectorV);
	
	/* Scale new basis vectors */
	vectorU.Scale(us);
	vectorV.Scale(vs);
	vectorW.Scale(ws);

	direction = Vector(vectorU.x + vectorV.x + vectorW.x, vectorU.y + vectorV.y + vectorW.y, vectorU.z + vectorV.z + vectorW.z);
	direction.Normalize();
}

/* Print Ray povray style */
void Ray::Print() {
	cout << "Ray: " << endl << "   ";
	start.Print();
	cout << "   ";
	direction.Print();
}





/*            		 *                  Colors			      *        	        */

/* Initialize Pigment to black */
Pigment::Pigment() {
	r = g = b = f = 0;
}

Pigment::Pigment(float r, float g, float b) {
	this->r = r;
	this->g = g;
	this->b = b;
	f = 0;
}

Pigment::Pigment(float r, float g, float b, float f) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->f = f;
}

/* Add other pigment to this pigment */
/* Caps rgb values at [0, 1] */
void Pigment::Add(Pigment *other) {
	r = r + other->r;
	g = g + other->g;
	b = b + other->b;

	if (r < 0)
		r = 0;
	else if (r > 1)
		r = 1;

	if (g < 0)
		g = 0;
	else if (g > 1)
		g = 1;

	if (b < 0)
		b = 0;
	else if (b > 1)
		b = 1;
}

/* Fill color_t variable (with Image.cpp compatibility) from my own Pigment class */
void Pigment::SetColorT(color_t *color) {
	color->r = r * 255;
	color->g = g * 255;
	color->b = b * 255;
	color->f = 1;

	if (color->r > 255)
		color->r = 255;
	else if (color->r < 0)
		color->r = 0;

	if (color->g > 255)
		color->g = 255;
	else if (color->g < 0)
		color->g = 0;

	if (color->b > 255)
		color->b = 255;
	else if (color->b < 0)
		color->b = 0;	
}

/* Print Pigment in povray format */
void Pigment::Print() {
	cout << "Pigment: " << r << ", " << g << ", " << b << endl;
}

Finish::Finish() {
	ambient = diffuse = specular = roughness = 0;
}

Finish::Finish(float ambient, float diffuse, float specular, float roughness) {
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->roughness = roughness;
}

/* Print Finish in povray format */
void Finish::Print() {
	cout << "Finish: ambient " << ambient << " diffuse " << diffuse << " specular " << specular << " roughness " << roughness << endl;
}





/*            		*                  Scene Setup				    *               */

Light::Light() {
	center = Point();
	pigment = Pigment();
}

Light::Light(Point center, Pigment pigment) {
	this->center = center;
	this->pigment = pigment;
}

/* Print Light in povray format */
void Light::Print() {
	cout << "light {<" << center.x << ", " << center.y << ", " << center.z << "> ";
	cout << "color <" << pigment.r << ", " << pigment.g << ", " << pigment.b << ", " << pigment.f << ">}" << endl;
}

Camera::Camera() {
	center = Point();
	lookat = Point();
	up = Vector();
	right = Vector();
}

Camera::Camera(Point center, Vector up, Vector right, Point lookat) {
	this->center = center;
	this->up = up;
	this->right = right;
	this->lookat = lookat;
}

/* Print Camera in povray format */
void Camera::Print() {
	cout << "camera {" << endl;
	cout << "  location   <" << center.x << ", " << center.y << ", " << center.z << ">" << endl;
	cout << "  up         <" << up.x << ", " << up.y << ", " << up.z << ">" << endl;
	cout << "  right      <" << right.x << ", " << right.y << ", " << right.z << ">" << endl;
	cout << "  look_at    <" << lookat.x << ", " << lookat.y << ", " << lookat.z << ">" << endl;
	cout << "}" << endl;
}





/*            		 *                  Geometry				    *                  */

Geometry::Geometry() {
	normal = Vector();
	pigment = Pigment();
	pigmentA = Pigment();
	pigmentD = Pigment();
	pigmentS = Pigment();
	finish = Finish();
}

/* Virtual function, should not be called */
void Geometry::Print() {
	cout << "This is a Geometry object." << endl;
}

/* Virtual function, should not be called */
float Geometry::Intersect(Ray *ray, Point *point) {
	cout << "Geometry object intersect." << endl;
	return -1;
}

/* Set Point on Geometry itself, along initial Ray from camera */
void Geometry::SetOnGeom(Ray *ray, float rayDistance) {
	onGeom.x = ray->start.x + rayDistance * ray->direction.x;
	onGeom.y = ray->start.y + rayDistance * ray->direction.y;
	onGeom.z = ray->start.z + rayDistance * ray->direction.z;
}

/* Virtual function, should not be called */
void Geometry::BlinnPhong(int g, Ray *ray, float rayDistance, Pigment *pixelPigment, Light *light, Camera *camera, vector<Geometry *> *allGeometry) {
	cout << "Geometry object Blinn Phong." << endl;
}

/* Find Ambient Pigment for Blinn Phong */
void Geometry::BlinnPhongAmbient(Pigment *pixelPigment, Light *light) {
	Pigment cappedLight = Pigment(light->pigment.r, light->pigment.g, light->pigment.b);
	if (cappedLight.r > 1)
		cappedLight.r = 1;

	if (cappedLight.g > 1)
		cappedLight.g = 1;

	if (cappedLight.b > 1)
		cappedLight.b = 1;

	pigmentA.r = finish.ambient * pigment.r * cappedLight.r;
	pigmentA.g = finish.ambient * pigment.g * cappedLight.g;
	pigmentA.b = finish.ambient * pigment.b * cappedLight.b;

	pixelPigment->Add(&pigmentA);
}

/* Find Diffuse Pigment for Blinn Phong */
void Geometry::BlinnPhongDiffuse(Pigment *pixelPigment, Light *light) {
	float zero = 0;

	Vector lightVector = Vector(light->center.x - onGeom.x, light->center.y - onGeom.y, light->center.z - onGeom.z);
	lightVector.Normalize();

	pigmentD.r = finish.diffuse * pigment.r * light->pigment.r * max(normal.Dot(&lightVector), zero);
	pigmentD.g = finish.diffuse * pigment.g * light->pigment.g * max(normal.Dot(&lightVector), zero);
	pigmentD.b = finish.diffuse * pigment.b * light->pigment.b * max(normal.Dot(&lightVector), zero);

	pixelPigment->Add(&pigmentD);
}

/* Find Specular Pigment for Blinn Phong */
void Geometry::BlinnPhongSpecular(Pigment *pixelPigment, Light *light, Camera *camera) {
	float zero = 0;
	Vector lightVector = Vector(light->center.x - onGeom.x, light->center.y - onGeom.y, light->center.z - onGeom.z);
	Vector view = Vector(camera->center.x - onGeom.x, camera->center.y - onGeom.y, camera->center.z - onGeom.z);

	lightVector.Normalize();
	view.Normalize();

	Vector half = Vector(view.x + lightVector.x, view.y + lightVector.y, view.z + lightVector.z);
	half.Normalize();

	float shiny = 1.0/finish.roughness;

	pigmentS.r = finish.specular * pigment.r * light->pigment.r * pow(max(half.Dot(&normal), zero), shiny);
	pigmentS.g = finish.specular * pigment.g * light->pigment.g * pow(max(half.Dot(&normal), zero), shiny);
	pigmentS.b = finish.specular * pigment.b * light->pigment.b * pow(max(half.Dot(&normal), zero), shiny);

	pixelPigment->Add(&pigmentS);
}

/* Send Shadow Feeler ray from current geometry */
/* Return boolean that determines if another object blocks the light source from current object */
bool Geometry::ShadowFeeler(int g, Light *light, vector<Geometry *> *allGeometry) {
	float dist = 0;
	float lightDistance = onGeom.Distance(&light->center);

	Vector feelVector = Vector(light->center.x - onGeom.x, light->center.y - onGeom.y, light->center.z - onGeom.z);
	feelVector.Normalize();
	feeler = Ray(&onGeom, &feelVector);

	for (int geom = 0; geom < allGeometry->size(); geom++) {
		if (geom != g) 
			dist = allGeometry->at(geom)->Intersect(&feeler, &onGeom);

		/* if object with positive distance is closer than light source */
		if (dist > 0.001 && dist < lightDistance)
			return false; /* Don't color pixel */
	}

	return true;
}

/* Reset pigments for next Blinn Phong */
void Geometry::ResetPigments() {
	pigmentA = Pigment();
	pigmentD = Pigment();
	pigmentS = Pigment();
}

Sphere::Sphere() {
	center = Point();
	onGeom = Point();
	radius = 0;
	normal = Vector();
	pigment = Pigment();
	pigmentA = Pigment();
	pigmentD = Pigment();
	pigmentS = Pigment();
	finish = Finish();
}

Sphere::Sphere(Point center, float radius, Pigment pigment, Finish finish) {
	this->center = center;
	this->radius = radius;
	this->pigment = pigment;
	this->finish = finish;
	pigmentA = Pigment();
	pigmentD = Pigment();
	pigmentS = Pigment();
	onGeom = Point();
}

/* Print sphere in povray format */
void Sphere::Print() {
	cout << "sphere { ";
	cout << "<" << center.x << ", " << center.y << ", " << center.z << ">, " << radius << endl;
	cout << "  pigment { color <" << pigment.r << ", " << pigment.g << ", " << pigment.b << ", " << pigment.f << ">}" << endl;
	cout << "  finish {ambient " << finish.ambient << " diffuse " << finish.diffuse;
	cout << " specular " << finish.specular << " roughness " << finish.roughness << "}" << endl;
	cout << "  onGeom <" << onGeom.x << ", " << onGeom.y << ", " << onGeom.z << ">" << endl;
	cout << "}" << endl;
}

 /* Return distance from point along ray to sphere */
float Sphere::Intersect(Ray *ray, Point *point) {
	float distance, t1, t2, rad;

	Vector rayD = Vector(ray->direction.x, ray->direction.y, ray->direction.z);
	Vector difPC = Vector(point->x - center.x, point->y - center.y, point->z - center.z);

	rad = pow(rayD.Dot(&difPC), 2) - rayD.Dot(&rayD) * ((difPC.Dot(&difPC)) - pow(radius, 2));

	/* If radicand is negative, return no hit */
	if (rad < 0) {
		distance = -1;
	}

	/* Return smallest (closest) positive distance */
	else if (rad == 0) {
		distance = -1 * rayD.Dot(&difPC) / rayD.Dot(&rayD);
	}

	else {
		t1 = (-1 * rayD.Dot(&difPC) + sqrt(rad)) / rayD.Dot(&rayD);
		t2 = (-1 * rayD.Dot(&difPC) - sqrt(rad)) / rayD.Dot(&rayD);

		if (t1 > 0 && t2 > 0)
			distance = t1 < t2 ? t1 : t2;
		else if (t1 > 0 && t2 < 0)
			distance = t1;
		else if (t1 < 0 && t2 > 0)
			distance = t2;
		else if (t1 < 0 && t2 < 0)
			distance = -1;
	}

	return distance;
}

/* Blinn Phong BRDF for Sphere object */
void Sphere::BlinnPhong(int g, Ray *ray, float rayDistance, Pigment *pixelPigment, Light *light, Camera *camera, vector<Geometry *> *allGeometry) {
	SetOnGeom(ray, rayDistance);
	normal = Vector((onGeom.x - center.x)/radius, (onGeom.y - center.y)/radius, (onGeom.z - center.z)/radius);
	normal.Normalize();

	BlinnPhongAmbient(pixelPigment, light);
	bool noShadow = ShadowFeeler(g, light, allGeometry);

	/* If current point on sphere is not in shadow, add Diffuse and Specular Pigments */
	if (noShadow) {
		BlinnPhongDiffuse(pixelPigment, light);
		BlinnPhongSpecular(pixelPigment, light, camera);
	}
}

Plane::Plane() {
	normal = Vector();
	onGeom = Point();
	distance = 0;
	pigment = Pigment();
	pigmentA = Pigment();
	pigmentD = Pigment();
	pigmentS = Pigment();
	finish = Finish();
}

Plane::Plane(Vector normal, float distance, Pigment pigment, Finish finish) {
	this->normal = normal;
	this->distance = distance;
	this->pigment = pigment;
	this->finish = finish;
	pigmentA = Pigment();
	pigmentD = Pigment();
	pigmentS = Pigment();
	onGeom = Point();
}

/* Print plane in povray format */
void Plane::Print() {
	cout << "plane {";
	cout << "<" << normal.x << ", " << normal.y << ", " << normal.z << ">, " << distance << endl;
	cout << "  pigment {color <" << pigment.r << ", " << pigment.g << ", " << pigment.b << ", " << pigment.f << ">}" << endl;
	cout << "  finish {ambient " << finish.ambient << " diffuse " << finish.diffuse << "}" << endl;
	cout << "  onGeom <" << onGeom.x << ", " << onGeom.y << ", " << onGeom.z << ">" << endl;
	cout << "}" << endl;
}

/* Initialize point on plane according to povray info */
/* Useful when parsing, otherwise use Geometry::SetOnGeom */
void Plane::SetOnGeom() {
	onGeom.x = distance * normal.x;
	onGeom.y = distance * normal.y;
	onGeom.z = distance * normal.z;
}

 /* Return distance from point along ray to plane */
float Plane::Intersect(Ray *ray, Point *point) {
	float distance;
	Vector rayD = Vector(ray->direction.x, ray->direction.y, ray->direction.z);
	Vector difObjectPlane = Vector(onGeom.x - point->x, onGeom.y - point->y, onGeom.z - point->z);

	/* If dot product is 0, return no hit */
	if (rayD.Dot(&normal) == 0)
		distance = -1;
	else
		distance = difObjectPlane.Dot(&normal)/rayD.Dot(&normal);
	
	return distance;
}

/* Blinn Phong BRDF for plane object */
void Plane::BlinnPhong(int g, Ray *ray, float rayDistance, Pigment *pixelPigment, Light *light, Camera *camera, vector<Geometry *> *allGeometry) {
	Geometry::SetOnGeom(ray, rayDistance);
	BlinnPhongAmbient(pixelPigment, light);

	bool noShadow = ShadowFeeler(g, light, allGeometry);

	/* If current point on plane is not in shadow, add Diffuse Pigment */
	if (noShadow) {
		BlinnPhongDiffuse(pixelPigment, light);
	}
}





