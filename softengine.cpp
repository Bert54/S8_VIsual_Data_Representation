#include <fstream>
#include <utility>
#include <iterator>
#include <cstring>
#include <algorithm>

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
    translationX = 0.0f;
    translationY = 0.0f;
    translationZ = 0.0f;
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
    translationX = 0.0f;
    translationY = 0.0f;
    translationZ = 0.0f;
}

void Mesh::setRotation(float rotationX, float rotationZ) {
    rotX = rotationX;
    rotZ = rotationZ;
}

void Mesh::setTranslation(float trX, float trY, float trZ) {
    translationX = trX;
    translationY = trY;
    translationZ = trZ;
}

Vec3f MultiplyMatrixVector(Vec4f v, Matrix m)
{
    Vec4f out = Vec4f();
    out.x = v.x * m(0,0) + v.y * m(1,0) + v.z * m(2,0) + out.w * m(3,0);
    out.y = v.x * m(0,1) + v.y * m(1,1) + v.z * m(2,1) + out.w * m(3,1);
    out.z = v.x * m(0,2) + v.y * m(1,2) + v.z * m(2,2) + out.w * m(3,2);
    out.w = v.x * m(0,3) + v.y * m(1,3) + v.z * m(2,3) + out.w * m(3,3);
    return Vec3f(out.x, out.y, out.z);
}

Matrix Matrix_MakeIdentity()
{
    Matrix matrix = Matrix(4,4,0);
    matrix(0,0) = 1.0f;
    matrix(1,1) = 1.0f;
    matrix(2,2) = 1.0f;
    matrix(3,3) = 1.0f;
    return matrix;
}

Matrix Matrix_MakeRotationX(float fAngleRad)
{
    Matrix matrix = Matrix(4,4,0);
    matrix(0,0) = 1;
    matrix(1,1) = cosf(fAngleRad * 0.5f);
    matrix(1,2) = sinf(fAngleRad * 0.5f);
    matrix(2,1) = -sinf(fAngleRad * 0.5f);
    matrix(2,2) = cosf(fAngleRad * 0.5f);
    matrix(3,3) = 1;
    return matrix;
}

Matrix Matrix_MakeRotationZ(float fAngleRad)
{
    Matrix matrix = Matrix(4,4,0);
    matrix(0,0) = cosf(fAngleRad);
    matrix(0,1) = sinf(fAngleRad);
    matrix(1,0) = -sinf(fAngleRad);
    matrix(1,1) = cosf(fAngleRad);
    matrix(2,2) = 1;
    matrix(3,3) = 1;
    return matrix;
}

Matrix Matrix_MakeTranslation(float x, float y, float z)
{
    Matrix matrix = Matrix_MakeIdentity();
    matrix(3,0) = x;
    matrix(3,1) = y;
    matrix(3,2) = z;
    return matrix;
}

