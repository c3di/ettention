#ifndef GENERATE_CIRCLE_CL
#define GENERATE_CIRCLE_CL

__kernel void generate(int resolutionX, int resolutionY, __global float* resultImage, float radius, float outsideIntensity, float insideIntensity)
{
	// integer voxel coordinates
	const unsigned int x = get_global_id(0);
    const unsigned int y = get_global_id(1);	

	const float2 imageCenter   = (float2) (resolutionX, resolutionY) / 2.0f;
	const float2 pixelPosition = (float2) (x, y);

	const float2 relativePixelPos = pixelPosition - imageCenter;

	int tx = x + (resolutionX/2);
	if (tx >= resolutionX)
		tx -= resolutionX;
    int ty = y + (resolutionY/2);
	if (ty >= resolutionY)
		ty -= resolutionY;
	const int index = ty * resolutionX + tx;								

	const float distanceToOriginSQR = relativePixelPos.x * relativePixelPos.x + relativePixelPos.y * relativePixelPos.y;

	const float innerRadius = (radius - 1.0f);
	if (distanceToOriginSQR < innerRadius * innerRadius) 
	{
		resultImage[index] = insideIntensity;
		return;
	}

	const float outerRadius = (radius + 1.0f);
	if (distanceToOriginSQR > outerRadius * outerRadius) 
	{
		resultImage[index] = outsideIntensity;
		return;
	}

	resultImage[index] = 0.0f;
	for (float dy = -0.45f; dy < 0.55f; dy += 0.1f)
		for (float dx = -0.45f; dx < 0.55f; dx += 0.1f) {
			
			const float samplePosX = relativePixelPos.x + dx;
			const float samplePosY = relativePixelPos.y + dy;
			if (samplePosX * samplePosX + samplePosY * samplePosY < radius * radius)
			{
				resultImage[index] += insideIntensity;
			}
			else
			{
				resultImage[index] += outsideIntensity;
			}
		}
	resultImage[index] /= 100.0f;
}

#endif