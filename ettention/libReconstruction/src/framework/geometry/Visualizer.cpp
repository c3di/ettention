#include "stdafx.h"
#include "Visualizer.h"
#include "framework/geometry/GeometricAlgorithms.h"
#include "framework/Logger.h"
#include "io/serializer/ImageSerializer.h"
#include "model/image/Image.h"

namespace ettention
{
    Visualizer::Visualizer(const std::string& path, const std::string& basename)
        : path(path.back() != '/' && path.back() != '\\' ? path + "/" : path)
        , basename(basename)
    {
        boost::filesystem::create_directories(this->path + "textures/");
    }

    Visualizer::~Visualizer()
    {
        for(auto it = materials.begin(); it != materials.end(); ++it)
        {
            delete it->texture;
        }
    }

    void Visualizer::addFace(size_t numVertices, const int* vertexOffsets, const int* normalOffsets, const int* texCoordsOffsets)
    {
        Face face;
        for(size_t i = 0; i < numVertices; ++i)
        {
            Index index =
            {
                1 + (unsigned int)((int)vertices.size() + vertexOffsets[i]),
                1 + (unsigned int)((int)normals.size() + normalOffsets[i]),
                INVALID_INDEX,
            };
            if(texCoordsOffsets)
            {
                index.texCoordsIndex = 1 + (unsigned int)((int)texCoords.size() + texCoordsOffsets[i]);
            }
            face.push_back(index);
        }
        currentMaterial.faces.push_back(face);
    }

    void Visualizer::addFace(size_t numVertices, const int* vertexOffsets, int normalOffset, const int* texCoordsOffsets)
    {
        std::vector<int> normalOffsets(numVertices);
        std::fill(normalOffsets.begin(), normalOffsets.end(), normalOffset);
        addFace(numVertices, vertexOffsets, &normalOffsets[0], texCoordsOffsets);
    }

    void Visualizer::append(const Vec3f& point, float radius, const Vec4f& color)
    {
        vertices.push_back(point + Vec3f(+radius, 0.0f, 0.0f));
        vertices.push_back(point + Vec3f(-radius, 0.0f, 0.0f));
        vertices.push_back(point + Vec3f(0.0f, +radius, 0.0f));
        vertices.push_back(point + Vec3f(0.0f, -radius, 0.0f));
        vertices.push_back(point + Vec3f(0.0f, 0.0f, +radius));
        vertices.push_back(point + Vec3f(0.0f, 0.0f, -radius));
        normals.push_back(Vec3f(+1.0f, 0.0f, 0.0f));
        normals.push_back(Vec3f(-1.0f, 0.0f, 0.0f));
        normals.push_back(Vec3f(0.0f, +1.0f, 0.0f));
        normals.push_back(Vec3f(0.0f, -1.0f, 0.0f));
        normals.push_back(Vec3f(0.0f, 0.0f, +1.0f));
        normals.push_back(Vec3f(0.0f, 0.0f, -1.0f));
        int offsets[8][3] =
        {
            { -6, -4, -2, },
            { -1, -4, -6, },
            { -5, -4, -1, },
            { -2, -4, -5, },
            { -6, -3, -1, },
            { -1, -3, -5, },
            { -5, -3, -2, },
            { -2, -3, -6, }
        };
        createNewMaterial(color);
        for(int i = 0; i < 8; ++i)
        {
            addFace(3, offsets[i], offsets[i]);
        }
        finishMaterial();
    }

    void Visualizer::append(const Ray& ray, float tStart, float tEnd, float radius, const Vec4f& color)
    {
        auto uv = getOrthonormalVectorsTo(ray.getNormalizedDirection());
        Vec3f start = ray.getPointAt(tStart);
        Vec3f end = ray.getPointAt(tEnd);
        vertices.push_back(start + radius * uv.first);
        vertices.push_back(start - radius * uv.first);
        vertices.push_back(start + radius * uv.second);
        vertices.push_back(start - radius * uv.second);
        vertices.push_back(end + radius * uv.first);
        vertices.push_back(end - radius * uv.first);
        vertices.push_back(end + radius * uv.second);
        vertices.push_back(end - radius * uv.second);
        normals.push_back(uv.first);
        normals.push_back(-uv.first);
        normals.push_back(uv.second);
        normals.push_back(-uv.second);
        int vertexOffsets[4][4] =
        {
            { -8, -4, -3, -7, },
            { -8, -7, -3, -4, },
            { -1, -2, -6, -5, },
            { -1, -5, -6, -2, },
        };
        int normalOffsets[4][4] =
        {
            { -4, -4, -4, -4, },
            { -3, -3, -3, -3, },
            { -2, -2, -2, -2, },
            { -1, -1, -1, -1, },
        };
        createNewMaterial(color);
        for(int i = 0; i < 4; ++i)
        {
            addFace(4, vertexOffsets[i], normalOffsets[i]);
        }
        finishMaterial();
    }

