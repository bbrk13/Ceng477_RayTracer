#include <iostream>
#include "parser.h"
#include "ppm.h"
#define ABS(a) ((a) > 0 ? (a) : -1 * (a))

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

struct RGB {
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

Ray getCamRay(parser::Camera, int y, int x) {

}

RGB rayTracer(Ray &ray, int count, int y, int x) {
    
}

int main(int argc, char *argv[])
{
    // Sample usage for reading an XML scene file
    parser::Scene scene;

    scene.loadFromXml(argv[1]);

    std::cout << "x = " << scene.background_color.x << std::endl;
    std::cout << "y = " << scene.background_color.y << std::endl;
    std::cout << "z = " << scene.background_color.z << std::endl;

    // The code below creates a test pattern and writes
    // it to a PPM file to demonstrate the usage of the
    // ppm_write function.
    //
    // Normally, you would be running your ray tracing
    // code here to produce the desired image.
    /*
    const RGB BAR_COLOR[8] =
        {
            {255, 255, 255}, // 100% White
            {255, 255, 0},   // Yellow
            {0, 255, 255},   // Cyan
            {0, 255, 0},     // Green
            {255, 0, 255},   // Magenta
            {255, 0, 0},     // Red
            {0, 0, 255},     // Blue
            {0, 0, 0},       // Black
        };
    */
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

        // TODO find intersections and change color of related pixels

        //TODO ambient light adding

        //TODO for each light : light position light intensity adding

        //TODO mirror light

        //TODO Defuse

        write_ppm(scene.cameras[camera_index].image_name.c_str(), image, scene.cameras[camera_index].image_width, scene.cameras[camera_index].image_height);
    }
}
