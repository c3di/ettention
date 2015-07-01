#include "stdafx.h"
#include "ScannerGeometry.h"
#include "framework/geometry/Visualizer.h"
#include "io/datasource/ImageStackDatasource.h"

namespace ettention
{
    ScannerGeometry::ScannerGeometry()
        : projectionType(PROJECTION_UNKNOWN)
        , sourceBase(0.0f, 0.0f, 0.0f)
        , detectorBase(0.0f, 0.0f, 0.0f)
        , horizontalPitch(0.0f, 0.0f, 0.0f)
        , verticalPitch(0.0f, 0.0f, 0.0f)
        , resolution(0, 0)
    {
    }

    ScannerGeometry::ScannerGeometry(ImageStackDatasource* inputStack)
        : projectionType(inputStack->getProjectionType())
        , resolution(inputStack->getResolution())
    {
    }

    ScannerGeometry::ScannerGeometry(ProjectionType projectionType, const Vec2ui& resolution)
        : projectionType(projectionType)
        , resolution(resolution)
    {
    }

    ScannerGeometry::ScannerGeometry(const Vec3f& sourceBase, const Vec3f& detectorBase, const Vec3f& horizontalPitch, const Vec3f& verticalPitch, const Vec2ui& resolution)
        : projectionType(PROJECTION_UNKNOWN)
        , sourceBase(sourceBase)
        , detectorBase(detectorBase)
        , horizontalPitch(horizontalPitch)
        , verticalPitch(verticalPitch)
        , resolution(resolution)
    {
    }

    ScannerGeometry::~ScannerGeometry()
    {
    }

    void ScannerGeometry::set(const Vec3f& sourceBase, const Vec3f& detectorBase, const Vec3f& horizontalPitch, const Vec3f& verticalPitch)
    {
        this->sourceBase = sourceBase;
        this->detectorBase = detectorBase;
        this->horizontalPitch = horizontalPitch;
        this->verticalPitch = verticalPitch;
    }

    void ScannerGeometry::setProjectionType(ProjectionType projectionType)
    {
        this->projectionType = projectionType;
    }

    ScannerGeometry::ProjectionType ScannerGeometry::getProjectionType() const
    {
        return projectionType;
    }

    void ScannerGeometry::setResolution(const Vec2ui& resolution)
    {
        this->resolution = resolution;
    }

    const Vec2ui& ScannerGeometry::getResolution() const
    {
        return resolution;
    }

    const Vec3f& ScannerGeometry::getSourceBase() const
    {
        return sourceBase;
    }

    Vec3f ScannerGeometry::getSourceCenter() const
    {
        switch(projectionType)
        {
        case PROJECTION_PARALLEL:
            return sourceBase + 0.5f * ((float)resolution.x * horizontalPitch + (float)resolution.y * verticalPitch);
        case PROJECTION_PERSPECTIVE:
            return sourceBase;
        default:
            throw Exception("Illegal projection type!");
        }
    }

    const Vec3f& ScannerGeometry::getDetectorBase() const
    {
        return detectorBase;
    }

    Vec3f ScannerGeometry::getDetectorCenter() const
    {
        return detectorBase + 0.5f * ((float)resolution.x * horizontalPitch + (float)resolution.y * verticalPitch);
    }

    const Vec3f& ScannerGeometry::getHorizontalPitch() const
    {
        return horizontalPitch;
    }

    const Vec3f& ScannerGeometry::getVerticalPitch() const
    {
        return verticalPitch;
    }

    Vec3f ScannerGeometry::getRayDirection() const
    {
        switch(projectionType)
        {
        case PROJECTION_PARALLEL:
            return Normalize(detectorBase - sourceBase);
        case PROJECTION_PERSPECTIVE:
            return Normalize(getDetectorCenter() - sourceBase);
        default:
            throw Exception("Illegal projection type!");
        }
    }

    Matrix4x4 ScannerGeometry::getProjectionMatrix() const
    {
        switch(projectionType)
        {
        case PROJECTION_PARALLEL:
            //return Matrix4x4::ParallelProjection(sourceBase, detectorBase, horizontalPitch, verticalPitch);
            return getParallelProjectionMatrix();
        case PROJECTION_PERSPECTIVE:
            return Matrix4x4::PerspectiveProjection(sourceBase, detectorBase, horizontalPitch, verticalPitch, resolution);
        default:
            throw Exception("Illegal projection type!");
        }
    }

