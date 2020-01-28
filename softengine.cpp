#include <fstream>
#include <utility>
#include <iterator>
#include <cstring>

#include "softengine.h"
#include "matrix.h"

using namespace SoftEngine;

Device::Device(int width, int height) {
    this->width = width;
    this->height = height;
    framebuffer = std::vector<Vec3f>(width * height);
}

Triangle::Triangle(Vec3f v1, Vec3f v2, Vec3f v3) {
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;
}

Triangle::Triangle() {}

Mesh::Mesh() {
    rotX = 0.0f;
    rotZ = 0.0f;
}

Mesh::Mesh(const char *filename, int method) {
    if (method == 0) {
        std::ifstream in;
        in.open (filename, std::ifstream::in);
        if (in.fail()) {
            std::cerr << "Failed to open " << filename << std::endl;
            return;
        }
        std::string line;
        while (!in.eof()) {
            std::getline(in, line);
            std::istringstream iss(line.c_str());
            char trash;
            if (!line.compare(0, 2, "v ")) {
                iss >> trash;
                Vec3f v;
                for (int i=0;i<3;i++) iss >> v[i];
                verts.push_back(v);
            } else if (!line.compare(0, 2, "f ")) {
                Vec3i f;
                int idx, cnt=0;
                iss >> trash;
                while (iss >> idx) {
                    idx--; // in wavefront obj all indices start at 1, not zero
                    f[cnt++] = idx;
                }
                if (3==cnt) faces_n.push_back(f);
            }
        }
        for (Vec3i face: faces_n) {
            Triangle t = Triangle();
            t.vertices[0] = verts.at(face.x);
            t.vertices[1] = verts.at(face.y);
            t.vertices[2] = verts.at(face.z);
            polygons.push_back(t);
        }
    }
    else if (method == 1) {
        std::ifstream in;
        in.open(filename, std::ifstream::in);
        if (in.fail()) return;
        std::string line;
        while (!in.eof()) {
            std::getline(in, line);
            std::istringstream iss(line.c_str());
            char trash;
            if (!line.compare(0, 2, "v ")) {
                iss >> trash;
                Vec3f v;
                for (int i = 0; i < 3; i++) iss >> v[i];
                verts.push_back(v);
            } else if (!line.compare(0, 3, "vn ")) {
                iss >> trash >> trash;
                Vec3f n;
                for (int i = 0; i < 3; i++) iss >> n[i];
                norms.push_back(n);
            } else if (!line.compare(0, 3, "vt ")) {
                iss >> trash >> trash;
                Vec2f uvn;
                for (int i = 0; i < 2; i++) iss >> uvn[i];
                uv.push_back(uvn);
            }  else if (!line.compare(0, 2, "f ")) {
                std::vector<Vec3i> f;
                Vec3i tmp;
                iss >> trash;
                while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                    for (int i=0; i<3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
                    f.push_back(tmp);
                }
                faces.push_back(f);
            }
        }
        for (auto faceList: faces)
        {
            Triangle t = Triangle();
            int i = 0;
            Vec3i p1;
            Vec3i p2;
            Vec3i p3;
            for (auto face: faceList) {
                if (i == 0) {
                    p1 = face;
                }
                else if (i == 1) {
                    p2 = face;
                }
                else {
                    p3 = face;
                }
                i++;
            }
            t.vertices[0] = verts.at(p1.x);
            t.vertices[1] = verts.at(p2.x);
            t.vertices[2] = verts.at(p3.x);
            polygons.push_back(t);
        }
    }
    rotX = 0.0f;
    rotZ = 0.0f;
}

void Mesh::setRotation(float rotationX, float rotationZ) {
    rotX = rotationX;
    rotZ = rotationZ;
}

Vec3f scalar_product_vectors_3f(Vec3f lhs, Vec3f rhs) {
    Vec3f result = Vec3f();
    result.x = lhs.x * rhs.x;
    result.y = lhs.y * rhs.y;
    result.z = lhs.z * rhs.z;
    return result;
}

