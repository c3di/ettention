#pragma once
#include <boost/any.hpp>
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    class ManualParameterSource : public ParameterSource
    {
    public:
        class ParameterTypeWrapper
        {
            friend class ManualParameterSource;
        public:
            template <typename _ParameterType>
            _ParameterType as() const { return boost::any_cast<_ParameterType>(value); }

        private:
            boost::any value;

            ParameterTypeWrapper(boost::any value);
        };

        virtual LogLevelValues LogLevel() const override;
        virtual void LogLevel(const LogLevelValues level) override;
        virtual void parse() override;
        virtual bool parameterExists(std::string aName) const override;
        ParameterTypeWrapper getParameter(const std::string name) const;
        virtual std::string getStringParameter(std::string aName) const override;
        boost::filesystem::path getPathParameter(std::string aName) const override;
        virtual float getFloatParameter(std::string aName) const override;
        virtual bool getBoolParameter(std::string aName) const override;
        virtual int getIntParameter(std::string aName) const override;
        virtual unsigned int getUIntParameter(std::string aName) const override;
        virtual Vec3ui getVec3uiParameter(std::string aName) const override;
        virtual Vec3f getVec3fParameter(std::string aName) const override;

        template<typename _ParameterType>
        void setParameter(const std::string& name, _ParameterType value);

    private:
        typedef std::map<std::string, boost::any> parameterContainterType;
        parameterContainterType parameters;
    };

    template<typename _ParameterType>
    void ManualParameterSource::setParameter(const std::string& name, _ParameterType value)
    {
        parameters[name] = boost::any(value);
    }
    
    template<>
    inline void ManualParameterSource::setParameter<const char*>(const std::string& key, const char* value)
    {
        setParameter(key, std::string(value));
    }
}

