#pragma once
#include <list>
#include "setup/parametersource/ParameterSource.h"
#include "setup/parameterset/ParameterSet.h"

namespace ettention
{
    namespace stem
    {
        class PLUGIN_API TF_ART_ParameterSet : public ParameterSet
        {
        public:
            enum BackProjectionMode : int
            {
                PARALLEL,
                REGULARIZATION,
                ADJOINT
            };

            enum ForwardProjectionMode : int
            {
                FORWARD_PARALLEL,
                FORWARD_CONVERGENT,
            };

            enum RegularizationMode : int
            {
                NONE,
                LINEAR_DISTANCE_TO_FOCAL_PLANE
            };

            TF_ART_ParameterSet(const ParameterSource* parameterSource);
            virtual ~TF_ART_ParameterSet();

			bool isConvergentBeam() const;
            float getBeamOpeningHalfAngle() const;
            BackProjectionMode getBackProjectionMode() const;
            ForwardProjectionMode getForwardProjectionMode() const;
            RegularizationMode getRegularizationMode() const;
            std::string MarkerFileName() const;
            void MarkerFileName(std::string val);
            bool shouldUseAutofocus() const;
            bool shouldOutputAutoFocusValue() const;

            bool shouldSkipDirection(float tiltAngle) const;
            bool shouldSkipFocalPosition(int focalPosition) const;
            size_t numberOfFocalPositionsToSkip() const;

            float getSimulatedFocusDefectAmount() const;
            void setSimulatedFocusDefectAmount(float val);

            boost::filesystem::path OutputAlignmentErrorFileName() const;
            bool ShouldOutputAlignmentError() const;

        protected:
            static std::list<float> parseSkipTiltAngles(std::string skipAngles);
            static std::list<int> parseSkipFocalPositions(std::string skipFocalPositions);
            static BackProjectionMode parseBackProjectionMethod(std::string methodString);
            static ForwardProjectionMode parseForwardProjectionMethod(std::string methodString);
            static RegularizationMode parseRegularizationMode(std::string methodString);

            bool convergentBeam;
            float beamOpeningHalfAngle;
            BackProjectionMode backProjectionMode;
            ForwardProjectionMode forwardProjectionMode;
            RegularizationMode regularizationMode;
            boost::filesystem::path markerFileName;
            std::list<float> skipTiltAngles;
            std::list<int> skipFocalPositions;
            bool useAutoFocus;
            bool outputAutoFocusValue;
            float simulatedFocusDefectAmount;

            bool shouldOutputAlignmentError;
            boost::filesystem::path outputAlignmentErrorFileName;

        };

    }
}