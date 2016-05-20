#pragma once
#include "framework/geometry/Ray.h"
#include "framework/geometry/Plane.h"
#include "framework/math/Vec2.h"
#include "framework/math/Vec3.h"
#include "framework/math/Vec4.h"

namespace ettention
{
    class Image;
    template<typename _T> class BoundingBox;
    typedef BoundingBox<Vec3f> BoundingBox3f;

    class Visualizer
    {
    public:
        Visualizer(const std::string& path, const std::string& basename);
        ~Visualizer();

        void append(const Vec3f& point, float radius, const Vec4f& color);
        void append(const Ray& ray, float tStart, float tEnd, float radius, const Vec4f& color);
        void append(const Plane& plane, const Vec3f& center, float radius, const Vec4f& color);
        void appendConnection(const Vec3f& pointA, const Vec3f& pointB, float radius, const Vec4f& color);
        void appendGrid(const Vec3f& base, const Vec3f u, const Vec3f& v, const Vec2ui& resolution, const Vec4f& color);
        void appendTexture(const Vec3f& base, const Vec3f u, const Vec3f& v, const Image* texture, bool shaded);
        void appendBoundingBox(const BoundingBox3f& boundingBox, const Vec4f& color);
        bool writeMesh() const;
        void clear();

    private:
        struct Index 
        {
            unsigned int vertexIndex;
            unsigned int normalIndex;
            unsigned int texCoordsIndex;
        };
        static const unsigned int INVALID_INDEX = (unsigned int)-1;

        typedef std::vector<Index> Face;

        struct Material
        {
            std::vector<Face> faces;
            Vec4f color;
            Image* texture;
        };

        std::string path;
        std::string basename;
        std::vector<Vec3f> vertices;
        std::vector<Vec3f> normals;
        std::vector<Vec2f> texCoords;
        std::vector<Material> materials;
        Material currentMaterial;

        void createNewMaterial(const Vec4f& color);
        void createNewMaterial(const Image* texture);
        void finishMaterial();
        void addFace(size_t numVertices, const int* vertexOffsets, const int* normalOffsets, const int* texCoordsOffsets = 0);
        void addFace(size_t numVertices, const int* vertexOffsets, int normalOffset, const int* texCoordsOffsets = 0);
        void writeVertices(std::ofstream& file) const;
        void writeNormals(std::ofstream& file) const;
        void writeTexCoords(std::ofstream& file) const;
        void writeFaces(std::ofstream& objFile, std::ofstream& mtlFile) const;
        void writeMaterial(std::ofstream& file, const std::string& materialName, const Material material) const;
        std::pair<Vec3f, Vec3f> getOrthonormalVectorsTo(const Vec3f& v);
    };
}