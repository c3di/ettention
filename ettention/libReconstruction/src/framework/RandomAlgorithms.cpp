#include "stdafx.h"

#include "framework/RandomAlgorithms.h"

#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "model/image/Image.h"

namespace ettention 
{
    void RandomAlgorithms::generateRandomNumbersFromMinusOneToOne(unsigned int amount, std::vector<float>& randomValues, unsigned int seed)
    {
        randomValues.resize(amount);

        typedef boost::variate_generator<boost::minstd_rand &, boost::uniform_real<>> gen_type;
        boost::minstd_rand generator(seed);
        gen_type die_gen(generator, boost::uniform_real<>(-1.0, 1.0));

        boost::generator_iterator<gen_type> die(&die_gen);
        for(unsigned int i = 0; i < amount; i++)
            randomValues[i] = (float)*die++;
    }

    std::vector<float> RandomAlgorithms::generateRandomNumbersFromMinusOneToOne(unsigned int amount, unsigned int seed)
    {
        std::vector<float> randomValues(amount);

        generateRandomNumbersFromMinusOneToOne(amount, randomValues);

        return randomValues;
    }

    void RandomAlgorithms::generateRandomNumbersFromZeroToMax(unsigned int amount, unsigned int maxValue, std::vector<unsigned int>& randomValues, unsigned int seed)
    {
        randomValues.resize(amount);

        typedef boost::variate_generator<boost::minstd_rand &, boost::uniform_int<>> gen_type;
        boost::minstd_rand generator(seed);
        gen_type die_gen(generator, boost::uniform_int<>(0, maxValue));

        boost::generator_iterator<gen_type> die(&die_gen);
        for(unsigned int i = 0; i < amount; i++)
            randomValues[i] = (unsigned int)*die++;
    }

    void RandomAlgorithms::generateRandomPointsFromZeroToMax(unsigned int amount, Vec2ui maxValue, std::vector<Vec2ui>& randomValues, unsigned int seed)
    {
        randomValues.resize(amount);

        typedef boost::variate_generator<boost::minstd_rand &, boost::uniform_int<>> gen_type;
        boost::minstd_rand generator(seed);
        gen_type die_genX(generator, boost::uniform_int<>(0, maxValue.x));
        gen_type die_genY(generator, boost::uniform_int<>(0, maxValue.y));

        boost::generator_iterator<gen_type> dieX(&die_genX);
        boost::generator_iterator<gen_type> dieY(&die_genY);
        for(unsigned int i = 0; i < amount; i++)
        {
            const unsigned int a = (unsigned int)*dieX++;
            const unsigned int b = (unsigned int)*dieY++;
            randomValues[i] = Vec2ui(a,b);
        }
    }

    std::vector<Vec3f> RandomAlgorithms::generateRandomUnitVectors(unsigned int amount, unsigned int seed)
    {
        std::vector<Vec3f> randomValues(amount);

        typedef boost::variate_generator<boost::minstd_rand &, boost::uniform_real<>> gen_type;
        boost::minstd_rand generator(seed);
        gen_type die_gen(generator, boost::uniform_real<>(0.0, 1.0));

        boost::generator_iterator<gen_type> die(&die_gen);
        for(unsigned int i = 0; i < amount; i++)
            randomValues[i] = Vec3f((float) *die++, (float) *die++, (float) *die++);

        return randomValues;
    }

    std::vector<unsigned int> RandomAlgorithms::generateRandomPermutationSequence(unsigned int sequenceLength, unsigned int seed)
    {

        struct permutation_function : std::unary_function<unsigned, unsigned> {
            boost::mt19937 &_state;
            unsigned operator()(unsigned i)
            {
                boost::uniform_int<> rng(0, i - 1);
                return rng(_state);
            }
            permutation_function(boost::mt19937 &state) : _state(state) {}
        };

        boost::random::mt19937 rng;
        permutation_function fct(rng);
        std::vector<unsigned int> sequence(sequenceLength);
        for(unsigned int i = 0; i < sequenceLength; ++i) 
            sequence[i] = i;

        std::random_shuffle(sequence.begin(), sequence.end(), fct);
        return sequence;
    }

    Image* RandomAlgorithms::generateRandomImage(unsigned int width, unsigned int height, unsigned int seed, float scalingFactor)
    {
        std::vector<float> randomValues;
        generateRandomNumbersFromMinusOneToOne(width * height, randomValues, seed);
        
        if (scalingFactor != 1.0)
            std::transform(randomValues.begin(), randomValues.end(), randomValues.begin(), std::bind1st(std::multiplies<float>(), scalingFactor));
    
        Image* randomImage = new Image(Vec2ui(width, height), &randomValues[0]);
        return randomImage;
    }

}