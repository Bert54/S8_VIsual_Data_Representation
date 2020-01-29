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
        Vec3f color;

        Triangle();
        Triangle(Vec3f, Vec3f, Vec3f);
    };

    class Mesh {

    public:
        std::vector<Triangle> polygons;
        std::vector<Vec3f> verts;
        std::vector<Vec3i> faces_n;
        std::vector<std::vector<Vec3i> > faces;
        std::vector<Vec3f> norms;
        std::vector<Vec2f> uv;

        float rotX;
        float rotZ;
        float translationX;
        float translationY;
        float translationZ;

        Mesh();
        Mesh(const char *filename, int method);
        void setRotation(float rotationX, float rotationZ);
        void setTranslation(float trX, float trY, float trZ);
    };

    class Device {


    public:

        std::vector<Vec3f> framebuffer;
        int width;
        int height;

        Device(int, int);
        void DrawPoint(Vec2f p, Vec3f color, int side);
        void DrawLine(Vec2f p1, Vec2f p2, Vec3f color, int side);
        void ProcessScanLine(int y, Vec2f pa, Vec2f pb, Vec2f pc, Vec2f pd, Vec3f color, int side);
        void DrawTriangle(Vec2f p1, Vec2f p2, Vec2f p3, Vec3f color, int side);
        void FillTriangle(Vec2f p1, Vec2f p2, Vec2f p3, Vec3f color, int side);
        void render(Camera camera, std::vector<Mesh> meshes, float fov, int mode);
        void render_prep(Camera cameraInit, std::vector<Mesh> meshes, float fov);
    };

};

#endif