Vec3f normalize_vector_3f(Vec3f vector) {
    float length = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
    Vec3f normalized_vector = Vec3f();
    if (length != 0) {
        normalized_vector.x = vector.x / length;
        normalized_vector.y = vector.y / length;
        normalized_vector.z = vector.z / length;
    }
    else {
        normalized_vector.x = 0;
        normalized_vector.y = 0;
        normalized_vector.z = 0;
    }
    return normalized_vector;
}

Vec3f MultiplyMatrixVector(Vec3f v, Matrix m)
{
    Vec3f out = Vec3f();
    out.x = v.x * m(0,0) + v.y * m(1,0) + v.z * m(2,0) + m(3,0);
    out.y = v.x * m(0,1) + v.y * m(1,1) + v.z * m(2,1) + m(3,1);
    out.z = v.x * m(0,2) + v.y * m(1,2) + v.z * m(2,2) + m(3,2);
    double w = v.x * m(0,3) + v.y * m(1,3) + v.z * m(2,3) + m(3,3);

    if (w != 0.0f)
    {
        out.x /= w; out.y /= w; out.z /= w;
    }
    return out;
}

void Device::DrawPoint(Vec2f p, Vec3f color) {
    if (p.x >= 0 && p.x < width && p.y >= 0 && p.y < height) {
        int index = ((int) p.x + (int) p.y * width);
        if (index < width * height) {
            framebuffer[index].x = color.x;
            framebuffer[index].y = color.y;
            framebuffer[index].z = color.z;
        }
    }
}

