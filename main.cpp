#include "geometry.h"
#include "matrix.h"
#include "softengine.h"

using namespace SoftEngine;

int main() {

    Mesh duck = Mesh("../duck.obj", 0);
    Mesh diablo = Mesh("../diablo3_pose.obj", 1);
    Mesh af_head = Mesh("../african_head.obj", 1);
    af_head.setRotation(0.f,135.6f, 0.f);
    af_head.setTranslation(-0.85f, 0, -0.25f);
    diablo.setRotation(0.f, 135.3f, 0.f);
    diablo.setTranslation(0.3f, 0, -0.78f);
    //duck.setTranslation(0, 0, 0.50f);
    Camera camera = Camera();
    Device device(1024, 768);
    std::vector<Mesh> meshes;
    //meshes.push_back(duck);
    meshes.push_back(diablo);
    meshes.push_back(af_head);
    camera.position = Vec3f(0.f, 0.f, -2.f);
    camera.target = Vec3f(0.f, 0.f, 1.f);
    device.render_prep(camera, meshes, 90.f);
    return 0;
}
