#pragma once

namespace ettention
{
    class BlobParameters
    {
    public:
        static BlobParameters createDefault();
        static BlobParameters createWithParameters(float supportRadius, unsigned int degree, float densityDropOff);

        ~BlobParameters() {}

        float getSupportRadius() const;
        unsigned int getDegree() const;
        float getDensityFallOff() const;
        double evaluate(double x) const;

    private:
        float supportRadius;
        unsigned int degree;
        float densityDropOff;

        BlobParameters(float supportRadius, unsigned int degree, float densityDropOff);

        double evaluateModifiedBessel(double x, unsigned int numberOfSummands) const;
    };
}