float Vector_Length(Vec3f vec) {
    return sqrtf(vec * vec);
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
    float dP1P2, dP1P3;
    if (p2.y - p1.y > 0)
        dP1P2 = (p2.x - p1.x) / (p2.y - p1.y);
    else
        dP1P2 = 0;

    if (p3.y - p1.y > 0)
        dP1P3 = (p3.x - p1.x) / (p3.y - p1.y);
    else
        dP1P3 = 0;
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

Vec3f GetColour(float lum)
{
    int pixel_bw = (int)(13.0f*lum);
    switch (pixel_bw)
    {
        case 0: return Vec3f(0.f, 0.f, 0.f);

        case 1: return Vec3f(0.08f, 0.08f, 0.08f);
        case 2: return Vec3f(0.17f, 0.17f, 0.17f);
        case 3: return Vec3f(0.25f, 0.25f, 0.25f);
        case 4: return Vec3f(0.33f, 0.33f, 0.33f);;

        case 5: return Vec3f(0.42f, 0.42f, 0.42f);
        case 6: return Vec3f(0.50f, 0.50f, 0.50f);
        case 7: return Vec3f(0.58f, 0.58f, 0.58f);
        case 8: return Vec3f(0.65f, 0.65f, 0.65f);

        case 9:  return Vec3f(0.73f, 0.73f, 0.74);
        case 10: return Vec3f(0.83f, 0.83f, 0.83f);
        case 11: return Vec3f(0.92f, 0.92f, 0.92f);
        case 12: return Vec3f(1.f, 1.f, 1.f);
    }

    return Vec3f(0.f, 0.f, 0.f);
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

    // Illumination
    Vec3f light_direction = { 0.0f, 0.0f, -1.0f };
    float l = sqrtf(light_direction.x*light_direction.x + light_direction.y*light_direction.y + light_direction.z*light_direction.z);
    light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

    for (auto mesh : meshes) {

        Matrix matRotZ = Matrix_MakeRotationZ(mesh.rotZ), matRotX = Matrix_MakeRotationX(mesh.rotX), matTran = Matrix_MakeTranslation(mesh.translationX, mesh.translationY, mesh.translationZ);

        std::vector<Triangle> trianglesToRaster;

        for (Triangle tri : mesh.polygons) {

            Triangle projectedTriangle = Triangle();
            Triangle triTran, triTranslated, triRotatedZ, triRotatedZX;

            triRotatedZ.vertices[0] = MultiplyMatrixVector(Vec4f(tri.vertices[0].x, tri.vertices[0].y, tri.vertices[0].z, 1), matRotZ);
            triRotatedZ.vertices[1] = MultiplyMatrixVector(Vec4f(tri.vertices[1].x, tri.vertices[1].y, tri.vertices[1].z, 1), matRotZ);
            triRotatedZ.vertices[2] = MultiplyMatrixVector(Vec4f(tri.vertices[2].x, tri.vertices[2].y, tri.vertices[2].z, 1), matRotZ);

            triRotatedZX.vertices[0] = MultiplyMatrixVector(Vec4f(triRotatedZ.vertices[0].x, triRotatedZ.vertices[0].y, triRotatedZ.vertices[0].z, 1), matRotX);
            triRotatedZX.vertices[1] = MultiplyMatrixVector(Vec4f(triRotatedZ.vertices[1].x, triRotatedZ.vertices[1].y, triRotatedZ.vertices[1].z, 1), matRotX);
            triRotatedZX.vertices[2] = MultiplyMatrixVector(Vec4f(triRotatedZ.vertices[2].x, triRotatedZ.vertices[2].y, triRotatedZ.vertices[2].z, 1), matRotX);

            triTran.vertices[0] = MultiplyMatrixVector(Vec4f(triRotatedZX.vertices[0].x, triRotatedZX.vertices[0].y, triRotatedZX.vertices[0].z, 1), matTran);
            triTran.vertices[1] = MultiplyMatrixVector(Vec4f(triRotatedZX.vertices[1].x, triRotatedZX.vertices[1].y, triRotatedZX.vertices[1].z, 1), matTran);
            triTran.vertices[2] = MultiplyMatrixVector(Vec4f(triRotatedZX.vertices[2].x, triRotatedZX.vertices[2].y, triRotatedZX.vertices[2].z, 1), matTran);

            triTranslated = triTran;
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

                float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;

                projectedTriangle.color = GetColour(dp);

                projectedTriangle.vertices[0] = MultiplyMatrixVector(Vec4f(triTranslated.vertices[0].x, triTranslated.vertices[0].y, triTranslated.vertices[0].z, 1), projectionMatrix);
                projectedTriangle.vertices[1] = MultiplyMatrixVector(Vec4f(triTranslated.vertices[1].x, triTranslated.vertices[1].y, triTranslated.vertices[1].z, 1), projectionMatrix);
                projectedTriangle.vertices[2] = MultiplyMatrixVector(Vec4f(triTranslated.vertices[2].x, triTranslated.vertices[2].y, triTranslated.vertices[2].z, 1), projectionMatrix);
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

                trianglesToRaster.push_back(projectedTriangle);

            }
        }

        // Sort triangles from back to front
        sort(trianglesToRaster.begin(), trianglesToRaster.end(), [](Triangle &t1, Triangle &t2)
        {
            float z1 = (t1.vertices[0].z + t1.vertices[1].z + t1.vertices[2].z) / 3.0f;
            float z2 = (t2.vertices[0].z + t2.vertices[1].z + t2.vertices[2].z) / 3.0f;
            return z1 > z2;
        });

        for (auto &triProjected : trianglesToRaster)
        {
            float color1 =
                    0.25f + ((float) ((rand() % 2000 + 1) % mesh.verts.size()) / mesh.verts.size()) * 0.75f;
            float color2 =
                    0.25f + ((float) ((rand() % 2000 + 1) % mesh.verts.size()) / mesh.verts.size()) * 0.75f;
            float color3 =
                    0.25f + ((float) ((rand() % 2000 + 1) % mesh.verts.size()) / mesh.verts.size()) * 0.75f;
            //DrawTriangle(Vec2f(projectedTriangle.vertices[0].x, projectedTriangle.vertices[0].y),
            //             Vec2f(projectedTriangle.vertices[1].x, projectedTriangle.vertices[1].y),
            //             Vec2f(projectedTriangle.vertices[2].x, projectedTriangle.vertices[2].y),
            //             Vec3f(color1, color2, color3));
            FillTriangle(Vec2f(triProjected.vertices[0].x, triProjected.vertices[0].y),
                         Vec2f(triProjected.vertices[1].x, triProjected.vertices[1].y),
                         Vec2f(triProjected.vertices[2].x, triProjected.vertices[2].y),
                    //           Vec3f(color1, color2, color3));
                         triProjected.color);
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
