#include "stdafx.h"
#include "TF_ART_ParameterSet.h"
#include "framework/error/Exception.h"

namespace ettention
{
    namespace stem
    {
        TF_ART_ParameterSet::TF_ART_ParameterSet(const ParameterSource* parameterSource)
        {
            convergentBeam = false;
            if(parameterSource->parameterExists("convergentBeam"))
                convergentBeam = parameterSource->getBoolParameter("convergentBeam");

            if(convergentBeam && parameterSource->parameterExists("beamOpeningHalfAngle"))
                beamOpeningHalfAngle = parameterSource->getFloatParameter("beamOpeningHalfAngle");

            if(convergentBeam)
                backProjectionMode = parseBackProjectionMethod(parameterSource->getStringParameter("backProjection"));

            if(convergentBeam)
                forwardProjectionMode = parseForwardProjectionMethod(parameterSource->getStringParameter("forwardProjection"));

            if(convergentBeam)
                regularizationMode = parseRegularizationMode(parameterSource->getStringParameter("backProjection.regularization"));

            useAutoFocus = false;
            if(convergentBeam && parameterSource->parameterExists("algorithm.useAutomaticFocusComputation"))
                useAutoFocus = parameterSource->getBoolParameter("algorithm.useAutomaticFocusComputation");

            outputAutoFocusValue = false;
            if(parameterSource->parameterExists("outputAxialAlignmentValues"))
                outputAutoFocusValue = parameterSource->getBoolParameter("outputAxialAlignmentValues");

            if(convergentBeam && useAutoFocus)
                markerFileName = parameterSource->getStringParameter("markerFilename");

            if(parameterSource->parameterExists("skipTiltAngles"))
                skipTiltAngles = parseSkipTiltAngles(parameterSource->getStringParameter("skipTiltAngles"));

            if(parameterSource->parameterExists("skipFocalPositions"))
                skipFocalPositions = parseSkipFocalPositions(parameterSource->getStringParameter("skipFocalPositions"));

            simulatedFocusDefectAmount = 0.0f;
            if(parameterSource->parameterExists("simulateFocusDefectAmount"))
                simulatedFocusDefectAmount = parameterSource->getFloatParameter("simulateFocusDefectAmount");

            shouldOutputAlignmentError = false;
            if(parameterSource->parameterExists("outputAlignmentErrorFileName"))
            {
                shouldOutputAlignmentError = true;
                outputAlignmentErrorFileName = boost::filesystem::path(parameterSource->getStringParameter("outputAlignmentErrorFileName"));
            }

        }

        TF_ART_ParameterSet:: ~TF_ART_ParameterSet()
        {
        }

        bool TF_ART_ParameterSet::isConvergentBeam() const
        {
            return convergentBeam;
        }

        float TF_ART_ParameterSet::BeamOpeningHalfAngle() const
        {
            return beamOpeningHalfAngle;
        }

        TF_ART_ParameterSet::BackProjectionMode TF_ART_ParameterSet::getBackProjectionMode() const
        {
            return backProjectionMode;
        }

        TF_ART_ParameterSet::ForwardProjectionMode TF_ART_ParameterSet::getForwardProjectionMode() const
        {
            return forwardProjectionMode;
        }

        TF_ART_ParameterSet::RegularizationMode TF_ART_ParameterSet::getRegularizationMode() const
        {
            return regularizationMode;
        }

        std::list<float> TF_ART_ParameterSet::parseSkipTiltAngles(std::string skipAngles)
        {
            std::list<float> results;
            std::istringstream iss(skipAngles);
            while(!iss.eof())
            {
                float skipAngle;
                iss >> skipAngle;
                results.push_back(skipAngle);
            }
            return results;
        }

        std::list<int> TF_ART_ParameterSet::parseSkipFocalPositions(std::string skipFocalPositions)
        {
            std::list<int> results;
            std::istringstream iss(skipFocalPositions);
            while(!iss.eof())
            {
                int skipPosition;
                iss >> skipPosition;
                results.push_back(skipPosition);
            }
            return results;
        }

        TF_ART_ParameterSet::BackProjectionMode TF_ART_ParameterSet::parseBackProjectionMethod(std::string methodString)
        {
            if(methodString.compare("stem.regularization") == 0)
                return REGULARIZATION;
            if(methodString.compare("stem.adjoint") == 0)
                return ADJOINT;
            throw Exception("parse error in ConvergentBeamsParameterSet::BackProjectionMethod: illegal method " + methodString);
        }

        TF_ART_ParameterSet::ForwardProjectionMode TF_ART_ParameterSet::parseForwardProjectionMethod(std::string methodString)
        {
            if(methodString.compare("stem") == 0)
                return FORWARD_CONVERGENT;
            throw Exception("parse error in ConvergentBeamsParameterSet::ForwardProjectionMethod: illegal method " + methodString);
        }

        TF_ART_ParameterSet::RegularizationMode TF_ART_ParameterSet::parseRegularizationMode(std::string methodString)
        {
            if(methodString.compare("none") == 0)
                return NONE;
            if(methodString.compare("linear_distance_to_focal_plane") == 0)
                return LINEAR_DISTANCE_TO_FOCAL_PLANE;
            throw Exception("parse error in ConvergentBeamsParameterSet::RegularizationMode: illegal method " + methodString);
        }

        std::string TF_ART_ParameterSet::MarkerFileName() const
        {
            return markerFileName;
        }

        void TF_ART_ParameterSet::MarkerFileName(std::string val)
        {
            markerFileName = val;
        }

        bool TF_ART_ParameterSet::shouldUseAutofocus() const
        {
            return useAutoFocus;
        }

        bool TF_ART_ParameterSet::shouldOutputAutoFocusValue() const
        {
            return outputAutoFocusValue;
        }

        bool TF_ART_ParameterSet::shouldSkipDirection(float tiltAngle) const
        {
            for(auto it = skipTiltAngles.begin(); it != skipTiltAngles.end(); ++it)
            {
                if(fabs((*it) - tiltAngle) < 0.01f)
                    return true;
            }
            return false;
        }

        bool TF_ART_ParameterSet::shouldSkipFocalPosition(int focalPosition) const
        {
            for(auto it = skipFocalPositions.begin(); it != skipFocalPositions.end(); ++it)
            {
                if((*it) == focalPosition)
                    return true;
            }
            return false;
        }

        size_t TF_ART_ParameterSet::numberOfFocalPositionsToSkip() const
        {
            return skipFocalPositions.size();
        }

        float TF_ART_ParameterSet::getSimulatedFocusDefectAmount() const
        {
            return simulatedFocusDefectAmount;
        }

        void TF_ART_ParameterSet::setSimulatedFocusDefectAmount(float val)
        {
            simulatedFocusDefectAmount = val;
        }

        boost::filesystem::path TF_ART_ParameterSet::OutputAlignmentErrorFileName() const
        {
            return outputAlignmentErrorFileName;
        }

        bool TF_ART_ParameterSet::ShouldOutputAlignmentError() const
        {
            return shouldOutputAlignmentError;
        }

    }
}