    void Visualizer::append(const Plane& plane, const Vec3f& center, float radius, const Vec4f& color)
    {
        static const int SEGMENTS = 32;
        auto c = GeometricAlgorithms::getClosestPointOnPlane(center, plane);
        auto uv = getOrthonormalVectorsTo(plane.getNormal());
        auto u = uv.first;
        auto v = uv.second;
        normals.push_back(plane.getNormal());
        std::vector<int> vertexOffsets;
        for(int i = 0; i < SEGMENTS; ++i)
        {
            double phi = 2.0 * M_PI * (double)i / (double)SEGMENTS;
            float sinPhi = (float)std::sin(-phi);
            float cosPhi = (float)std::cos(phi);
            vertices.push_back(c + radius * (sinPhi * u + cosPhi * v));
            vertexOffsets.push_back(-i - 1);
        }
        std::vector<int> normalOffsets;
        normalOffsets.resize(SEGMENTS);
        std::fill(normalOffsets.begin(), normalOffsets.end(), -1);
        createNewMaterial(color);
        addFace(SEGMENTS, &vertexOffsets[0], &normalOffsets[0]);
        finishMaterial();
    }

    void Visualizer::appendGrid(const Vec3f& base, const Vec3f u, const Vec3f& v, const Vec2ui& resolution, const Vec4f& color)
    {
        int vertexOffsets1[][4] =
        {
            { -8, -7, -3, -4, },
            { -7, -6, -2, -3, },
            { -6, -5, -1, -2, },
            { -5, -8, -4, -1, },
        };
        int vertexOffsets2[][4] =
        {
            { -4, -3, -7, -8, },
            { -3, -2, -6, -7, },
            { -2, -1, -5, -6, },
            { -1, -4, -8, -5, },
        };
        int normalOffsets1[4] = { -1, -1, -1, -1, };
        int normalOffsets2[4] = { -2, -2, -2, -2, };
        normals.push_back(crossProduct(v, u));
        normals.push_back(crossProduct(u, v));
        createNewMaterial(color);
        for(unsigned int y = 0; y < resolution.y - 1; ++y)
        {
            for(unsigned int x = 0; x < resolution.x - 1; ++x)
            {
                vertices.push_back(base + ((float)x + 0.0f) * u + ((float)y + 0.0f) * v);
                vertices.push_back(base + ((float)x + 1.0f) * u + ((float)y + 0.0f) * v);
                vertices.push_back(base + ((float)x + 1.0f) * u + ((float)y + 1.0f) * v);
                vertices.push_back(base + ((float)x + 0.0f) * u + ((float)y + 1.0f) * v);
                vertices.push_back(base + ((float)x + 0.1f) * u + ((float)y + 0.1f) * v);
                vertices.push_back(base + ((float)x + 0.9f) * u + ((float)y + 0.1f) * v);
                vertices.push_back(base + ((float)x + 0.9f) * u + ((float)y + 0.9f) * v);
                vertices.push_back(base + ((float)x + 0.1f) * u + ((float)y + 0.9f) * v);
                for(int i = 0; i < 4; ++i)
                {
                    addFace(4, vertexOffsets1[i], normalOffsets1);
                    addFace(4, vertexOffsets2[i], normalOffsets2);
                }
            }
        }
        finishMaterial();
    }

