#include "stdafx.h"
#include "BlobParameters.h"
#include "framework/NumericalAlgorithms.h"

namespace ettention
{
    BlobParameters::BlobParameters(float supportRadius, unsigned int degree, float densityDropOff):
        supportRadius(supportRadius), degree(degree), densityDropOff(densityDropOff)
    {

    }

    BlobParameters BlobParameters::createDefault()
    {
        return BlobParameters(2.0f, 2, 3.6f);
    }

    BlobParameters BlobParameters::createWithParameters(float supportRadius, unsigned int degree, float densityDropOff)
    {
        return BlobParameters(supportRadius, degree, densityDropOff);
    }
    
    float BlobParameters::getSupportRadius() const
    {
        return supportRadius;
    }

    unsigned int BlobParameters::getDegree() const
    {
        return degree;
    }

    float BlobParameters::getDensityFallOff() const
    {
        return densityDropOff;
    }

#define NUMBER_OF_MODIFIED_BESSEL_SUMMANDS 40
    double BlobParameters::evaluate(double x) const
    {
        // implementing R.M. Lewitt, Phys. Med. Biol. 37 (1992) 708
        double a = getSupportRadius();
        unsigned int m = getDegree();
        double alpha = getDensityFallOff();
        if(0 <= x && x <= a)
        {
            double rootOfOneMinusROverAlphaSquared = sqrt(1.0 - (x * x) / (a * a));
            double firstIm = evaluateModifiedBessel(alpha, NUMBER_OF_MODIFIED_BESSEL_SUMMANDS);
            double secondEvalParam = alpha * rootOfOneMinusROverAlphaSquared;
            double secondIm = evaluateModifiedBessel(secondEvalParam, NUMBER_OF_MODIFIED_BESSEL_SUMMANDS);
            return 1.0 / firstIm * NumericalAlgorithms::uintPow(rootOfOneMinusROverAlphaSquared, m) * secondIm;
        }
        else
        {
            return 0.0;
        }
    }

    double BlobParameters::evaluateModifiedBessel(double x, unsigned int numberOfSummands) const
    {
        // implementing http://en.wikipedia.org/wiki/Bessel_function#Modified_Bessel_functions_:_I.CE.B1.2C_K.CE.B1
        double sum = 0.0;
        unsigned int alpha = getDegree();
        double summand = NumericalAlgorithms::uintPow(0.5 * x, alpha) / (double)NumericalAlgorithms::factorial(alpha);
        for(unsigned int m = 0; m < numberOfSummands; ++m)
        {
            sum += summand;
            summand *= 0.25 * x * x / (double)((m + 1) * (m + alpha + 1));
        }
        return sum;
    }
}