#pragma once
#include "Image.h"
#include <vector>
using namespace std;

class Point {
public:
	Point();
	Point(float x, float y, float z);
	Point(class Ray *ray, float distance);
	float Distance(Point *other);
	void Print();
	float x, y, z;
};

class Vector {
public:
	Vector();
	Vector(float x, float y, float z);
	void operator*=(float scalar);
	float Dot(Vector *other);
	void Cross(Vector *other, Vector *result);
	void SetMagnitude(float x, float y, float z);
	void Normalize();
	void Print();
	float x, y, z, magnitude;
};

class Ray {
public:
	Ray();
	Ray(Point *start, Vector *direction);
	Ray(int i, int j, int width, int height, class Camera *camera);
	Ray(Ray *initial, Point *intersect, Vector *normal);
	void Print();
	void PrintTest();
	Point start;
	Vector direction;
};

/* Used for rgb or rgbf colors */
class Pigment {
public:
	Pigment();
	Pigment(float r, float g, float b);
	Pigment(float r, float g, float b, float f);
	Pigment operator+(Pigment other);
	void operator+=(Pigment other);
	void operator+=(Pigment *other);
	Pigment operator*(float scalar);
	void operator*=(float scalar);
	void Print();
	void PrintBig();
	void SetColorT(color_t *color);
	float r, g, b, f;
};

/* Contains BRDF details */
class Finish {
public:
	Finish();
	Finish(float ambient, float diffuse, float specular, float roughness, float reflect, float refract, float ior);
	void Print();
	float ambient, diffuse, specular, roughness, reflect, refract, ior;
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
	virtual void PrintType();
	virtual float Intersect(int i, int j, Ray *ray);
	virtual Pigment BlinnPhong(int i, int j, Ray *ray, float rayDist);
	virtual void SetNormal();
	void BlinnPhongAmbient();
	void BlinnPhongDiffuse();
	void BlinnPhongSpecular();
	void SetOnGeom(Ray *ray, float rayDistance);
	bool ShadowFeeler(int i, int j);
	void ResetPigments();
	Pigment Reflect(int i, int j, float rayDist, Ray ray, int bounce); /* watch out for pointers pointing to garbage later */
	Pigment pigment; /* stores object color */
	Pigment truePigment; /* stores full object color after BlinnPhong */
	Finish finish; /* stores finish informatino */
	Vector normal;
	Point onGeom; /* stores Point on geometry itself */
	
	Pigment pigmentA, pigmentD, pigmentS; /* stores Ambient, Diffuse, and Specular pigments during Blinn Phong */
	Ray feeler; /* stores shadow feeler for Blinn Phong */
	
	Camera *camera;
	Light *light;
	vector<Geometry *> *allGeometry;
};

/* Child of Geometry, contains center Point and radius value */
class Sphere : public Geometry {
public:
	Sphere();
	Sphere(Point *center, float radius, Pigment *pigment, Finish *finish);
	void Print();
	void PrintType();
	float Intersect(int i, int j, Ray *ray);
	Pigment BlinnPhong(int i, int j, Ray *ray, float rayDist);
	void SetNormal();
	Point center;
	float radius;
};

/* Child of Geometry, inherits normal Vector and contains distance along normal Vecor */
class Plane : public Geometry {
public:
	Plane();
	Plane(Vector *normal, float distance, Pigment *pigment, Finish *finish);
	void Print();
	void PrintType();
	float Intersect(int i, int j, Ray *ray);
	Pigment BlinnPhong(int i, int j, Ray *ray, float rayDist);
	void SetOnGeom();
	float distance; /* Distance along normal defines plane location */
};

/* Child of Geometry, inherits normal vector, contains three defining vertices */
class Triangle : public Geometry {
public:
	Triangle();
	Triangle(Point *vertexA, Point *vertexB, Point *vertexC);
	void SetVectors();
	void SetNormal(Ray *ray);
	void Print();
	void PrintType();
	float Intersect(int i, int j, Ray *ray);
	Pigment BlinnPhong(int i, int j, Ray *ray, float rayDist);
	Point vertexA, vertexB, vertexC;
	Vector AB, AC; /* helpful when setting normal Vector */
};

/* Stores distance to point and color at pixel */
class Storage {
public:
	Storage();
	Storage(float distance, Pigment *pigment);
	Storage(float distance, color_t *color);
	float distance;
	Pigment pigment;
};




