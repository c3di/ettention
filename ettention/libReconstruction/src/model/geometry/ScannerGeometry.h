#pragma once
#pragma once
#include "framework/geometry/BoundingBox.h"
#include "framework/geometry/Ray.h"
#include "framework/math/Matrix4x4.h"
#include "framework/math/Vec2.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    class Image;
    class ImageStackDatasource;
    class Visualizer;
	class VariableContainer;

    class ScannerGeometry
    {
    public:
		ScannerGeometry();
		ScannerGeometry(const ScannerGeometry* other);
		ScannerGeometry(ImageStackDatasource* inputStack);
        ScannerGeometry(const Vec2ui& resolution);
        ScannerGeometry(const Vec3f& sourceBase, const Vec3f& detectorBase, const Vec3f& horizontalPitch, const Vec3f& verticalPitch, const Vec2ui& resolution);
        virtual ~ScannerGeometry();

		virtual Vec3f getSourceCenter() const = 0;
		virtual Vec3f getRayDirection() const = 0;
		virtual Matrix4x4 getProjectionMatrix() const = 0;
		virtual float getTraversalLengthThroughBoundingBox(const BoundingBox3f& boundingBox) const = 0;
		virtual Ray getRayThroughPixelCenter(const Vec2ui& pixel) const = 0;
		virtual void exportGeometryToVisualizer(Visualizer* visualizer, const Image* projectionImage) const = 0;
		virtual ScannerGeometry* deriveTransformed(const Matrix4x4& transform) const = 0;
		virtual ScannerGeometry* clone() const = 0;
		virtual void addToVariableContainer(VariableContainer* container) const;

		virtual void set(const Vec3f& sourceBase, const Vec3f& detectorBase, const Vec3f& horizontalPitch, const Vec3f& verticalPitch);
		virtual const Vec3f& getSourceBase() const;
		virtual const Vec3f& getDetectorBase() const;
		virtual Vec3f getDetectorCenter() const;
		virtual const Vec3f& getHorizontalPitch() const;
		virtual const Vec3f& getVerticalPitch() const;
		virtual void setResolution(const Vec2ui& resolution);
		virtual const Vec2ui& getResolution() const;

    protected:
        Vec2ui resolution;
        Vec3f sourceBase;
        Vec3f detectorBase;
        Vec3f horizontalPitch;
        Vec3f verticalPitch;
    };
}