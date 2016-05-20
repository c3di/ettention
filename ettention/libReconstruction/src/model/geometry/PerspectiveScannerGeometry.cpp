#include "stdafx.h"

#include "PerspectiveScannerGeometry.h"
#include "framework/geometry/Visualizer.h"

namespace ettention
{
	PerspectiveScannerGeometry::PerspectiveScannerGeometry()
		: ScannerGeometry()
	{
	}

	PerspectiveScannerGeometry::PerspectiveScannerGeometry(const PerspectiveScannerGeometry* other)
		: ScannerGeometry(other)
	{
	}

	PerspectiveScannerGeometry::PerspectiveScannerGeometry(ImageStackDatasource* inputStack)
		: ScannerGeometry(inputStack)
	{
	}

	PerspectiveScannerGeometry::PerspectiveScannerGeometry(const Vec2ui& resolution)
		: ScannerGeometry(resolution)
	{
	}

	PerspectiveScannerGeometry::PerspectiveScannerGeometry(const Vec3f& sourceBase, const Vec3f& detectorBase, const Vec3f& horizontalPitch, const Vec3f& verticalPitch, const Vec2ui& resolution)
		: ScannerGeometry(sourceBase, detectorBase, horizontalPitch, verticalPitch, resolution)
	{
	}

	PerspectiveScannerGeometry::~PerspectiveScannerGeometry()
	{
	}

	Vec3f PerspectiveScannerGeometry::getSourceCenter() const
    {
		return sourceBase;
    }

	Vec3f PerspectiveScannerGeometry::getRayDirection() const
    {
		return doNormalize(getDetectorCenter() - sourceBase);
    }

	Matrix4x4 PerspectiveScannerGeometry::getProjectionMatrix() const
    {
		return Matrix4x4::getPerspectiveProjection(sourceBase, resolution, detectorBase, horizontalPitch, verticalPitch);
    }

	ScannerGeometry* PerspectiveScannerGeometry::deriveTransformed(const Matrix4x4& transform) const
    {
		auto transformed = new PerspectiveScannerGeometry( transformCoord(transform, sourceBase),
														   transformCoord(transform, detectorBase),
														   transformNormal(transform, horizontalPitch),
														   transformNormal(transform, verticalPitch),
														   resolution);
        return transformed;
    }

	ScannerGeometry* PerspectiveScannerGeometry::clone() const
	{
		return new PerspectiveScannerGeometry(this);
	}

	float PerspectiveScannerGeometry::getTraversalLengthThroughBoundingBox(const BoundingBox3f& boundingBox) const
    {
		throw Exception("Calculation of traversal length is only supported for parallel scanner geometry!");
    }

	Ray PerspectiveScannerGeometry::getRayThroughPixelCenter(const Vec2ui& pixel) const
    {
        auto pixelCenter = detectorBase + ((float)pixel.x + 0.5f) * horizontalPitch + ((float)pixel.y + 0.5f) * verticalPitch;
        return Ray::createFromPoints(sourceBase, pixelCenter);
    }

	void PerspectiveScannerGeometry::exportGeometryToVisualizer(Visualizer* visualizer, const Image* projectionImage) const
    {
        float rayRadius = horizontalPitch.getLengthF();
        float sx = (float)resolution.x / 128.0f;
        float sy = (float)resolution.y / 128.0f;
        auto hPitch = horizontalPitch * 128.0f;
        auto vPitch = verticalPitch * 128.0f;
        visualizer->append(getSourceCenter(), rayRadius, Vec4f(0.7f, 1.0f, 0.7f, 1.0f));
        visualizer->appendConnection(getSourceCenter(), detectorBase, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
        visualizer->appendConnection(getSourceCenter(), detectorBase + sy * vPitch, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
        visualizer->appendConnection(getSourceCenter(), detectorBase + sx * hPitch, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
        visualizer->appendConnection(getSourceCenter(), detectorBase + sx * hPitch + sy * vPitch, rayRadius, Vec4f(1.0f, 1.0f, 0.7f, 1.0f));
        if(projectionImage)
        {
            visualizer->appendTexture(detectorBase, horizontalPitch, verticalPitch, projectionImage, false);
        }
        else
        {
            visualizer->appendGrid(detectorBase, hPitch, vPitch, resolution / 128, Vec4f(1.0f, 0.7f, 1.0f, 1.0f));
        }
    }
}