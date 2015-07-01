#pragma once
#include "io/HyperStackIndex.h"

namespace ettention
{
    class ParameterSet;
    class ParameterSource;

    class AlgebraicParameterSet
    {
    public:
        enum BasisFunctionType
        {
            BASIS_VOXELS,
            BASIS_BLOBS,
        };

    private:
        const ParameterSource* parameterSource;
        std::vector<const ParameterSet*> parameterSets;
        std::string projectionSetType;
        std::set<HyperStackIndex> skipProjectionList;
        BasisFunctionType basisFunctions;

        void ParseSkipProjectionList(const std::string &s);
        void InsertRangeIntoSkipProjectionsList(const std::string &s);

    public:
        AlgebraicParameterSet(const ParameterSource* parameterSource);
        ~AlgebraicParameterSet();

        const std::string& ProjectionSetType() const;
        const std::set<HyperStackIndex>& SkipProjectionList() const;
        BasisFunctionType GetBasisFunctions() const;

        template<typename _ParameterSetType>
        const _ParameterSetType* get();
    };

    template<typename _ParameterSetType>
    const _ParameterSetType* AlgebraicParameterSet::get()
    {
        for(auto it = parameterSets.begin(); it != parameterSets.end(); ++it)
        {
            auto paramSet = dynamic_cast<const _ParameterSetType*>(*it);
            if(paramSet)
            {
                return paramSet;
            }
        }
        auto paramSet = new _ParameterSetType(parameterSource);
        parameterSets.push_back(paramSet);
        return paramSet;
    }
}