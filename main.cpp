#include "geometry.h"
#include "matrix.h"
#include "softengine.h"

using namespace SoftEngine;

int main() {

    Mesh duck = Mesh("../duck.obj", 0);
    Mesh diablo = Mesh("../diablo3_pose.obj", 1);
    Mesh af_head = Mesh("../african_head.obj", 1);
    af_head.setRotation(270.f, 0.f);
    diablo.setRotation(270.f, 0.f);
    Camera camera = Camera();
    Device device(1024, 768);
    std::vector<Mesh> meshes;
    //meshes.push_back(duck);
    meshes.push_back(diablo);
    //meshes.push_back(af_head);
    camera.position = Vec3f(0.f, 0.f, 0.f);
    camera.target = Vec3f(0.f, 0.f, 1.f);
    device.render(camera, meshes, 90.f);
    return 0;
}
