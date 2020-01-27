#include "geometry.h"
#include "matrix.h"
#include "softengine.h"

using namespace SoftEngine;

int main() {

    Mesh mesh{};
    Camera camera = Camera();
    Device device(1024, 768);
    mesh.polygons.push_back(Triangle(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 1.0f, 0.0f), Vec3f(1.0f, 1.0f, 0.0f)));
    mesh.polygons.push_back(Triangle(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(1.0f, 1.0f, 0.0f), Vec3f(1.0f, 0.0f, 0.0f)));
    mesh.polygons.push_back(Triangle(Vec3f(1.0f, 0.0f, 0.0f), Vec3f(1.0f, 1.0f, 0.0f), Vec3f(1.0f, 1.0f, 1.0f)));
    mesh.polygons.push_back(Triangle(Vec3f(1.0f, 0.0f, 0.0f), Vec3f(1.0f, 1.0f, 1.0f), Vec3f(1.0f, 0.0f, 1.0f)));
    mesh.polygons.push_back(Triangle(Vec3f(1.0f, 0.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f)));
    mesh.polygons.push_back(Triangle(Vec3f(1.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(0.0f, 0.0f, 1.0f)));
    mesh.polygons.push_back(Triangle(Vec3f(0.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(0.0f, 1.0f, 0.0f)));
    mesh.polygons.push_back(Triangle(Vec3f(0.0f, 0.0f, 1.0f), Vec3f(0.0f, 1.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f)));
    mesh.polygons.push_back(Triangle(Vec3f(0.0f, 1.0f, 0.0f), Vec3f(0.0f, 1.0f, 1.0f), Vec3f(1.0f, 1.0f, 1.0f)));
    mesh.polygons.push_back(Triangle(Vec3f(0.0f, 1.0f, 0.0f), Vec3f(1.0f, 1.0f, 1.0f), Vec3f(1.0f, 1.0f, 0.0f)));
    mesh.polygons.push_back(Triangle(Vec3f(1.0f, 0.0f, 1.0f), Vec3f(0.0f, 0.0f, 1.0f), Vec3f(0.0f, 0.0f, 0.0f)));
    mesh.polygons.push_back(Triangle(Vec3f(1.0f, 0.0f, 1.0f), Vec3f(0.0f, 0.0f, 0.0f), Vec3f(1.0f, 0.0f, 0.0f)));
    std::vector<Mesh> meshes;
    meshes.push_back(mesh);
    camera.position = Vec3f(3.f, 3.f, 3.f);
    camera.target = Vec3f(0.01f, 0.01f, 0.01f);
    device.render(camera, meshes);
    return 0;
}
