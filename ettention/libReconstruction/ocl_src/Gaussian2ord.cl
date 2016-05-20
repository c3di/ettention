__kernel void computeSecondOrderDirectionalGaussian(__global float *GsXX, __global float *GsXY, __global float *GsYY, int sizeX, int sizeY, float sigma, int particleDiameter)
{
	int x = get_global_id(0);
	int y = get_global_id(1);

	const int i = y * sizeX + x;

	x -= sizeX / 2;
	y -= sizeY / 2;

	__local float s2;
	__local float divr;

	GsXX[i] = 42;
	GsXY[i] = 42;
	GsYY[i] = 42;

	float x2 = x * x;
	float y2 = y * y;

	s2 = sigma * sigma;
	divr = 2 * M_PI_F * s2 * s2 * s2;

	float exponent = exp(-(x2 + y2) / (2 * s2));

	GsXX[i] = ((x2 - s2) / divr) * exponent;
	GsXY[i] = ((x  *  y) / divr) * exponent;
	GsYY[i] = ((y2 - s2) / divr) * exponent;
}