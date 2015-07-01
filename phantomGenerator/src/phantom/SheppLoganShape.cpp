#include "stdafx.h"
#include "SheppLoganShape.h"
#include "EllipsoidShape.h"
#include <framework/NumericalAlgorithms.h>
#include <model/volume/Volume.h>

namespace ettention
{
    SheppLoganShape::SheppLoganShape(Volume* volume, bool generateMaskOnly)
        : ComposedShape(volume, Matrix4x4::identity)
    {
        scale = Vec3f((float)volume->Properties().GetVolumeResolution().x, (float)volume->Properties().GetVolumeResolution().y, (float)volume->Properties().GetVolumeResolution().z);

        if(generateMaskOnly)
        {
            // those magic numbers are taken straight from the paper-definition of the shepp logan phantom
            // in case a mask (only outer contour) is required, only one shape is added
            addEllipsoid(1.0f, 0.6900f, 0.920f, 0.810f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        }
        else
        {
            // those magic numbers are taken straight from the paper-definition of the shepp logan phantom
            addEllipsoid(1.0f, 0.6900f, 0.920f, 0.810f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            addEllipsoid(-0.8f, 0.6624f, 0.874f, 0.780f, 0.0f, -0.0184f, 0.0f, 0.0f, 0.0f, 0.0f);

            addEllipsoid(-0.2f, 0.1100f, 0.310f, 0.220f, 0.22f, 0.0f, 0.0f, 18.0f, 10.0f, 0.0f);
            addEllipsoid(-0.2f, 0.1600f, 0.410f, 0.280f, -0.22f, 0.0f, 0.0f, -18.0f, 10.0f, 0.0f);

            addEllipsoid(0.1f, 0.2100f, 0.250f, 0.410f, 0.0f, 0.35f, -0.15f, 0.0f, 0.0f, 0.0f);
            addEllipsoid(0.1f, 0.0460f, 0.046f, 0.050f, 0.0f, 0.1f, 0.25f, 0.0f, 0.0f, 0.0f);
            addEllipsoid(0.1f, 0.0460f, 0.046f, 0.050f, 0.0f, -0.1f, 0.25f, 0.0f, 0.0f, 0.0f);

            addEllipsoid(0.1f, 0.0460f, 0.023f, 0.050f, -0.08f, -0.605f, 0.0f, 0.0f, 0.0f, 0.0f);
            addEllipsoid(0.1f, 0.0230f, 0.023f, 0.020f, 0.0f, -0.606f, 0.0f, 0.0f, 0.0f, 0.0f);
            addEllipsoid(0.1f, 0.0230f, 0.046f, 0.020f, 0.06f, -0.605f, 0.0f, 0.0f, 0.0f, 0.0f);
        }
    }

    SheppLoganShape::~SheppLoganShape()
    {
    }

    void SheppLoganShape::addEllipsoid(float intensity, float a, float b, float c, float x0, float y0, float z0, float phiInDegree, float thetaInDegree, float psiInDegree)
    {
        a *= scale.x / 2.0f;
        b *= scale.y / 2.0f;
        c *= scale.z / 2.0f;
        x0 = x0 * (scale.x / 2.0f) + (scale.x / 2.0f);
        y0 = y0 * (scale.y / 2.0f) + (scale.x / 2.0f);
        z0 = z0 * (scale.z / 2.0f) + (scale.x / 2.0f);

        const float phiInRadians = NumericalAlgorithms::degreeToRadians(phiInDegree);
        const float thetaInRadians = NumericalAlgorithms::degreeToRadians(thetaInDegree);
        const float psiInRadians = NumericalAlgorithms::degreeToRadians(psiInDegree);

        Matrix4x4 rotation = Matrix4x4::rotationMatrix(phiInRadians, thetaInRadians, psiInRadians);
        Matrix4x4 translation = Matrix4x4::translationMatrix(Vec3f(x0, y0, z0));
        Matrix4x4 transform = multiply(translation, rotation);

        addShape(new EllipsoidShape(getVolume(), transform, a, b, c, intensity));
    }
}