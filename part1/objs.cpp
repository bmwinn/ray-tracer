#include "objs.h"
#include <vector>
#include <cmath>
#include <iostream>
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

Vector::Vector() {
	x = y = z = magnitude = 0;
}

Vector::Vector(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
	magnitude = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

/* Comes in handy while parsing */
void Vector::SetMagnitude(float x, float y, float z) {
	magnitude = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

 /* Return dot product between this and other Vector objects */
float Vector::Dot(Vector *other) {
	return x * other->x + y * other->y + z * other->z;
}

 /* Fill in result Vector object with cross product between this and other Vector objects */
void Vector::Cross(Vector *other, Vector *result) {
	result->x = y * other->z - z * other->y;
	result->y = -1 * (x * other->z - z * other->x);
	result->z = x * other->y - y * other->x;
}

/* Normalize this Vector object */
void Vector::Normalize() {
	x /= magnitude;
	y /= magnitude;
	z /= magnitude;
	magnitude = 1;
}

Ray::Ray() {
	start = Point();
	direction = Vector();
}

Ray::Ray(Point start, Vector direction) {
	this->start = start;
	this->direction = direction;
}

/* Constructor comes in handy during main pixel loop */
Ray::Ray(int i, int j, int width, int height, Camera *camera) {
	float us, vs, ws, right, left, bottom, top;
	this->start = Point(camera->center.x, camera->center.y, camera->center.z);
	
	right = camera->right.magnitude / 2.0;
	left = -1 * camera->right.magnitude / 2.0;
	bottom = -1 * camera->up.magnitude / 2.0;
	top = camera->up.magnitude / 2.0;

	us = left + (right - left) * (i + 0.5) / (float) width;
	vs = bottom + (top - bottom) * (j + 0.5) / (float) height;
	ws = -1;
	this->direction = Vector(us - camera->center.x, vs - camera->center.y, ws);
	this->direction.Normalize();
}




/*            		 *                  Colors			      *        	        */

/* Initialize Pigment to black */
Pigment::Pigment() {
	r = g = b = 0;
	f = 1;
}

Pigment::Pigment(float r, float g, float b, float f) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->f = f;
}

void Pigment::Reset() {
	r = g = b = 0;
	f = 1;
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




/*            		*                  Scene Setup				    *               */

Light::Light() {
	center = Point();
	pigment = Pigment();
}

Light::Light(Point center, Pigment pigment) {
	this->center = center;
	this->pigment = pigment;
}

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
	finish = Finish();
}

void Geometry::SetNormal(Vector *other) {
	normal.x = other->x;
	normal.y = other->y;
	normal.z = other->z;
}

void Geometry::SetPigment(Pigment *other) {
	pigment.r = other->r;
	pigment.g = other->g;
	pigment.b = other->b;
	pigment.f = other->f;
}

void Geometry::SetFinish(Finish *other) {
	finish.ambient = other->ambient;
	finish.diffuse = other->diffuse;
	finish.specular = other->specular;
	finish.roughness = other->roughness;
}

/* Virtual function, should not be called */
void Geometry::Print() {
	cout << "This is a Geometry object." << endl;
}

/* Virtual function, should not be called */
float Geometry::Intersect(Ray *ray, Camera *camera) {
	cout << "Geometry object intersect." << endl;
	return 10000;
}

Sphere::Sphere() {
	center = Point();
	radius = 0;
	normal = Vector();
	pigment = Pigment();
	finish = Finish();
}

Sphere::Sphere(Point center, float radius, Pigment pigment, Finish finish) {
	this->center = center;
	this->radius = radius;
	this->pigment = pigment;
	this->finish = finish;
}

void Sphere::Print() {
	cout << "sphere { ";
	cout << "<" << center.x << ", " << center.y << ", " << center.z << ">, " << radius << endl;
	cout << "  pigment { color <" << pigment.r << ", " << pigment.g << ", " << pigment.b << ", " << pigment.f << ">}" << endl;
	cout << "  finish {ambient " << finish.ambient << " diffuse " << finish.diffuse << "}" << endl;
	cout << "}" << endl;
}

 /* Return distance along ray to sphere */
float Sphere::Intersect(Ray *ray, Camera *camera) {
	float distance, t1, t2, rad;
	Vector difPC = Vector(camera->center.x - center.x, camera->center.y - center.y, camera->center.z - center.z);
	Vector difPCCopy = Vector(difPC.x, difPC.y, difPC.z);
	Vector dCopy = Vector(ray->direction.x, ray->direction.y, ray->direction.z);

	rad = pow(ray->direction.Dot(&difPC), 2) - (ray->direction.Dot(&dCopy) * (difPC.Dot(&difPCCopy)) - pow(radius, 2));

	/* If radicand is negative, return no hit */
	if (rad < 0)
		distance = -1;

	/* Return smallest (closest) positive distance */
	else if (rad == 0)
		distance = -1 * ray->direction.Dot(&difPC) / ray->direction.Dot(&dCopy);

	else {
		t1 = (-1 * ray->direction.Dot(&difPC) + sqrt(rad)) / ray->direction.Dot(&dCopy);
		t2 = (-1 * ray->direction.Dot(&difPC) - sqrt(rad)) / ray->direction.Dot(&dCopy);

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

Plane::Plane() {
	normal = Vector();
	distance = 0;
	pigment = Pigment();
	finish = Finish();
}

Plane::Plane(Vector normal, float distance, Pigment pigment, Finish finish) {
	this->normal = normal;
	this->distance = distance;
	this->pigment = pigment;
	this->finish = finish;
}

void Plane::Print() {
	cout << "plane {";
	cout << "<" << normal.x << ", " << normal.y << ", " << normal.z << ">, " << distance << endl;
	cout << "  pigment {color <" << pigment.r << ", " << pigment.g << ", " << pigment.b << ", " << pigment.f << ">}" << endl;
	cout << "  finish {ambient " << finish.ambient << " diffuse " << finish.diffuse << "}" << endl;
	cout << "}" << endl;
}

 /* Return distance along ray to plane */
float Plane::Intersect(Ray *ray, Camera *camera) {
	float distance;
	Point onPlane = Point(this->distance * normal.x, this->distance * normal.y, this->distance * normal.z);
	Vector difCameraPlane = Vector(onPlane.x - camera->center.x, onPlane.y - camera->center.y, onPlane.z - camera->center.z);

	/* If dot product is 0, return no hit */
	if (ray->direction.Dot(&normal) == 0)
		distance = -1;
	else
		distance = difCameraPlane.Dot(&normal) / ray->direction.Dot(&normal);
	
	return distance;
}





