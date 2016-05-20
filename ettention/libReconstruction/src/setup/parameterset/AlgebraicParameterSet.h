#pragma once

namespace ettention
{
    class ParameterSet;
    class ParameterSource;

    class AlgebraicParameterSet
    {
    public:

    private:
        const ParameterSource* parameterSource;
        std::vector<const ParameterSet*> parameterSets;

    public:
        AlgebraicParameterSet(const ParameterSource* parameterSource);
        ~AlgebraicParameterSet();

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