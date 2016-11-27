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

/* Point constructor, given a ray and distance away, returns point at destination */
Point::Point(Ray *ray, float distance) {
	x = ray->start.x + ray->direction.x * distance;
	y = ray->start.y + ray->direction.y * distance;
	z = ray->start.z + ray->direction.z * distance;
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

void Vector::operator*=(float scalar) {
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
	vectorU *= us;
	vectorV *= vs;
	vectorW *= ws;

	direction = Vector(vectorU.x + vectorV.x + vectorW.x, vectorU.y + vectorV.y + vectorW.y, vectorU.z + vectorV.z + vectorW.z);
	direction.Normalize();
}

Ray::Ray(Ray *initial, Point *surface, Vector *normal) {
	start = Point(surface->x, surface->y, surface->z);

	Vector negativeDir = Vector(-1 * initial->direction.x, -1 * initial->direction.y, -1 * initial->direction.z);
	direction.x = initial->direction.x + 2 * normal->Dot(&negativeDir) * normal->x;
	direction.y = initial->direction.y + 2 * normal->Dot(&negativeDir) * normal->y;
	direction.z = initial->direction.z + 2 * normal->Dot(&negativeDir) * normal->z;
	direction.SetMagnitude(direction.x, direction.y, direction.z);
}

/* Print Ray povray style */
void Ray::Print() {
	cout << "Ray: " << endl << "   ";
	start.Print();
	cout << "   ";
	direction.Print();
}

void Ray::PrintTest() {
	cout << "Ray: {" << start.x << ", " << start.y << ", " << start.z << "}";
	cout << " -> {" << direction.x << ", " << direction.y << ", " << direction.z << "}" << endl;
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

Pigment Pigment::operator*(float scalar) {
	Pigment result = Pigment(this->r * scalar, this->g * scalar, this->b * scalar);
	
	if (result.r > 255)
		result.r = 255;

	if (result.g > 255)
		result.g = 255;

	if (result.b > 255)
		result.b = 255;

	return result;
}

void Pigment::operator*=(float scalar) {
	r *= scalar;
	g *= scalar;
	b *= scalar;

	if (r > 255)
		r = 255;

	if (g > 255)
		g = 255;

	if (b > 255)
		b = 255;	
}

Pigment Pigment::operator+(Pigment other) {
	Pigment result = Pigment(r + other.r, g + other.g, b + other.b);

	if (result.r > 255)
		result.r = 255;

	if (result.g > 255)
		result.g = 255;

	if (result.b > 255)
		result.b = 255;

	return result;
}

void Pigment::operator+=(Pigment other) {
	r += other.r;
	g += other.g;
	b += other.b;

	if (r > 255)
		r = 255;

	if (g > 255)
		g = 255;

	if (b > 255)
		b = 255;
}

void Pigment::operator+=(Pigment *other) {
	r += other->r;
	g += other->g;
	b += other->b;
}

/* Fill color_t variable (with Image.cpp compatibility) from my own Pigment class */
void Pigment::SetColorT(color_t *color) {
	color->r = r * 255;
	color->g = g * 255;
	color->b = b * 255;
	color->f = f;

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
	cout << "Pigment {" << r << ", " << g << ", " << b << "}" << endl;
}

void Pigment::PrintBig() {
	cout << "Pigment {" << r * 255 << ", " << g * 255 << ", " << b * 255 << "}" << endl;
}

Finish::Finish() {
	ambient = diffuse = specular = reflect = refract = ior = 0; //refraction and ior set to 0 or 1?
	roughness = 1;
}

Finish::Finish(float ambient, float diffuse, float specular, float roughness, float reflect, float refract, float ior) {
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->roughness = roughness;
	this->reflect = reflect;
	this->refract = refract;
	this->ior = ior;
}

/* Print Finish in povray format */
void Finish::Print() {
	cout << "Finish {ambient " << ambient << " diffuse " << diffuse << " specular " << specular << " roughness " << roughness;
	cout <<  " reflect " << reflect << " refract " << refract << " ior " << ior << "}" << endl;
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
	cout << "color <" << pigment.r << ", " << pigment.g << ", " << pigment.b << ", " << pigment.f << ">}" << endl << endl;
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
	cout << "}" << endl << endl;
}





/*            		 *                  Geometry				    *                  */

Geometry::Geometry() {
	normal = Vector();
	pigment = Pigment();
	truePigment = Pigment();
	pigmentA = Pigment();
	pigmentD = Pigment();
	pigmentS = Pigment();
	finish = Finish();
}

/* Virtual function, should not be called */
void Geometry::Print() {
	cout << "Geometry {}" << endl;
}

void Geometry::PrintType() {
	cout << "Geometry" << endl;
}

/* Virtual function, should not be called */
float Geometry::Intersect(int i, int j, Ray *ray) {
	cout << "Geometry object intersect." << endl;
	return -1;
}

/* Set Point on Geometry itself, along initial Ray from camera */
void Geometry::SetOnGeom(Ray *ray, float rayDistance) {
	onGeom.x = ray->start.x + rayDistance * ray->direction.x;
	onGeom.y = ray->start.y + rayDistance * ray->direction.y;
	onGeom.z = ray->start.z + rayDistance * ray->direction.z;
}

void Geometry::SetNormal() {
	;
}

/* Virtual function, should not be called */
Pigment Geometry::BlinnPhong(int i, int j, Ray *ray, float rayDist) {
	cout << "Geometry object Blinn Phong." << endl;
	return Pigment(0, 0, 0);
}

/* Find Ambient Pigment for Blinn Phong */
void Geometry::BlinnPhongAmbient() {
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

	truePigment += &pigmentA;
}

/* Find Diffuse Pigment for Blinn Phong */
void Geometry::BlinnPhongDiffuse() {
	float zero = 0;
	Vector lightVector = Vector(light->center.x - onGeom.x, light->center.y - onGeom.y, light->center.z - onGeom.z);
	lightVector.Normalize();

	pigmentD.r = finish.diffuse * pigment.r * light->pigment.r * max(normal.Dot(&lightVector), zero);
	pigmentD.g = finish.diffuse * pigment.g * light->pigment.g * max(normal.Dot(&lightVector), zero);
	pigmentD.b = finish.diffuse * pigment.b * light->pigment.b * max(normal.Dot(&lightVector), zero);

	pigmentD *= 1 - finish.reflect;
	//pigmentD *= 1 - pigment.f;

	truePigment += pigmentD;
}

/* Find Specular Pigment for Blinn Phong */
void Geometry::BlinnPhongSpecular() {
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

	truePigment += &pigmentS;
}

/* Send Shadow Feeler ray from current geometry */
/* Return boolean that determines if another object blocks the light source from current object */
bool Geometry::ShadowFeeler(int i, int j) {
	float dist = 0;
	float lightDistance = onGeom.Distance(&light->center);

	Vector feelVector = Vector(light->center.x - onGeom.x, light->center.y - onGeom.y, light->center.z - onGeom.z);
	feelVector.Normalize();
	feeler = Ray(&onGeom, &feelVector);

	for (int geom = 0; geom < allGeometry->size(); geom++) {
		dist = allGeometry->at(geom)->Intersect(i, j, &feeler);

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

Pigment Geometry::Reflect(int i, int j, float rayDist, Ray ray, int bounce) {		
	BlinnPhong(i, j, &ray, rayDist); /* truePigment holds result of this->BlinnPhong */

	if (i == 320 && j == 145) {
		cout << "Ambient: " << to_string(pigmentA.r) << ", " << to_string(pigmentA.g) << ", " << to_string(pigmentA.b) << endl;
		cout << "Diffuse: " << to_string(pigmentD.r) << ", " << to_string(pigmentD.g) << ", " << to_string(pigmentD.b) << endl;
		cout << "Specular: " << to_string(pigmentS.r) << ", " << to_string(pigmentS.g) << ", " << to_string(pigmentS.b) << endl;
	}

	/* If we have hit max bounces, or what we've hit isn't reflective, return its color */
	// thumbs up
	if (bounce > 4 || !finish.reflect) {
		return truePigment;
	}

	else {
		/* Compute reflected ray */
		Ray reflectRay = Ray(&ray, &onGeom, &normal);

		if (i == 320 && j == 145) {
			cout << "----" << endl << "Iteration type: Reflection" << endl;
			reflectRay.PrintTest();
		}

		float newDistance, closest = 10000;
		Geometry *hitGeometry;
		
		/* From current geometry, send reflect ray towards other geometry */
		for (int g = 0; g < allGeometry->size(); g++) {
			newDistance = allGeometry->at(g)->Intersect(i, j, &reflectRay);
			if (newDistance > 0.001 && newDistance < closest) { //distance > 0.001 && distance < storage.distance
				closest = newDistance;
				hitGeometry = allGeometry->at(g);
			}
		}

		/* We didn't hit new geometry after reflecting, what to do? */
		if (closest == 10000)
			return (pigmentS + pigmentD) * (1 - finish.reflect) + pigmentA; //we add the pigment of current reflective object and ambient light

		if (i == 320 and j == 145) {
			cout << "1 - finish.reflect: " << 1 - finish.reflect << endl;
			truePigment.Print();
		}

		/* finalColor = 1.0*local_ambient + (1-reflect-filt)*(diffuse+specular) + reflect*(reflected_color)+filt*refracted_color; */

		/* Call Reflect from new hit Geometry */
		/* return this pigment offset by leftover reflective qualities + hit object pigment * reflective qualities */
		//     truePigment * (1 - finish.reflect)
		return pigmentA + (pigmentS + pigmentD) * (1 - finish.reflect) + hitGeometry->Reflect(i, j, closest, reflectRay, bounce + 1) * finish.reflect;
	}
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

Sphere::Sphere(Point *center, float radius, Pigment *pigment, Finish *finish) {
	this->center = Point(center->x, center->y, center->z);
	this->radius = radius;
	this->pigment = Pigment(pigment->r, pigment->g, pigment->b, pigment->f);
	this->finish = Finish(finish->ambient, finish->diffuse, finish->specular, finish->roughness, finish->reflect, finish->refract, finish->ior);
	pigmentA = Pigment();
	pigmentD = Pigment();
	pigmentS = Pigment();
	onGeom = Point();
}

/* Print sphere in povray format */
// float ambient, diffuse, specular, roughness, reflect, refract, ior;
void Sphere::Print() {
	cout << "sphere { ";
	cout << "<" << center.x << ", " << center.y << ", " << center.z << ">, " << radius << endl;
	cout << "  pigment { color <" << pigment.r << ", " << pigment.g << ", " << pigment.b << ", " << pigment.f << ">}" << endl;
	cout << "  finish {ambient " << finish.ambient << " diffuse " << finish.diffuse;
	if (finish.specular)
		cout << " specular " << finish.specular;
	if (finish.roughness)
		cout << " roughness " << finish.roughness;
	if (finish.reflect)
		cout << " reflect " << finish.reflect;
	if (finish.refract)
		cout << " refraction " << finish.refract;
	if (finish.ior)
		cout << " ior " << finish.ior;
	cout << "}" << endl;
	cout << "  onGeom <" << onGeom.x << ", " << onGeom.y << ", " << onGeom.z << ">" << endl;
	cout << "}" << endl;
}

void Sphere::PrintType() {
	cout << "Sphere" << endl;
}

 /* Return distance from point along ray to sphere */
float Sphere::Intersect(int i, int j, Ray *ray) {
	float distance, t1, t2, rad;

	Vector rayD = Vector(ray->direction.x, ray->direction.y, ray->direction.z);
	Vector difPC = Vector(ray->start.x - center.x, ray->start.y - center.y, ray->start.z - center.z);

	rad = pow(rayD.Dot(&difPC), 2) - rayD.Dot(&rayD) * ((difPC.Dot(&difPC)) - pow(radius, 2));

	/* If radicand is negative, return no hit */
	if (rad < 0)
		distance = -1;

	/* Return smallest (closest) positive distance */
	else if (rad == 0)
		distance = -1 * rayD.Dot(&difPC) / rayD.Dot(&rayD);

	else {
		t1 = (-1 * rayD.Dot(&difPC) + sqrt(rad)) / rayD.Dot(&rayD);
		t2 = (-1 * rayD.Dot(&difPC) - sqrt(rad)) / rayD.Dot(&rayD);

		if (t1 > 0 && t2 > 0)
			distance = t1 < t2 ? t1 : t2;
		else if (t1 > 0 && t2 < 0)
			distance = t1;
		else if (t1 < 0 && t2 > 0)
			distance = t2;
		else
			distance = -1;
	}

	return distance;
}

void Sphere::SetNormal() {
	normal = Vector((onGeom.x - center.x)/radius, (onGeom.y - center.y)/radius, (onGeom.z - center.z)/radius);
	normal.Normalize();
}

/* Blinn Phong BRDF for Sphere object */
Pigment Sphere::BlinnPhong(int i, int j, Ray *ray, float rayDist) {
	truePigment = Pigment(0, 0, 0);
	SetOnGeom(ray, rayDist);
	SetNormal();
	BlinnPhongAmbient();
	bool noShadow = ShadowFeeler(i, j);

	/* If current point on sphere is not in shadow, add Diffuse and Specular Pigments */
	if (noShadow) {
		BlinnPhongDiffuse();
		BlinnPhongSpecular();
	}

	return truePigment;
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

Plane::Plane(Vector *normal, float distance, Pigment *pigment, Finish *finish) {
	this->normal = Vector(normal->x, normal->y, normal->z);
	this->distance = distance;
	this->pigment = Pigment(pigment->r, pigment->g, pigment->b, pigment->f);
	this->finish = Finish(finish->ambient, finish->diffuse, finish->specular, finish->roughness, finish->reflect, finish->refract, finish->ior);
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
	cout << "  finish {ambient " << finish.ambient << " diffuse " << finish.diffuse;
	if (finish.specular)
		cout << " specular " << finish.specular;
	if (finish.roughness)
		cout << " roughness " << finish.roughness;
	if (finish.reflect)
		cout << " reflect " << finish.reflect;
	if (finish.refract)
		cout << " refraction " << finish.refract;
	if (finish.ior)
		cout << " ior " << finish.ior;
	cout << "}" << endl;
	cout << "}" << endl;
}

void Plane::PrintType() {
	cout << "Plane" << endl;
}

/* Initialize point on plane according to povray info */
/* Useful when parsing, otherwise use Geometry::SetOnGeom */
void Plane::SetOnGeom() {
	onGeom.x = distance * normal.x;
	onGeom.y = distance * normal.y;
	onGeom.z = distance * normal.z;
}

/* Return distance from point along ray to plane */
float Plane::Intersect(int i, int j, Ray *ray) {
	float distance;
	Vector rayD = Vector(ray->direction.x, ray->direction.y, ray->direction.z);
	//Vector difObjectPlane = Vector(onGeom.x - point->x, onGeom.y - point->y, onGeom.z - point->z);
	Vector difObjectPlane = Vector(onGeom.x - ray->start.x, onGeom.y - ray->start.y, onGeom.z - ray->start.z);

	/* If dot product is 0, return no hit */
	if (rayD.Dot(&normal) == 0)
		distance = -1;
	else
		distance = difObjectPlane.Dot(&normal)/rayD.Dot(&normal);
	
	return distance;
}

/* Blinn Phong BRDF for plane object */
Pigment Plane::BlinnPhong(int i, int j, Ray *ray, float rayDist) {
	Geometry::SetOnGeom(ray, rayDist);
	truePigment = Pigment(0, 0, 0);
	BlinnPhongAmbient();

	bool noShadow = ShadowFeeler(i, j);

	/* If current point on plane is not in shadow, add Diffuse Pigment */
	if (noShadow) {
		BlinnPhongDiffuse();
	}

	return truePigment;
}

Triangle::Triangle() {
	vertexA = Point();
	vertexB = Point();
	vertexC = Point();
	AB = Vector();
	AC = Vector();
	normal = Vector();
	onGeom = Point();
	pigment = Pigment();
	pigmentA = Pigment();
	pigmentD = Pigment();
	pigmentS = Pigment();
	finish = Finish();
}

Triangle::Triangle(Point *vertexA, Point *vertexB, Point *vertexC) {
	this->vertexA = Point(vertexA->x, vertexA->y, vertexA->z);
	this->vertexB = Point(vertexB->x, vertexB->y, vertexB->z);
	this->vertexC = Point(vertexC->x, vertexC->y, vertexC->z);
	SetVectors();
	normal = Vector();
	AB.Cross(&AC, &normal);
	normal.Normalize();

	onGeom = Point();
	pigment = Pigment();
	pigmentA = Pigment();
	pigmentD = Pigment();
	pigmentS = Pigment();
	finish = Finish();	
}

void Triangle::Print() {
	cout << "triangle {" << endl << "   ";
	vertexA.Print();
	cout << "   ";
	vertexB.Print();
	cout << "   ";
	vertexC.Print();
	cout << "   ";
	pigment.Print();
	cout << "   ";
	finish.Print();
	cout << "}" << endl << endl;
}

void Triangle::PrintType() {
	cout << "Triangle" << endl;
}

void Triangle::SetVectors() {
	AB = Vector(vertexA.x - vertexB.x, vertexA.y - vertexB.y, vertexA.z - vertexB.z);
	AC = Vector(vertexA.x - vertexC.x, vertexA.y - vertexC.y, vertexA.z - vertexC.z);
}

void Triangle::SetNormal(Ray *ray) {
	AB.Cross(&AC, &normal);
	normal.Normalize();

	if (ray->direction.Dot(&normal) > 0)
		normal *= -1;
}

float Triangle::Intersect(int i, int j, Ray *ray) {
	float distance, t, gamma, beta;
	float M, a, b, c, d, e, f, g, h, I, J, k, l;

	a = vertexA.x - vertexB.x;
	b = vertexA.y - vertexB.y;
	c = vertexA.z - vertexB.z;
	d = vertexA.x - vertexC.x;
	e = vertexA.y - vertexC.y;
	f = vertexA.z - vertexC.z;
	g = ray->direction.x;
	h = ray->direction.y;
	I = ray->direction.z;
	J = vertexA.x - ray->start.x;
	k = vertexA.y - ray->start.y;
	l = vertexA.z - ray->start.z;

	M = a*(e*I - h*f) + b*(g*f - d*I) + c*(d*h - e*g);

	t = -1 * (f*(a*k - J*b) + e*(J*c - a*l) + d*(b*l - k*c))/M;

	if (t <= 0.001)
		return -1;

	gamma = (I*(a*k - J*b) + h*(J*c - a*l) + g*(b*l - k*c))/M;
	if (gamma >= 1 || gamma <= 0)
		return -1;

	beta = (J*(e*I - h*f) + k*(g*f - d*I) + l*(d*h - e*g))/M;
	if (beta >= 1 || beta <= 0)
		return -1;

	if (beta > 0 && gamma > 0 && beta + gamma < 1)
		return t;
	else
		return -1;
}

Pigment Triangle::BlinnPhong(int i, int j, Ray *ray, float rayDist) {
	truePigment = Pigment(0, 0, 0);
	SetOnGeom(ray, rayDist);
	SetNormal(ray);
	BlinnPhongAmbient();

	bool noShadow = ShadowFeeler(i, j);

	/* If current point on triangle is not in shadow, add Diffuse Pigment */
	if (noShadow) {
		BlinnPhongDiffuse();
	}

	return truePigment;
}




/* 				   *				Storage						*				  */
Storage::Storage() {
	distance = 0;
	pigment = Pigment();
}

Storage::Storage(float distance, Pigment *pigment) {
	this->distance = distance;
	this->pigment = Pigment(pigment->r, pigment->g, pigment->b, pigment->f);
}

Storage::Storage(float distance, color_t *color) {
	this->distance = distance;
	pigment = Pigment(color->r, color->g, color->b, color->f);
}



