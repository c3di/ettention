#pragma once

#include "ScannerGeometry.h"

namespace ettention
{
    class Image;
    class ImageStackDatasource;
    class Visualizer;

    class PerspectiveScannerGeometry : public ScannerGeometry
    {
	public:
		PerspectiveScannerGeometry();
		PerspectiveScannerGeometry(const PerspectiveScannerGeometry* other);
		PerspectiveScannerGeometry(ImageStackDatasource* inputStack);
		PerspectiveScannerGeometry(const Vec2ui& resolution);
		PerspectiveScannerGeometry(const Vec3f& sourceBase, const Vec3f& detectorBase, const Vec3f& horizontalPitch, const Vec3f& verticalPitch, const Vec2ui& resolution);
		virtual ~PerspectiveScannerGeometry();

		virtual Vec3f getSourceCenter() const override;
		virtual Vec3f getRayDirection() const override;
		virtual Matrix4x4 getProjectionMatrix() const override;
		virtual float getTraversalLengthThroughBoundingBox(const BoundingBox3f& boundingBox) const override;
		virtual Ray getRayThroughPixelCenter(const Vec2ui& pixel) const override;
		virtual ScannerGeometry* deriveTransformed(const Matrix4x4& transform) const override;
		virtual ScannerGeometry* clone() const override;

		virtual void exportGeometryToVisualizer(Visualizer* visualizer, const Image* projectionImage) const;
	};
}