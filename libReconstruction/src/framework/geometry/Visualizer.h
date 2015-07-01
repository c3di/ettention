#pragma once
#include "framework/geometry/Ray.h"
#include "framework/geometry/Plane.h"
#include "framework/math/Vec2.h"
#include "framework/math/Vec3.h"
#include "framework/math/Vec4.h"

namespace ettention
{
    class Visualizer
    {
    public:
        Visualizer();
        ~Visualizer();

        void append(const Vec3f& point, float radius, const Vec4f& color);
        void append(const Ray& ray, float tStart, float tEnd, float radius, const Vec4f& color);
        void append(const Plane& plane, const Vec3f& center, float radius, const Vec4f& color);
        void appendConnection(const Vec3f& pointA, const Vec3f& pointB, float radius, const Vec4f& color);
        void appendGrid(const Vec3f& base, const Vec3f u, const Vec3f& v, const Vec2ui& resolution, const Vec4f& color);
        void appendAabb(const Vec3f& min, const Vec3f& max, const Vec4f& color);
        bool writeObjAndMtl(const std::string& path, const std::string& basename) const;
        void clear();

    private:
        struct Index 
        {
            unsigned int vertexIndex;
            unsigned int normalIndex;
        };

        typedef std::vector<Index> Face;

        struct Material
        {
            std::vector<Face> faces;
            Vec4f color;
        };

        std::vector<Vec3f> vertices;
        std::vector<Vec3f> normals;
        std::vector<Material> materials;
        Material currentMaterial;

        void createNewMaterial(const Vec4f& color);
        void finishMaterial();
        void addFace(const int* vertexOffsets, const int* normalOffsets, size_t numVertices);
        void addFace(const int* vertexOffsets, int normalOffset, size_t numVertices);
        void writeVertices(std::ofstream& file) const;
        void writeNormals(std::ofstream& file) const;
        void writeFaces(std::ofstream& objFile, std::ofstream& mtlFile) const;
        void writeMaterial(std::ofstream& file, unsigned int materialNr, const Vec4f& color) const;
        std::pair<Vec3f, Vec3f> getOrthonormalVectorsTo(const Vec3f& v);
    };
}