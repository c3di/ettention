#include "stdafx.h"

#include "Float16Compressor.h"

namespace ettention
{
    half_float_t Float16Compressor::_compress(float value)
    {
        Bits v, s;
        v.f = value;
        uint32_t sign = v.si & signN;
        v.si ^= sign;
        sign >>= shiftSign; // logical shift
        s.si = mulN;
        s.si = (int32_t)(s.f * v.f); // correct subnormals
        v.si ^= (s.si ^ v.si) & -(minN > v.si);
        v.si ^= (infN ^ v.si) & -((infN > v.si) & (v.si > maxN));
        v.si ^= (nanN ^ v.si) & -((nanN > v.si) & (v.si > infN));
        v.ui >>= shift; // logical shift
        v.si ^= ((v.si - maxD) ^ v.si) & -(v.si > maxC);
        v.si ^= ((v.si - minD) ^ v.si) & -(v.si > subC);
        return v.ui | sign;
    }

    float Float16Compressor::_decompress(half_float_t value)
    {
        Bits v;
        v.ui = value;
        int32_t sign = v.si & signC;
        v.si ^= sign;
        sign <<= shiftSign;
        v.si ^= ((v.si + minD) ^ v.si) & -(v.si > subC);
        v.si ^= ((v.si + maxD) ^ v.si) & -(v.si > maxC);
        Bits s;
        s.si = mulC;
        s.f *= v.si;
        int32_t mask = -(norC > v.si);
        v.si <<= shift;
        v.si ^= (s.si ^ v.si) & mask;
        v.si |= sign;
        return v.f;
    }

    half_float_t Float16Compressor::compress(float value)
    {
        return _compress(value);
    }

    float Float16Compressor::decompress(half_float_t value)
    {
        return _decompress(value);
    }


    void Float16Compressor::compress(const float* input, half_float_t* output, size_t count)
    {
        for(size_t i = 0; i < count; i++)
        {
            *output = _compress(*input);
            input++;
            output++;
        }
    }

    void Float16Compressor::decompress(half_float_t* input, float* output, size_t count)
    {
        for(size_t i = 0; i < count; i++)
        {
            *output = _decompress(*input);
            input++;
            output++;
        }
    }
}