    void Visualizer::appendTexture(const Vec3f& base, const Vec3f u, const Vec3f& v, const Image* texture, bool shaded)
    {
        normals.push_back(shaded ? crossProduct(v, u) : Vec3f(0.0f, 0.0f, 0.0f));
        normals.push_back(shaded ? crossProduct(u, v) : Vec3f(0.0f, 0.0f, 0.0f));
        createNewMaterial(texture);
        vertices.push_back(base);
        vertices.push_back(base + (float)texture->getResolution().x * u);
        vertices.push_back(base + (float)texture->getResolution().x * u + (float)texture->getResolution().y * v);
        vertices.push_back(base + (float)texture->getResolution().y * v);
        texCoords.push_back(Vec2f(1.0f, 0.0f));
        texCoords.push_back(Vec2f(0.0f, 0.0f));
        texCoords.push_back(Vec2f(0.0f, 1.0f));
        texCoords.push_back(Vec2f(1.0f, 1.0f));
        const int vertexOffsets1[4] = { -4, -3, -2, -1, };
        const int vertexOffsets2[4] = { -1, -2, -3, -4, };
        addFace(4, vertexOffsets1, -1, vertexOffsets1);
        addFace(4, vertexOffsets2, -2, vertexOffsets2);
        finishMaterial();
    }

    void Visualizer::appendConnection(const Vec3f& pointA, const Vec3f& pointB, float radius, const Vec4f& color)
    {
        append(Ray::createFromPoints(pointA, pointB), 0.0f, (pointB - pointA).getLengthF(), radius, color);
    }

    void Visualizer::appendBoundingBox(const BoundingBox3f& boundingBox, const Vec4f& color)
    {
        vertices.push_back(Vec3f(boundingBox.getMin().x, boundingBox.getMin().y, boundingBox.getMin().z));
        vertices.push_back(Vec3f(boundingBox.getMax().x, boundingBox.getMin().y, boundingBox.getMin().z));
        vertices.push_back(Vec3f(boundingBox.getMax().x, boundingBox.getMin().y, boundingBox.getMax().z));
        vertices.push_back(Vec3f(boundingBox.getMin().x, boundingBox.getMin().y, boundingBox.getMax().z));
        vertices.push_back(Vec3f(boundingBox.getMin().x, boundingBox.getMax().y, boundingBox.getMin().z));
        vertices.push_back(Vec3f(boundingBox.getMax().x, boundingBox.getMax().y, boundingBox.getMin().z));
        vertices.push_back(Vec3f(boundingBox.getMax().x, boundingBox.getMax().y, boundingBox.getMax().z));
        vertices.push_back(Vec3f(boundingBox.getMin().x, boundingBox.getMax().y, boundingBox.getMax().z));
        normals.push_back(Vec3f(1.0f, 0.0f, 0.0f));
        normals.push_back(Vec3f(-1.0f, 0.0f, 0.0f));
        normals.push_back(Vec3f(0.0f, 1.0f, 0.0f));
        normals.push_back(Vec3f(0.0f, -1.0f, 0.0f));
        normals.push_back(Vec3f(0.0f, 0.0f, 1.0f));
        normals.push_back(Vec3f(0.0f, 0.0f, -1.0f));
        int faces[6][4] =
        {
            { -7, -3, -2, -6, },
            { -4, -8, -5, -1, },
            { -5, -6, -2, -1, },
            { -4, -3, -7, -8, },
            { -3, -4, -1, -2, },
            { -8, -7, -6, -5, },
        };
        createNewMaterial(color);
        for(unsigned int i = 0; i < 6; ++i)
        {
            addFace(4, faces[i], i - 6);
        }
        finishMaterial();
    }

    void Visualizer::clear()
    {
        materials.clear();
        vertices.clear();
        normals.clear();
        currentMaterial.faces.clear();
    }

    std::pair<Vec3f, Vec3f> Visualizer::getOrthonormalVectorsTo(const Vec3f& v)
    {
        Vec3f u;
        if(fabs(v.x) > std::max(fabs(v.y), fabs(v.z)))
        {
            u = doNormalize(Vec3f(-v.y / v.x, 1.0f, 0.0f));
        }
        else if(fabs(v.y) > std::max(fabs(v.x), fabs(v.z)))
        {
            u = doNormalize(Vec3f(0.0f, -v.z / v.y, 1.0f));
        }
        else
        {
            u = doNormalize(Vec3f(1.0f, 0.0f, -v.x / v.z));
        }
        return std::make_pair(u, doNormalize(crossProduct(u, v)));
    }

