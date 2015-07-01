#pragma once

namespace ettention
{
    class BlobParameters
    {
    public:
        static BlobParameters CreateDefault();
        static BlobParameters CreateWithParameters(float supportRadius, unsigned int degree, float densityDropOff);
        ~BlobParameters() {}

        float GetSupportRadius() const { return supportRadius; }
        unsigned int GetDegree() const { return degree; }
        float GetDensityFallOff() const { return densityDropOff; }
        double Evaluate(double x) const;

    private:
        float supportRadius;
        unsigned int degree;
        float densityDropOff;

        BlobParameters(float supportRadius, unsigned int degree, float densityDropOff);

        double EvaluateModifiedBessel(double x, unsigned int numberOfSummands) const;
    };
}