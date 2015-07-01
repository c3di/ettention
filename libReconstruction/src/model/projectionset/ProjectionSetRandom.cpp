#include "stdafx.h"
#include "ProjectionSetRandom.h"
#include <boost/random.hpp>

namespace ettention
{
    ProjectionSetRandom::ProjectionSetRandom()
    {

    }

    ProjectionSetRandom::~ProjectionSetRandom()
    {

    }

    void ProjectionSetRandom::Reorder(std::vector<HyperStackIndex>& indices)
    {
        struct permutation_function : std::unary_function<unsigned, unsigned>
        {
            boost::mt19937 &_state;
            unsigned operator()(unsigned i) {
                boost::uniform_int<> rng(0, i - 1);
                return rng(_state);
            }
            permutation_function(boost::mt19937 &state) : _state(state) {
            }
        };

        boost::random::mt19937 rng;
        permutation_function fct(rng);
        for(unsigned int i = 0; i < indices.size(); ++i)
        {
            indices[i] = i;
        }

        std::random_shuffle(indices.begin(), indices.end(), fct);
    }
}