#include <iostream>
#include "parser.h"
#include "ppm.h"
#define ABS(a) ((a) > 0 ? (a) : -1 * (a))

parser::Scene scene;

struct CheckIntersectResult
{
    int object_type; // 0: no intersect, 1: triangle, 2: sphere, 3: mesh
    int i;
};

struct Ray
{
    parser::Vec3f origin;
    parser::Vec3f direction;
};

struct RGB
{
    float red;
    float green;
    float blue;
};

float dot(parser::Vec3f a, parser::Vec3f b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
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

}

CheckIntersectResult checkIntersect(Ray camRay, int y, int x) {

}

RGB calculateLights(Ray camRay, CheckIntersectResult res) {

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