    ScannerGeometry ScannerGeometry::deriveTransformed(const Matrix4x4& transform) const
    {
        ScannerGeometry transformed(TransformCoord(transform, sourceBase),
                                    TransformCoord(transform, detectorBase),
                                    TransformNormal(transform, horizontalPitch),
                                    TransformNormal(transform, verticalPitch),
                                    resolution);
        transformed.setProjectionType(projectionType);
        return transformed;
    }

    float ScannerGeometry::getTraversalLengthThroughBoundingBox(const BoundingBox3f& boundingBox) const
    {
        if(projectionType != PROJECTION_PARALLEL)
        {
            throw Exception("Calculation of traversal length is only supported for parallel scanner geometry!");
        }
        float z_in = (boundingBox.GetMin().z - sourceBase.z) / getRayDirection().z;
        float z_out = (boundingBox.GetMax().z - sourceBase.z) / getRayDirection().z;
        return (std::max(z_in, z_out) - std::min(z_in, z_out));
    }

    Ray ScannerGeometry::getRayThroughPixelCenter(const Vec2ui& pixel) const
    {
        auto pixelCenter = detectorBase + ((float)pixel.x + 0.5f) * horizontalPitch + ((float)pixel.y + 0.5f) * verticalPitch;
        switch(projectionType)
        {
        case PROJECTION_PARALLEL:
            return Ray::FromPointAndDirection(pixelCenter, getRayDirection());
        case PROJECTION_PERSPECTIVE:
            return Ray::FromPoints(sourceBase, pixelCenter);
        default:
            throw Exception("Illegal projection type!");
        }
    }

    bool ScannerGeometry::operator==(const ScannerGeometry& other) const
    {
        return projectionType == other.projectionType &&
            resolution == other.resolution &&
            detectorBase == other.detectorBase &&
            sourceBase == other.sourceBase &&
            horizontalPitch == other.horizontalPitch &&
            verticalPitch == other.verticalPitch;
    }

    bool ScannerGeometry::operator!=(const ScannerGeometry& other) const
    {
        return !(*this == other);
    }

    Matrix4x4 ScannerGeometry::getParallelProjectionMatrix() const
    {
        Matrix4x4 m = Matrix4x4(Vec4f(horizontalPitch, 0.0f),
                                Vec4f(verticalPitch, 0.0f),
                                Vec4f(getRayDirection(), 0.0f),
                                Vec4f(0.0f, 0.0f, 0.0f, 1.0)).inverse();
        Vec2f v = 0.5f * (Vec2f)resolution + Vec2f(0.5f, 0.5f);
        Matrix4x4 coord(Vec4f(1.0f, 0.0f, 0.0f, 0.0f),
                        Vec4f(0.0f, 1.0f, 0.0f, 0.0f),
                        Vec4f(0.0f, 0.0f, 1.0f, 0.0f),
                        Vec4f(v, 0.0f, 1.0f));
        return coord * m;
    }

    void ScannerGeometry::exportGeometryToVisualizer(Visualizer* visualizer) const
    {
        float rayRadius = horizontalPitch.LengthF();
        float sx = (float)resolution.x / 128.0f;
        float sy = (float)resolution.y / 128.0f;
        auto hPitch = horizontalPitch * 128.0f;
        auto vPitch = verticalPitch * 128.0f;
        switch(projectionType)
        {
        case PROJECTION_PARALLEL:
            visualizer->appendGrid(sourceBase, hPitch, vPitch, resolution / 128, Vec4f(0.7f, 1.0f, 0.7f, 1.0f));
            visualizer->appendConnection(sourceBase, detectorBase, 10.0f * rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
            visualizer->appendConnection(sourceBase + sy * vPitch, detectorBase + sy * vPitch, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
            visualizer->appendConnection(sourceBase + sx * hPitch, detectorBase + sx * hPitch, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
            visualizer->appendConnection(sourceBase + sx * hPitch + sy * vPitch, detectorBase + sx * hPitch + sy * vPitch, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
            break;
        case PROJECTION_PERSPECTIVE:
            visualizer->append(getSourceCenter(), rayRadius, Vec4f(0.7f, 1.0f, 0.7f, 1.0f));
            visualizer->appendConnection(getSourceCenter(), detectorBase, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
            visualizer->appendConnection(getSourceCenter(), detectorBase + sy * vPitch, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
            visualizer->appendConnection(getSourceCenter(), detectorBase + sx * hPitch, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
            visualizer->appendConnection(getSourceCenter(), detectorBase + sx * hPitch + sy * vPitch, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
            break;
        default:
            throw Exception("Illegal projection type!");
        }
        visualizer->appendGrid(detectorBase, hPitch, vPitch, resolution / 128, Vec4f(1.0f, 0.7f, 1.0f, 1.0f));
    }
}