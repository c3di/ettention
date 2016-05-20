#include "stdafx.h"

#include "ParallelScannerGeometry.h"
#include "ScannerGeometry.h"
#include "framework/geometry/Visualizer.h"
#include "io/datasource/ImageStackDatasource.h"

namespace ettention
{
    ParallelScannerGeometry::ParallelScannerGeometry()
        : ScannerGeometry()
    {
    }

    ParallelScannerGeometry::ParallelScannerGeometry(const ParallelScannerGeometry* other)
        : ScannerGeometry(other)
    {
    }

    ParallelScannerGeometry::ParallelScannerGeometry(ImageStackDatasource* inputStack)
        : ScannerGeometry(inputStack)
    {
    }

    ParallelScannerGeometry::ParallelScannerGeometry(const Vec2ui& resolution)
        : ScannerGeometry(resolution)
    {
    }

    ParallelScannerGeometry::ParallelScannerGeometry(const Vec3f& sourceBase, const Vec3f& detectorBase, const Vec3f& horizontalPitch, const Vec3f& verticalPitch, const Vec2ui& resolution)
        : ScannerGeometry(sourceBase, detectorBase, horizontalPitch, verticalPitch, resolution)
    {
    }

    ParallelScannerGeometry::~ParallelScannerGeometry()
    {
    }

    Vec3f ParallelScannerGeometry::getSourceCenter() const
    {
        return sourceBase + 0.5f * ((float)resolution.x * horizontalPitch + (float)resolution.y * verticalPitch);
    }

    Vec3f ParallelScannerGeometry::getRayDirection() const
    {
        return doNormalize(detectorBase - sourceBase);
    }

    Matrix4x4 ParallelScannerGeometry::getProjectionMatrix() const
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

    float ParallelScannerGeometry::getTraversalLengthThroughBoundingBox(const BoundingBox3f& boundingBox) const
    {
        float z_in = (boundingBox.getMin().z - sourceBase.z) / getRayDirection().z;
        float z_out = (boundingBox.getMax().z - sourceBase.z) / getRayDirection().z;
        return (std::max(z_in, z_out) - std::min(z_in, z_out));
    }

    Ray ParallelScannerGeometry::getRayThroughPixelCenter(const Vec2ui& pixel) const
    {
        auto pixelCenter = detectorBase + ((float)pixel.x + 0.5f) * horizontalPitch + ((float)pixel.y + 0.5f) * verticalPitch;
        return Ray::createFromPointAndDirection(pixelCenter, getRayDirection());
    }

    ScannerGeometry* ParallelScannerGeometry::deriveTransformed(const Matrix4x4& transform) const
    {
        auto transformed = new ParallelScannerGeometry( transformCoord(transform, sourceBase),
                                                        transformCoord(transform, detectorBase),
                                                        transformNormal(transform, horizontalPitch),
                                                        transformNormal(transform, verticalPitch),
                                                        resolution);
        return transformed;
    }

    ScannerGeometry* ParallelScannerGeometry::clone() const
    {
        return new ParallelScannerGeometry(this);
    }

    void ParallelScannerGeometry::exportGeometryToVisualizer(Visualizer* visualizer, const Image* projectionImage) const
    {
        float rayRadius = horizontalPitch.getLengthF();
        float sx = (float)resolution.x / 128.0f;
        float sy = (float)resolution.y / 128.0f;
        auto hPitch = horizontalPitch * 128.0f;
        auto vPitch = verticalPitch * 128.0f;
 
        if(projectionImage)
        {
            // visualizer->appendTexture(sourceBase, horizontalPitch, verticalPitch, projectionImage, false);
        }
        else
        {
            visualizer->appendGrid(sourceBase, hPitch, vPitch, resolution / 128, Vec4f(0.7f, 1.0f, 0.7f, 1.0f));
        }
        visualizer->appendConnection(sourceBase, detectorBase, 10.0f * rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
        visualizer->appendConnection(sourceBase + sy * vPitch, detectorBase + sy * vPitch, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
        visualizer->appendConnection(sourceBase + sx * hPitch, detectorBase + sx * hPitch, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
        visualizer->appendConnection(sourceBase + sx * hPitch + sy * vPitch, detectorBase + sx * hPitch + sy * vPitch, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));

        if ( projectionImage )
        {
            visualizer->appendTexture(detectorBase, horizontalPitch, verticalPitch, projectionImage, false);
        }
        else
        {
            visualizer->appendGrid(detectorBase, hPitch, vPitch, resolution / 128, Vec4f(1.0f, 0.7f, 1.0f, 1.0f));
        }
    }
}