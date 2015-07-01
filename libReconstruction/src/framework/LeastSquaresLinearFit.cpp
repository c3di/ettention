#include "stdafx.h"

#include "framework/LeastSquaresLinearFit.h"

namespace ettention
{
    LeastSquaresLinearFit::LeastSquaresLinearFit(const std::vector<Vec2f>& points)
        : points(points)
    {
        computeFit();
    }

    LeastSquaresLinearFit::~LeastSquaresLinearFit()
    {
    }

    void LeastSquaresLinearFit::computeFit()
    {
        double xavg = 0.0;
        double yavg = 0.0;
        double sx = 0.0;
        double sy = 0.0;
        double sxy = 0.0;

        for(size_t i = 0; i < points.size(); i++)
        {
            xavg += points[i].x;
            yavg += points[i].y;
        }
        xavg /= (float)points.size();
        yavg /= (float)points.size();

        for(size_t i = 0; i < points.size(); i++)
        {
            sx += (points[i].x - xavg) * (points[i].x - xavg);
            sy += (points[i].y - yavg) * (points[i].y - yavg);
            sxy += (points[i].x - xavg) * (points[i].y - yavg);
        }

        slope = (float)(sxy / sx);
        yAxisIntersection = (float)(yavg - slope*xavg);
    }

    float LeastSquaresLinearFit::getYAxisIntersection()
    {
        return yAxisIntersection;
    }

    float LeastSquaresLinearFit::getSlope()
    {
        return slope;
    }

    float LeastSquaresLinearFit::remainingErrorOfPoint(Vec2f p)
    {
        float error = p.y - linearModelValueAt(p.x);
        return error*error;
    }

    float LeastSquaresLinearFit::linearModelValueAt(float x)
    {
        return yAxisIntersection + x * slope;
    }

    float LeastSquaresLinearFit::totalVarianceOfValues()
    {
        float avg = 0.0f;
        for(size_t i = 0; i < points.size(); i++)
            avg += points[i].y;
        avg /= (float)points.size();

        float totalVariance = 0.0f;
        for(size_t i = 0; i < points.size(); i++)
        {
            float error = points[i].y - avg;
            totalVariance += error*error;
        }
        return totalVariance;
    }
}