void Device::DrawLine(Vec2f p1, Vec2f p2, Vec3f color) {
    const bool steep = (fabs(p2.y - p1.y) > fabs(p2.x - p1.x));
    if(steep)
    {
        std::swap(p1.x, p1.y);
        std::swap(p2.x, p2.y);
    }

    if(p1.x > p2.x)
    {
        std::swap(p1.x, p2.x);
        std::swap(p1.y, p2.y);
    }

    const float dx = p2.x - p1.x;
    const float dy = fabs(p2.y - p1.y);
    float error = dx / 2.0f;
    const int ystep = (p1.y < p2.y) ? 1 : -1;
    int y = (int)p1.y;

    const int maxX = (int)p2.x;

    for(int x=(int)p1.x; x<=maxX; x++)
    {
        if(steep)
        {
            DrawPoint(Vec2f(y, x), color);
        }
        else
        {
            DrawPoint(Vec2f(x, y), color);
        }

        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
}

void Device::DrawTriangle(Vec2f p1, Vec2f p2, Vec2f p3, Vec3f color) {
    DrawLine(p1, p2, color);
    DrawLine(p2, p3, color);
    DrawLine(p1, p3, color);
}

int orient2d(const Vec2f& a, const Vec2f& b, const Vec2f& c)
{
    return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
}

// Clamping values to keep them between 0 and 1
float Clamp(float value, float min = 0, float max = 1)
{
    return std::max(min, std::min(value, max));
}

// Interpolating the value between 2 vertices
// min is the starting point, max the ending point
// and gradient the % between the 2 points
float Interpolate(float min, float max, float gradient)
{
    return min + (max - min) * Clamp(gradient);
}

// drawing line between 2 points from left to right
// papb -> pcpd
// pa, pb, pc, pd must then be sorted before
void Device::ProcessScanLine(int y, Vec2f pa, Vec2f pb, Vec2f pc, Vec2f pd, Vec3f color)
{
    // Thanks to current Y, we can compute the gradient to compute others values like
    // the starting X (sx) and ending X (ex) to draw between
    // if pa.Y == pb.Y or pc.Y == pd.Y, gradient is forced to 1
    auto gradient1 = pa.y != pb.y ? (y - pa.y) / (pb.y - pa.y) : 1;
    auto gradient2 = pc.y != pd.y ? (y - pc.y) / (pd.y - pc.y) : 1;

    int sx = (int)Interpolate(pa.x, pb.x, gradient1);
    int ex = (int)Interpolate(pc.x, pd.x, gradient2);

    // drawing a line from left (sx) to right (ex)
    for (int x = sx; x < ex; x++)
    {
        DrawPoint(Vec2f(x, y), color);
    }
}

void Device::FillTriangle(Vec2f p1, Vec2f p2, Vec2f p3, Vec3f color) {

    // Sorting the points in order to always have this order on screen p1, p2 & p3
    // with p1 always up (thus having the Y the lowest possible to be near the top screen)
    // then p2 between p1 & p3
    if (p1.y > p2.y)
    {
        auto temp = p2;
        p2 = p1;
        p1 = temp;
    }

    if (p2.y > p3.y)
    {
        auto temp = p2;
        p2 = p3;
        p3 = temp;
    }

    if (p1.y > p2.y)
    {
        auto temp = p2;
        p2 = p1;
        p1 = temp;
    }

    // inverse slopes
    float dP1P2, dP1P3;

    // http://en.wikipedia.org/wiki/Slope
    // Computing inverse slopes
    if (p2.y - p1.y > 0)
        dP1P2 = (p2.x - p1.x) / (p2.y - p1.y);
    else
        dP1P2 = 0;

    if (p3.y - p1.y > 0)
        dP1P3 = (p3.x - p1.x) / (p3.y - p1.y);
    else
        dP1P3 = 0;

    // First case where triangles are like that:
    // P1
    // -
    // --
    // - -
    // -  -
    // -   - P2
    // -  -
    // - -
    // -
    // P3
    if (dP1P2 > dP1P3)
    {
        for (int y = (int)p1.y; y <= (int)p3.y; y++)
        {
            if (y < (int)p2.y)
            {
                ProcessScanLine(y, p1, p3, p1, p2, color);
            }
            else
            {
                ProcessScanLine(y, p1, p3, p2, p3, color);
            }
        }
    }
        // First case where triangles are like that:
        //       P1
        //        -
        //       --
        //      - -
        //     -  -
        // P2 -   -
        //     -  -
        //      - -
        //        -
        //       P3
    else
    {
        for (int y = (int)p1.y; y <= (int)p3.y; y++)
        {
            if (y < (int)p2.y)
            {
                ProcessScanLine(y, p1, p2, p1, p3, color);
            }
            else
            {
                ProcessScanLine(y, p2, p3, p1, p3, color);
            }
        }
    }

}

void Device::render(Camera camera, std::vector<Mesh> meshes, float fov) {

    // Projection Matrix
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = fov;
    float fAspectRatio = (float)height / (float)width;
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * M_PI);
    Matrix projectionMatrix = Matrix(4, 4, 0);
    projectionMatrix(0,0) = fAspectRatio * fFovRad;
    projectionMatrix(1,1) = fFovRad;
    projectionMatrix(2,2) = fFar / (fFar - fNear);
    projectionMatrix(3,2) = (-fFar * fNear) / (fFar - fNear);
    projectionMatrix(2,3) = 1.0f;
    projectionMatrix(3,3) = 0.0f;

    Matrix matRotZ(4, 4, 0), matRotX(4, 4, 0);

    for (auto mesh : meshes) {

        // Rotation Z
        matRotZ(0,0) = cosf(mesh.rotZ);
        matRotZ(0,1) = sinf(mesh.rotZ);
        matRotZ(1,0) = -sinf(mesh.rotZ);
        matRotZ(1,1) = cosf(mesh.rotZ);
        matRotZ(2,2) = 1;
        matRotZ(3,3) = 1;

        // Rotation X
        matRotX(0,0) = 1;
        matRotX(1,1) = cosf(mesh.rotX * 0.5f);
        matRotX(1,2) = sinf(mesh.rotX * 0.5f);
        matRotX(2,1) = -sinf(mesh.rotX * 0.5f);
        matRotX(2,2) = cosf(mesh.rotX * 0.5f);
        matRotX(3,3) = 1;

        for (auto tri : mesh.polygons) {

            Triangle projectedTriangle = Triangle();
            Triangle triTranslated, triRotatedZ, triRotatedZX;

            triRotatedZ.vertices[0] = MultiplyMatrixVector(tri.vertices[0], matRotZ);
            triRotatedZ.vertices[1] = MultiplyMatrixVector(tri.vertices[1], matRotZ);
            triRotatedZ.vertices[2] = MultiplyMatrixVector(tri.vertices[2], matRotZ);

            triRotatedZX.vertices[0] = MultiplyMatrixVector(triRotatedZ.vertices[0], matRotX);
            triRotatedZX.vertices[1] = MultiplyMatrixVector(triRotatedZ.vertices[1], matRotX);
            triRotatedZX.vertices[2] = MultiplyMatrixVector(triRotatedZ.vertices[2], matRotX);

            triTranslated = triRotatedZX;
            triTranslated.vertices[0].z = triRotatedZX.vertices[0].z + 3.0f;
            triTranslated.vertices[1].z = triRotatedZX.vertices[1].z + 3.0f;
            triTranslated.vertices[2].z = triRotatedZX.vertices[2].z + 3.0f;

            Vec3f normal, line1, line2;
            line1 = triTranslated.vertices[1] - triTranslated.vertices[0];
            line2 = triTranslated.vertices[2] - triTranslated.vertices[0];

            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;

            float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
            normal.x /= l; normal.y /= l; normal.z /= l;

            if (normal.x * (triTranslated.vertices[0].x - camera.position.x) +
                normal.y * (triTranslated.vertices[0].y - camera.position.y) +
                normal.z * (triTranslated.vertices[0].z - camera.position.z) < 0.0f) {
            //if (normal.z < 0) {
                projectedTriangle.vertices[0] = MultiplyMatrixVector(triTranslated.vertices[0], projectionMatrix);
                projectedTriangle.vertices[1] = MultiplyMatrixVector(triTranslated.vertices[1], projectionMatrix);
                projectedTriangle.vertices[2] = MultiplyMatrixVector(triTranslated.vertices[2], projectionMatrix);
                projectedTriangle.vertices[0].x += 1.0f;
                projectedTriangle.vertices[0].y += 1.0f;
                projectedTriangle.vertices[1].x += 1.0f;
                projectedTriangle.vertices[1].y += 1.0f;
                projectedTriangle.vertices[2].x += 1.0f;
                projectedTriangle.vertices[2].y += 1.0f;
                projectedTriangle.vertices[0].x *= 0.5f * (float) width;
                projectedTriangle.vertices[0].y *= 0.5f * (float) height;
                projectedTriangle.vertices[1].x *= 0.5f * (float) width;
                projectedTriangle.vertices[1].y *= 0.5f * (float) height;
                projectedTriangle.vertices[2].x *= 0.5f * (float) width;
                projectedTriangle.vertices[2].y *= 0.5f * (float) height;
                float color1 = 0.25f + ((float)((rand() % 2000+1) % mesh.verts.size()) / mesh.verts.size()) * 0.75f;
                float color2 = 0.25f + ((float)((rand() % 2000+1) % mesh.verts.size()) / mesh.verts.size()) * 0.75f;
                float color3 = 0.25f + ((float)((rand() % 2000+1) % mesh.verts.size()) / mesh.verts.size()) * 0.75f;
                //DrawTriangle(Vec2f(projectedTriangle.vertices[0].x, projectedTriangle.vertices[0].y),
                //             Vec2f(projectedTriangle.vertices[1].x, projectedTriangle.vertices[1].y),
                //             Vec2f(projectedTriangle.vertices[2].x, projectedTriangle.vertices[2].y),
                //             Vec3f(color1, color2, color3));
                FillTriangle(Vec2f(projectedTriangle.vertices[0].x, projectedTriangle.vertices[0].y),
                             Vec2f(projectedTriangle.vertices[1].x, projectedTriangle.vertices[1].y),
                             Vec2f(projectedTriangle.vertices[2].x, projectedTriangle.vertices[2].y),
                             Vec3f(color1, color2, color3));
            }
        }
    }
    std::ofstream ofs; // save the framebuffer to file
    ofs.open("./out.ppm",std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < height*width; ++i)
    {
        Vec3f &c = framebuffer[i];
        float max = std::max(c[0], std::
        max(c[1], c[2]));
        if (max>1) c = c*(1./max);
        for (size_t j = 0; j<3; j++)
        {
            ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}
