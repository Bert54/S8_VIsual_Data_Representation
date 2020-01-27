#include <fstream>
#include <utility>
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

Vec2f DivideVector2fbyValue(Vec2f vec, float value) {
    vec.x /= value;
    vec.y /= value;
    return vec;
}

void Device::DrawPoint(Vec2f p, Vec3f color) {
    int index = ((int)p.x + (int)p.y * width);
    framebuffer[index].x = color.x;
    framebuffer[index].y = color.y;
    framebuffer[index].z = color.z;
}

void Device::DrawLine(Vec2f p1, Vec2f p2, Vec3f color) {
    DrawPoint(p1, color);
    DrawPoint(p2, color);
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

void Device::render(Camera camera, std::vector<Mesh> meshes) {

    // Projection Matrix
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 70.0f;
    float fAspectRatio = (float)height / (float)width;
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * M_PI);
    float fTheta = 0;
    Matrix projectionMatrix = Matrix(4, 4, 0);
    projectionMatrix(0,0) = fAspectRatio * fFovRad;
    projectionMatrix(1,1) = fFovRad;
    projectionMatrix(2,2) = fFar / (fFar - fNear);
    projectionMatrix(3,2) = (-fFar * fNear) / (fFar - fNear);
    projectionMatrix(2,3) = 1.0f;
    projectionMatrix(3,3) = 0.0f;

    Matrix matRotZ(4, 4, 0), matRotX(4, 4, 0);
    fTheta += 0.f;

    // Rotation Z
    matRotZ(0,0) = cosf(fTheta);
    matRotZ(0,1) = sinf(fTheta);
    matRotZ(1,0) = -sinf(fTheta);
    matRotZ(1,1) = cosf(fTheta);
    matRotZ(2,2) = 1;
    matRotZ(3,3) = 1;

    // Rotation X
    matRotX(0,0) = 1;
    matRotX(1,1) = cosf(fTheta * 0.5f);
    matRotX(1,2) = sinf(fTheta * 0.5f);
    matRotX(2,1) = -sinf(fTheta * 0.5f);
    matRotX(2,2) = cosf(fTheta * 0.5f);
    matRotX(3,3) = 1;

    for (auto mesh : meshes) {
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

            projectedTriangle.vertices[0] = MultiplyMatrixVector(triTranslated.vertices[0], projectionMatrix);
            projectedTriangle.vertices[1] = MultiplyMatrixVector(triTranslated.vertices[1], projectionMatrix);
            projectedTriangle.vertices[2] = MultiplyMatrixVector(triTranslated.vertices[2], projectionMatrix);
            projectedTriangle.vertices[0].x += 1.0f; projectedTriangle.vertices[0].y += 1.0f;
            projectedTriangle.vertices[1].x += 1.0f; projectedTriangle.vertices[1].y += 1.0f;
            projectedTriangle.vertices[2].x += 1.0f; projectedTriangle.vertices[2].y += 1.0f;
            projectedTriangle.vertices[0].x *= 0.5f * (float)width;
            projectedTriangle.vertices[0].y *= 0.5f * (float)height;
            projectedTriangle.vertices[1].x *= 0.5f * (float)width;
            projectedTriangle.vertices[1].y *= 0.5f * (float)height;
            projectedTriangle.vertices[2].x *= 0.5f * (float)width;
            projectedTriangle.vertices[2].y *= 0.5f * (float)height;
            //std::cout << "x: " << projectedTriangle.vertices[0].x << " ; y: " << projectedTriangle.vertices[0].y << "\n";
            //std::cout << "x: " << projectedTriangle.vertices[1].x << " ; y: " << projectedTriangle.vertices[1].y << "\n";
            //std::cout << "x: " << projectedTriangle.vertices[2].x << " ; y: " << projectedTriangle.vertices[2].y << "\n";
            //std::cout << "------------------------\n";
            DrawTriangle(Vec2f(projectedTriangle.vertices[0].x, projectedTriangle.vertices[0].y),
                         Vec2f(projectedTriangle.vertices[1].x, projectedTriangle.vertices[1].y),
                         Vec2f(projectedTriangle.vertices[2].x, projectedTriangle.vertices[2].y),
                         Vec3f(1.f, 1.f, 1.f));
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