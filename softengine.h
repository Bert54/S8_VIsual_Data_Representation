#ifndef PROJET_SOFTENGINE_H
#define PROJET_SOFTENGINE_H

#include "geometry.h"

namespace SoftEngine {
    class Camera {

    public:
        Vec3f position;
        Vec3f target;
    };

    class Triangle {
    public:
        Vec3f vertices[3];

        Triangle();
        Triangle(Vec3f, Vec3f, Vec3f);
    };

    class Mesh {

    public:
        std::vector<Triangle> polygons;


    };

    class Device {


    public:

        std::vector<Vec3f> framebuffer;
        int width;
        int height;

        Device(int, int);
        void DrawPoint(Vec2f p, Vec3f color);
        void DrawLine(Vec2f p1, Vec2f p2, Vec3f color);
        void DrawTriangle(Vec2f p1, Vec2f p2, Vec2f p3, Vec3f color);
        void render(Camera camera, std::vector<Mesh> meshes);

    };

};


#endif
