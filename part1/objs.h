#pragma once
#include <vector>

class Point {
public:
	Point();
	Point(float x, float y, float z);
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
	float x, y, z, magnitude;
};

class Ray {
public:
	Ray();
	Ray(Point start, Vector direction);
	Ray(int i, int j, int width, int height, class Camera *camera);
	Point start;
	Vector direction;
};

/* Used for rgb or rgbf colors */
class Pigment {
public:
	Pigment();
	Pigment(float r, float g, float b, float f);
	void Reset();
	float r, g, b, f;
};

/* Contains BRDF details */
class Finish {
public:
	Finish();
	Finish(float ambient, float diffuse, float specular, float roughness);
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
/* Passes to children normal Vector, Pigment value, and Finish value */
/* Passes to children virtual classes to Print values and Intersect object with Ray */
class Geometry {
public:
	Geometry();
	void SetNormal(Vector *normal);
	void SetPigment(Pigment *pigment);
	void SetFinish(Finish *finish);
	virtual void Print();
	virtual float Intersect(Ray *ray, Camera *camera);
	Vector normal;
	Pigment pigment;
	Finish finish;
};

/* Child of Geometry, contains center Point and radius value */
class Sphere : public Geometry {
public:
	Sphere();
	Sphere(Point center, float radius, Pigment pigment, Finish finish);
	void Print();
	float Intersect(Ray *ray, Camera *camera);
	Point center;
	float radius;
};

/* Child of Geometry, inherits normal Vector and contains distance along normal Vecor */
class Plane : public Geometry {
public:
	Plane();
	Plane(Vector normal, float distance, Pigment pigment, Finish finish);
	void Print();
	float Intersect(Ray *ray, Camera *camera);
	float distance;
};