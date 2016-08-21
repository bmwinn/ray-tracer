#pragma once
#include "Image.h"
#include <vector>
using namespace std;

class Point {
public:
	Point();
	Point(float x, float y, float z);
	float Distance(Point *other);
	void Print();
	float x, y, z;
};

class Vector {
public:
	Vector();
	Vector(float x, float y, float z);
	float Dot(Vector *other);
	void Cross(Vector *other, Vector *result);
	void SetMagnitude(float x, float y, float z);
	void Normalize();
	void Print();
	void Scale(float scalar);
	float x, y, z, magnitude;
};

class Ray {
public:
	Ray();
	Ray(Point *start, Vector *direction);
	Ray(int i, int j, int width, int height, class Camera *camera);
	void Print();
	Point start;
	Vector direction;
};

/* Used for rgb or rgbf colors */
class Pigment {
public:
	Pigment();
	Pigment(float r, float g, float b);
	Pigment(float r, float g, float b, float f);
	void Add(Pigment *other);
	void Print();
	void SetColorT(color_t *color);
	float r, g, b, f;
};

/* Contains BRDF details */
class Finish {
public:
	Finish();
	Finish(float ambient, float diffuse, float specular, float roughness);
	void Print();
	float ambient, diffuse, specular, roughness;
};

/* Contains Light location "center" and Pigment value */
class Light {
public:
	Light();
	Light(Point center, Pigment pigment);
	void Print();
	Point center;
	Pigment pigment;
};

/* Contains Camera location "center", up Vector, right Vector, and lookat Point */
class Camera {
public:
	Camera();
	Camera(Point center, Vector up, Vector right, Point lookat);
	void Print();
	Point center, lookat;
	Vector up, right;
};

/* Parent class to all Geometric objects */
class Geometry {
public:
	Geometry();
	virtual void Print();
	virtual float Intersect(Ray *ray, Point *point);
	virtual void BlinnPhong(int g, Ray *ray, float rayDistance, Pigment *pixelPigment, Light *light, Camera *camera, vector<Geometry *> *allGeometry);
	void BlinnPhongAmbient(Pigment *pixelPigment, Light *light);
	void BlinnPhongDiffuse(Pigment *pixelPigment, Light *light);
	void BlinnPhongSpecular(Pigment *pixelPigment, Light *light, Camera *camera);
	void SetOnGeom(Ray *ray, float rayDistance);
	bool ShadowFeeler(int g, Light *light, vector<Geometry *> *allGeometry);
	void ResetPigments();
	Pigment pigment; /* stores object color */
	Finish finish; /* stores finish informatino */
	Vector normal;
	Point onGeom; /* stores Point on geometry itself */
	Pigment pigmentA, pigmentD, pigmentS; /* stores Ambient, Diffuse, and Specular pigments during Blinn Phong */
	Ray feeler; /* stores shadow feeler for Blinn Phong */
};

/* Child of Geometry, contains center Point and radius value */
class Sphere : public Geometry {
public:
	Sphere();
	Sphere(Point center, float radius, Pigment pigment, Finish finish);
	void Print();
	float Intersect(Ray *ray, Point *point);
	void BlinnPhong(int g, Ray *ray, float rayDistance, Pigment *pixelPigment, Light *light, Camera *camera, vector<Geometry *> *allGeometry);
	Point center;
	float radius;
};

/* Child of Geometry, inherits normal Vector and contains distance along normal Vecor */
class Plane : public Geometry {
public:
	Plane();
	Plane(Vector normal, float distance, Pigment pigment, Finish finish);
	void Print();
	float Intersect(Ray *ray, Point *point);
	void BlinnPhong(int g, Ray *ray, float rayDistance, Pigment *pixelPigment, Light *light, Camera *camera, vector<Geometry *> *allGeometry);
	void SetOnGeom();
	float distance; /* Distance along normal defines plane location */
};

