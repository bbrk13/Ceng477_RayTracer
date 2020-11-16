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

Ray getCamRay(parser::Camera Cam, int y, int x) {
    parser::Vector3f direction, m, q, s;
    Ray resultRay;
    float su, sv;

    m.x = Cam.position.x;
    m.y = Cam.position.y;
    m.z = Cam.position.z - Cam.near_distance;

    // Cam.near_pane.x = image plane left
    // Cam.near_pane.y = image plane right
    // Cam.near_pane.z = image plane bottom
    // Cam.near_pane.w = image plane top

    q.x = m.x + Cam.near_plane.x;
    q.y = m.y + Cam.near_plane.y;
    q.z = m.z;

    su = ((Cam.near_plane.y - Cam.near_plane.x) / Cam.image_width) * (y + 0.5);
    sv = ((Cam.near_plane.w - Cam.near_plane.z) / Cam.image_height) * (x + 0.5);

    s.x = q.x + su;
    s.y = q.y - sv;
    s.z = q.z;

    direction.x = s.x - Cam.position.x;
    direction.y = s.y - Cam.position.y;
    direction.z = s.z - Cam.position.z;

    direction = normalize(direction);

    resultRay.origin.x = Cam.position.x;
    resultRay.origin.y = Cam.position.y;
    resultRay.origin.z = Cam.position.z;

    resultRay.direction.x = direction.x;
    resultRay.direction.y = direction.y;
    resultRay.direction.z = direction.z;

    return resultRay;

float determinant(parser::Vec3f a, parser::Vec3f b, parser::Vec3f c){
	float result =a.x*(b.y*c.z - c.y*b.z) + a.y*(c.x*b.z - c.z*b.x) + a.z*(b.x*c.y - b.y*c.x);
	return result;
}

CheckIntersectResult checkIntersect(Ray camRay, int y, int x) {
 // TODO checkIntersectTriangle
 // TODO checkIntersectSphere
 // TODO checkIntersectMesh

 // TODO Please add "t" result to return value of checkIntersect
 // TODO Please add "position" value to return object

 // TODO How do we pass objects to this function ?
}

RGB calculateLights(Ray camRay, CheckIntersectResult res, Parser::Material mat) {

    float tmins = 1;
    RGB resultContributionColor;
    resultContributionColor.x = 0;
    resultContributionColor.y = 0;
    resultContributionColor.z = 0;

    for (int index = 0; index < scene.point_lights.size() ; index++){

        Ray rayShadow;
        rayShadow.direction = scene.point_lights[index].position;
        rayShadow.origin = subtract(res.position, scene.point_lights[index].position);

        // for triangles
        for (int tri_index = 0; tri_index < scene.triangles.size(); tri_index++){
            parser::Triangle shadowTriangleObject = scene.triangles[tri_index];
            CheckIntersectResult interResult = checkIntersectTriangle(shadowTriangleObject, rayShadow);

            if (interResult.object_type != 0 && interResult.t > scene.shadow_ray_epsilon && interResult.t < tmins){
                tmins = interResult.t;
            }

        }

        // for spheres
        for (int sph_index = 0; sph_index < scene.spheres.size(); sph_index++){
            parser::Sphere shadowSphereObject = scene.spheres[sph_index];
            CheckIntersectResult interResult = checkIntersectSphere(shadowSphereObject, rayShadow);

            if (interResult.object_type != 0 && interResult.t > scene.shadow_ray_epsilon && interResult.t < tmins){
                tmins = interResult.t;
            }

        }

        // for meshes
        for (int mesh_index = 0; mesh_index < scene.meshes.size(); mesh_index++){
            parser::Mesh shadowMeshObject = scene.meshes[mesh_index];
            CheckIntersectResult interResult = checkIntersectMesh(shadowMeshObject, rayShadow);

            if (interResult.object_type != 0 && interResult.t > scene.shadow_ray_epsilon && interResult.t < tmins){
                tmins = interResult.t;
            }

        }

        if (tmins >= 0.9998){
            parser::Vec3f diffuseContribution;
            parser::Vec3f specularContribution;

            float cos1, cos2;

            diffuseContribution = calculateLightContribution(scene.point_lights[index], res.position);
            specularContribution = calculateLightContribution(scene.point_lights[index], res.position);

            parser::Vec3f wi, w0, h;

            wi = normalize(subtract(scene.point_lights[index].position, res.position));
            w0 = normalize(subtract(camRay.origin, res.position));
            h = normalize(add(wi, w0));

            cos1 = dot(wi, res.normal) / length(res.normal) * length(wi));
            cos2 = dot(h, res.normal) / length(res.normal) * length(h));

            if (cos1 < 0){
                res.normal.x *= -1;
                res.normal.y *= -1;
                res.normal.z *= -1;

                cos1 = dot(res.normal, wi) / dot(length(res.normal * length(h)));

                if (cos2 < 0){
                    cos2 = dot(res.normal, h) / (length(res.normal) * length(h));
                }
            }

            if (mat.phong_exponent > 100){
                cos2 = 1;
            }else {
                cos2 = pow(cos2, mat.phong_exponent);
            }


            if (cos1 >= 0){
                diffuseContribution.x *= cos1;
                diffuseContribution.y *= cos1;
                diffuseContribution.z *= cos1;
            }else{
                diffuseContribution.x = 0;
                diffuseContribution.y = 0;
                diffuseContribution.z = 0;
            }


            if (cos2 >= 0){
                specularContribution.x *= cos2;
                specularContribution.y *= cos2;
                specularContribution.z *= cos2;
            }else{
                specularContribution.x = 0;
                specularContribution.y = 0;
                specularContribution.z = 0;
            }

            resultContributionColor.x += mat.diffuse.x * diffuseContribution.x ;
            resultContributionColor.y += mat.diffuse.y * diffuseContribution.y ;
            resultContributionColor.z += mat.diffuse.z * diffuseContribution.z ;

            resultContributionColor.x += mat.specular.x * specularContribution.x ;
            resultContributionColor.y += mat.specular.y * specularContribution.y ;
            resultContributionColor.z += mat.specular.z * specularContribution.z ;

        }
        tmins = 1;



    }
     return resultContributionColor;
}

parser::Vector3f calculateLightContribution(parser::PointLight l, parser::Vec3f p){
    parser::Vec3f tmp;
    float distance = distance(p, l.position);
    tmp.x = l.intensity.x;
    tmp.y = l.intensity.y;
    tmp.z = l.intensity.z;
    return tmp;
}

RGB addAmbient() {

}

bool shouldCalculateMirror() {

}

RGB calculateMirror() {

}

RGB rayTracer(Ray &camRay, int count, int y, int x)
{
    RGB colors;
    CheckIntersectResult res = checkIntersect(camRay, y, x);
    if (res.object_type == 0)
    {
        colors.red = scene.background_color.x;
        colors.green = scene.background_color.y;
        colors.blue = scene.background_color.z;
        return colors;
    }

    colors = addAmbient();

    if (shouldCalculateMirror())
    {
        RGB mirrorColors = calculateMirror();
        colors.blue += mirrorColors.blue;
        colors.red += mirrorColors.red;
        colors.green += mirrorColors.green;
    }

    RGB lightColors = calculateLights(camRay, res);
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

                image[imageIndex++] = colors.red;
                image[imageIndex++] = colors.green;
                image[imageIndex++] = colors.blue;
            }
        }

        write_ppm(scene.cameras[camera_index].image_name.c_str(), image, scene.cameras[camera_index].image_width, scene.cameras[camera_index].image_height);
    }
}
