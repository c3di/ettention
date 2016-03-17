__kernel void computeGlobalMinimumPosition(__global float *source, __global float *resultValue, __global int *resultPosition, int taskSize, __local float *minValuesBuffer, __local int *minPositionsBuffer)
{
	// size of source : taskSize
	// size of resultValue,Position : numberOfGroups = taskSize / groupSize + [0|1]
	// size of minValuesBuffer : groupSize
	const uint totalWorkers = get_global_size(0);	// Not PoT, i.e. 1000 for input size of 2000
	const uint groupSize 	= get_local_size(0);	// PoT, i.e. 256
	const uint workerGId 	= get_global_id(0);		// Each worker takes care of 2 input floats
	const uint workerLId 	= get_local_id(0);
	const uint groupId		= get_group_id(0);		// To know which part of output array to use

	int positionFirst = workerGId;
	int positionSecond = workerGId + totalWorkers;
	if( positionSecond > taskSize ) // if taskSize was odd, last data pair has to be padded
		positionSecond = positionFirst;

	float tempValue1 = source[positionFirst];
	float tempValue2 = source[positionSecond];
	
	int positionOfMin = positionFirst;
	float valueOfMin = tempValue1;
	// Copy part of global array into local memory, doing comparison meanwhile
	if( (tempValue2 < tempValue1) && (!isnan(tempValue2)) )
	{
		positionOfMin = positionSecond;
		valueOfMin = tempValue2;
	}

	minPositionsBuffer[workerLId] = positionOfMin;
	minValuesBuffer[workerLId] = valueOfMin;
	barrier(CLK_GLOBAL_MEM_FENCE);

	uint uid = workerLId;
	uint powerOfTwo = 1;
	uint posLeft;
	uint posRight;

	for(uint stop = groupSize; stop > 1; stop /= 2)
	{
		if( (uid % 2) != 0 )
		{
			return;
		}
		barrier(CLK_GLOBAL_MEM_FENCE);

		posLeft 	= workerLId;
		posRight 	= workerLId + powerOfTwo;

		if( (posRight < groupSize) && (minValuesBuffer[posRight] < minValuesBuffer[posLeft]) && (!isnan(minValuesBuffer[posRight])) )
		{
			minPositionsBuffer[posLeft] = minPositionsBuffer[posRight];
			minValuesBuffer[posLeft] = minValuesBuffer[posRight];
		}

		uid /= 2;
		powerOfTwo *= 2;
	}
	
	barrier(CLK_GLOBAL_MEM_FENCE);
	resultPosition[groupId] = minPositionsBuffer[0];
	resultValue[groupId] 	= minValuesBuffer[0];	// Leftmost element is our goal element
}