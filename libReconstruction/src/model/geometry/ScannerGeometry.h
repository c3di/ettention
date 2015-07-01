#pragma once
#include "framework/geometry/BoundingBox.h"
#include "framework/geometry/Ray.h"
#include "framework/math/Matrix4x4.h"
#include "framework/math/Vec2.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    class Visualizer;
    class ImageStackDatasource;

    class ScannerGeometry
    {
    public:
        enum ProjectionType
        {
            PROJECTION_UNKNOWN,
            PROJECTION_PARALLEL,
            PROJECTION_PERSPECTIVE,
        };

        ScannerGeometry();
        ScannerGeometry(ImageStackDatasource* inputStack);
        ScannerGeometry(ProjectionType projectionType, const Vec2ui& resolution);
        ScannerGeometry(const Vec3f& sourceBase, const Vec3f& detectorBase, const Vec3f& horizontalPitch, const Vec3f& verticalPitch, const Vec2ui& resolution);
        ~ScannerGeometry();

        void set(const Vec3f& sourceBase, const Vec3f& detectorBase, const Vec3f& horizontalPitch, const Vec3f& verticalPitch);
        const Vec3f& getSourceBase() const;
        Vec3f getSourceCenter() const;
        const Vec3f& getDetectorBase() const;
        Vec3f getDetectorCenter() const;
        const Vec3f& getHorizontalPitch() const;
        const Vec3f& getVerticalPitch() const;
        Matrix4x4 getProjectionMatrix() const;
        ScannerGeometry deriveTransformed(const Matrix4x4& transform) const;
        void setProjectionType(ProjectionType projectionType);
        ProjectionType getProjectionType() const;
        void setResolution(const Vec2ui& resolution);
        const Vec2ui& getResolution() const;
        Vec3f getRayDirection() const;
        float getTraversalLengthThroughBoundingBox(const BoundingBox3f& boundingBox) const;
        Ray getRayThroughPixelCenter(const Vec2ui& pixel) const;
        bool operator==(const ScannerGeometry& other) const;
        bool operator!=(const ScannerGeometry& other) const;
        void exportGeometryToVisualizer(Visualizer* visualizer) const;

    private:
        ProjectionType projectionType;
        Vec2ui resolution;
        Vec3f sourceBase;
        Vec3f detectorBase;
        Vec3f horizontalPitch;
        Vec3f verticalPitch;

        Matrix4x4 getParallelProjectionMatrix() const;
    };
}