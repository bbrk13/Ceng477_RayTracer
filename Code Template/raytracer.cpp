#include <iostream>
#include "parser.h"
#include "ppm.h"

typedef unsigned char RGB[3];

float dot(Vec3f a, Vec3f b)
{
    return a.x*b.x+a.y*b.y+a.z*b.z;
}

float length2(Vec3f v)
{
    return (v.x*v.x+v.y*v.y+v.z*v.z);
}

float length(Vec3f v)
{
    return sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

Vec3f normalize(Vec3f v)
{
    Vec3f tmp;
    float d;

    d=length(v);
    tmp.x = v.x/d;
    tmp.y = v.y/d;
    tmp.z = v.z/d;

    return tmp;
}

Vec3f add(Vec3f a, Vec3f b)
{
    Vec3f tmp;
    tmp.x = a.x+b.x;
    tmp.y = a.y+b.y;
    tmp.z = a.z+b.z;

    return tmp;
}

Vec3f mult(Vec3f a, float c)
{
    Vec3f tmp;
    tmp.x = a.x*c;
    tmp.y = a.y*c;
    tmp.z = a.z*c;

    return tmp;
}

float distance(Vec3f a, Vec3f b)
{
    return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z));
}

int equal(Vec3f a, Vec3f b)
{
    double e = 0.000000001;

    //printf("%lf %lf %f ----",ABS((a.x-b.x)),ABS((a.y-b.y)),ABS((a.z-b.z)));
    if ((ABS((a.x-b.x))<e) && (ABS((a.y-b.y))<e) && (ABS((a.z-b.z))<e))
    { return 1;}
    else { return 0;}
}

int main(int argc, char* argv[])
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



    const RGB BAR_COLOR[8] =
    {
        { 255, 255, 255 },  // 100% White
        { 255, 255, 0 },  // Yellow
        {   0, 255, 255 },  // Cyan
        {   0, 255,   0 },  // Green
        { 255,   0, 255 },  // Magenta
        { 255,   0,   0 },  // Red
        {   0,   0, 255 },  // Blue
        {   0,   0,   0 },  // Black
    };
    for (int camera_index = 0; camera_index < scene.cameras.size(); camera_index++){
        int width = scene.cameras[camera_index].image_width;
        int height = scene.cameras[camera_index].image_height;


        unsigned char* image = new unsigned char [width * height * 3];



        int i = 0;
        // creting image by using background color from scene
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                image[i++] = scene.background_color.x;
                image[i++] = scene.background_color.y;
                image[i++] = scene.background_color.z;
            }
        }

        // TODO find intersections and change color of related pixels



        write_ppm(scene.cameras[camera_index].image_name.c_str(), image, scene.cameras[camera_index].image_width, scene.cameras[camera_index].image_height);
    }


}


