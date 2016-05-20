__kernel void computeGlobalMaximumPosition(__global float *source, __global float *resultValue, __global int *resultPosition, int taskSize, __local float *maxValuesBuffer, __local int *maxPositionsBuffer)
{
    // size of source : taskSize
    // size of resultValue,Position : numberOfGroups = taskSize / groupSize + [0|1]
    // size of maxValuesBuffer : groupSize
    const uint totalWorkers = get_global_size(0);	// Not PoT, i.e. 1000 for input size of 2000
    const uint groupSize 	= get_local_size(0);	// PoT, i.e. 256
    const uint workerGId 	= get_global_id(0);		// Each worker takes care of 2 input floats
    const uint workerLId 	= get_local_id(0);
    const uint groupId		= get_group_id(0);		// To know which part of output array to use

    int positionFirst = workerGId;
    int positionSecond = workerGId + totalWorkers;
    if( positionSecond>= taskSize ) // if taskSize was odd, last data pair has to be padded
        positionSecond = positionFirst;

    float tempValue1 = source[positionFirst];
    float tempValue2 = source[positionSecond];
    
    int positionOfMax = positionFirst;
    float valueOfMax = tempValue1;
    // Copy part of global array into local memory, doing comparison meanwhile
    if(!isnan(tempValue2) && (isnan(tempValue1) || (!isnan(tempValue1) && tempValue2 > tempValue1)))
    {
        positionOfMax = positionSecond;
        valueOfMax = tempValue2;
    }

    maxPositionsBuffer[workerLId] = positionOfMax;
    maxValuesBuffer[workerLId] = valueOfMax;
    barrier(CLK_GLOBAL_MEM_FENCE);

    uint uid = workerLId;
    uint powerOfTwo = 1;
    uint posLeft;
    uint posRight;

    for(uint stop = groupSize; stop >= 1; stop /= 2)
    {
        if( (uid % 2) != 0 )
        {
            return;
        }

        posLeft 	= workerLId;
        posRight 	= workerLId + powerOfTwo;

        if(posRight < groupSize && !isnan(maxValuesBuffer[posRight]) && (isnan(maxValuesBuffer[posLeft]) || maxValuesBuffer[posRight] > maxValuesBuffer[posLeft]))
        {
            maxPositionsBuffer[posLeft] = maxPositionsBuffer[posRight];
            maxValuesBuffer[posLeft] = maxValuesBuffer[posRight];
        }
        barrier(CLK_GLOBAL_MEM_FENCE);

        uid /= 2;
        powerOfTwo *= 2;
    }
    
    barrier(CLK_GLOBAL_MEM_FENCE);
    resultPosition[groupId] = maxPositionsBuffer[0];
    resultValue[groupId] 	= maxValuesBuffer[0];	// Leftmost element is our goal element
}