#include <iostream>
#include "parser.h"
#include "ppm.h"
#include <limits>
#include<cmath>
#define ABS(a) ((a) > 0 ? (a) : -1 * (a))

parser::Scene scene;

typedef struct Intersection
{
    parser::Material material;
    parser::Vec3f point;
    parser::Vec3f normal;
    bool exists;
    float t;
} Intersection;

typedef struct Ray
{
    parser::Vec3f origin;
    parser::Vec3f direction;
} Ray;

typedef struct RGB
{
    float red;
    float green;
    float blue;
} RGB;

float dot(parser::Vec3f a, parser::Vec3f b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

parser::Vec3f cross(parser::Vec3f a, parser::Vec3f b) {
	parser::Vec3f result;
	result.x = a.y*b.z - a.z*b.y;
	result.y = a.z*b.x - a.x*b.z;
	result.z = a.x*b.y - a.y*b.x;
	return result;
}

float length2(parser::Vec3f v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z);
}

float length(parser::Vec3f v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

parser::Vec3f normalize(parser::Vec3f v)
{
    parser::Vec3f tmp;
    float d;

    d = length(v);
    tmp.x = v.x / d;
    tmp.y = v.y / d;
    tmp.z = v.z / d;

    return tmp;
}

parser::Vec3f add(parser::Vec3f a, parser::Vec3f b)
{
    parser::Vec3f tmp;
    tmp.x = a.x + b.x;
    tmp.y = a.y + b.y;
    tmp.z = a.z + b.z;

    return tmp;
}

parser::Vec3f mult(parser::Vec3f a, float c)
{
    parser::Vec3f tmp;
    tmp.x = a.x * c;
    tmp.y = a.y * c;
    tmp.z = a.z * c;

    return tmp;
}

parser::Vec3f subtract(parser::Vec3f a, parser::Vec3f b) {
	parser::Vec3f result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

float distance(parser::Vec3f a, parser::Vec3f b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

int equal(parser::Vec3f a, parser::Vec3f b)
{
    double e = 0.000000001;

    //printf("%lf %lf %f ----",ABS((a.x-b.x)),ABS((a.y-b.y)),ABS((a.z-b.z)));
    if ((ABS((a.x - b.x)) < e) && (ABS((a.y - b.y)) < e) && (ABS((a.z - b.z)) < e))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

float determinant(parser::Vec3f a, parser::Vec3f b, parser::Vec3f c){
	float result =a.x*(b.y*c.z - c.y*b.z) + a.y*(c.x*b.z - c.z*b.x) + a.z*(b.x*c.y - b.y*c.x);
	return result;
}

Ray getCamRay(parser::Camera, int y, int x)
{
}

Intersection sphereIntersection(parser::Sphere sphere, Ray camRay)
{
    Intersection intersection;
    float t1, t2, det;
	parser::Vec3f center = scene.vertex_data[sphere.center_vertex_id-1];
	intersection.exists = false;
	det = pow(dot(camRay.direction, subtract(camRay.origin,center)), 2) - dot(camRay.direction,camRay.direction) * (dot(subtract(camRay.origin,center),subtract(camRay.origin,center)) - pow(sphere.radius, 2));
	if(det>0){
		intersection.exists = true;
		t1 = (-dot(camRay.direction,subtract(camRay.origin,center)) + sqrt(det))/dot(camRay.direction,camRay.direction);
		t2 = (-dot(camRay.direction,subtract(camRay.origin,center)) - sqrt(det))/dot(camRay.direction,camRay.direction);

		if(t1>0 && t2>0){
			if(t1>t2){
                intersection.point = add(camRay.origin, mult(camRay.direction, t2));
		        intersection.t=t2;
			}else{
                intersection.point = add(camRay.origin, mult(camRay.direction, t1));
		        intersection.t=t1;
			}
            intersection.normal = normalize(subtract(intersection.point, center));
		}
    }
    return intersection;
}

Intersection triangleIntersection(parser::Triangle triangle, Ray camRay)
{   
    Intersection intersection;

    parser::Vec3f a = scene.vertex_data[triangle.indices.v0_id-1];
    parser::Vec3f b = scene.vertex_data[triangle.indices.v1_id-1];
    parser::Vec3f c = scene.vertex_data[triangle.indices.v2_id-1];
    float t, beta, alpha, A;
    intersection.exists = false;

    A = determinant(subtract(a,b),subtract(a,c),camRay.direction);
    beta = determinant(subtract(a,camRay.origin),subtract(a,c),camRay.direction)/A;
	alpha = determinant(subtract(a,b),subtract(a,camRay.origin),camRay.direction)/A;
	t = determinant(subtract(a,b),subtract(a,c),subtract(a,camRay.origin))/A;
	if(beta>=0.0 && alpha>=0.0 && (beta+alpha)<=1.0 && t>0){
		intersection.exists = true;
        intersection.normal = normalize(cross(subtract(c, a), subtract(b, a)));
        intersection.t = t;
        intersection.point.x = camRay.origin.x + camRay.direction.x * t;
        intersection.point.y = camRay.origin.y + camRay.direction.y * t;
        intersection.point.z = camRay.origin.z + camRay.direction.z * t;
	}

    return intersection;
}

Intersection getIntersection(Ray camRay, int y, int x)
{
    Intersection currentIntersection;
    Intersection nearestIntersection;
    nearestIntersection.exists = false;
    nearestIntersection.t = std::numeric_limits<float>::infinity();
    for (int i = 0; i < scene.spheres.size(); i++)
    {
        currentIntersection = sphereIntersection(scene.spheres[i], camRay);
        if (currentIntersection.t < nearestIntersection.t)
        {
            nearestIntersection = currentIntersection;
        }
    }
    for (int i = 0; i < scene.triangles.size(); i++)
    {
        currentIntersection = triangleIntersection(scene.triangles[i], camRay);
        if (currentIntersection.t < nearestIntersection.t)
        {
            nearestIntersection = currentIntersection;
        }
    }
    for (int i = 0; i < scene.meshes.size(); i++)
    {
        for (int j = 0; j < scene.meshes[i].faces.size(); j++)
        {
            struct parser::Triangle triangle;
            triangle.material_id = scene.meshes[i].material_id;
            triangle.indices = scene.meshes[i].faces[j];
            currentIntersection = triangleIntersection(triangle, camRay);
            if (currentIntersection.t < nearestIntersection.t)
            {
                nearestIntersection = currentIntersection;
            }
        }
    }
    return nearestIntersection;
}

RGB calculateLights(Ray camRay, Intersection res)
{
    RGB rgb;
    rgb.red = 0;
    rgb.green = 0;
    rgb.blue = 0;
    return rgb;
}

RGB addAmbient(Intersection intersection) {
    RGB rgb;
    rgb.red = intersection.material.ambient.x * scene.ambient_light.x;
    rgb.green = intersection.material.ambient.y * scene.ambient_light.y;
    rgb.blue = intersection.material.ambient.z * scene.ambient_light.z;
    return rgb;
}

bool shouldCalculateMirror()
{
    return false;
}

RGB calculateMirror()
{
}

RGB rayTracer(Ray &camRay, int count, int y, int x)
{
    RGB colors;
    Intersection intersection = getIntersection(camRay, y, x);
    if (!intersection.exists)
    {
        colors.red = scene.background_color.x;
        colors.green = scene.background_color.y;
        colors.blue = scene.background_color.z;
        return colors;
    }

    colors = addAmbient(intersection);

    if (shouldCalculateMirror())
    {
        RGB mirrorColors = calculateMirror();
        colors.blue += mirrorColors.blue;
        colors.red += mirrorColors.red;
        colors.green += mirrorColors.green;
    }

    RGB lightColors = calculateLights(camRay, intersection);
    colors.blue += lightColors.blue;
    colors.red += lightColors.red;
    colors.green += lightColors.green;

    return colors;
}

int main(int argc, char *argv[])
{

    scene.loadFromXml(argv[1]);

    std::cout << "x = " << scene.background_color.x << std::endl;
    std::cout << "y = " << scene.background_color.y << std::endl;
    std::cout << "z = " << scene.background_color.z << std::endl;

    for (int camera_index = 0; camera_index < scene.cameras.size(); camera_index++)
    {
        int width = scene.cameras[camera_index].image_width;
        int height = scene.cameras[camera_index].image_height;

        unsigned char *image = new unsigned char[width * height * 3];

        int imageIndex = 0;
        // creting image by using background color from scene
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {

                Ray camRay = getCamRay(scene.cameras[camera_index], y, x);
                RGB colors = rayTracer(camRay, 1, y, x);

                image[imageIndex++] = colors.red > 255 ? 255 : int(colors.red);
                image[imageIndex++] = colors.green > 255 ? 255 : int(colors.green);
                image[imageIndex++] = colors.blue > 255 ? 255 : int(colors.blue);
            }
        }

        write_ppm(scene.cameras[camera_index].image_name.c_str(), image, scene.cameras[camera_index].image_width, scene.cameras[camera_index].image_height);
    }
}