    void Visualizer::createNewMaterial(const Vec4f& color)
    {
        finishMaterial();
        currentMaterial.color = color;
        currentMaterial.texture = 0;
    }

    void Visualizer::createNewMaterial(const Image* texture)
    {
        finishMaterial();
        currentMaterial.color = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
        currentMaterial.texture = new Image(texture);
    }

    void Visualizer::finishMaterial()
    {
        if(!currentMaterial.faces.empty())
        {
            materials.push_back(currentMaterial);
            currentMaterial.faces.clear();
        }
    }

    bool Visualizer::writeMesh() const
    {
        std::ofstream objFile(path + basename + ".obj");
        std::ofstream mtlFile(path + basename + ".mtl");
        if(!objFile.is_open() || !mtlFile.is_open())
        {
            LOGGER_ERROR("Opening of " << basename << " obj/mtl files failed!");
            return false;
        }
        else
        {
            objFile << "mtllib " << basename << ".mtl" << std::endl << std::endl;
            writeVertices(objFile);
            writeNormals(objFile);
            writeTexCoords(objFile);
            writeFaces(objFile, mtlFile);
            objFile.close();
            return true;
        }
    }

    void Visualizer::writeVertices(std::ofstream& file) const
    {
        for(auto it = vertices.begin(); it != vertices.end(); ++it)
        {
            file << "v " << it->x << " " << it->y << " " << it->z << std::endl;
        }
        file << std::endl;
    }

    void Visualizer::writeNormals(std::ofstream& file) const
    {
        for(auto it = normals.begin(); it != normals.end(); ++it)
        {
            file << "vn " << it->x << " " << it->y << " " << it->z << std::endl;
        }
        file << std::endl;
    }

    void Visualizer::writeTexCoords(std::ofstream& file) const
    {
        for(auto it = texCoords.begin(); it != texCoords.end(); ++it)
        {
            file << "vt " << it->x << " " << it->y << std::endl;
        }
        file << std::endl;
    }

    void Visualizer::writeFaces(std::ofstream& objFile, std::ofstream& mtlFile) const
    {
        unsigned int materialCount = 0;
        for(auto matIt = materials.begin(); matIt != materials.end(); ++matIt)
        {
            std::stringstream materialName;
            materialName << "mat_" << materialCount++;
            writeMaterial(mtlFile, materialName.str(), *matIt);
            objFile << "usemtl " << materialName.str() << std::endl;
            for(auto faceIt = matIt->faces.begin(); faceIt != matIt->faces.end(); ++faceIt)
            {
                objFile << "f";
                for(auto indexIt = faceIt->begin(); indexIt != faceIt->end(); ++indexIt)
                {
                    objFile << " " << indexIt->vertexIndex << "/";
                    if(indexIt->texCoordsIndex != INVALID_INDEX)
                    {
                        objFile << indexIt->texCoordsIndex;
                    }
                    objFile << "/" << indexIt->normalIndex;;
                }
                objFile << std::endl;
            }
            objFile << std::endl;
        }
    }

    void Visualizer::writeMaterial(std::ofstream& file, const std::string& materialName, const Material material) const
    {
        file << "newmtl " << materialName << std::endl;
        file << "Ns 96.078431" << std::endl;
        file << "Ka 0.000000 0.000000 0.000000" << std::endl;
        file << "Kd " << material.color.x << " " << material.color.y << " " << material.color.z << std::endl;
        file << "Ks 0.500000 0.500000 0.500000" << std::endl;
        file << "Ni 1.000000" << std::endl;
        file << "d 1.000000" << std::endl;
        file << "illum 2" << std::endl;
        if(material.texture)
        {
            std::stringstream materialTextureFileName;
            materialTextureFileName << basename << "_" << materialName << "_kd";
            file << "map_Kd textures/" << materialTextureFileName.str() << ".png" << std::endl;
            ImageSerializer::writeImage(path + "textures/" + materialTextureFileName.str(), material.texture, ImageSerializer::PNG_GRAY_8);
        }
        file << std::endl;
    }
}