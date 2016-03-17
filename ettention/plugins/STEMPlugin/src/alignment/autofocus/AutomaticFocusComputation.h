#pragma once
#include "io/datasource/ImageStackDatasource.h"
#include "model/ProjectionSetRandomDirectionSequentialFocus.h"
#include "model/STEMScannerGeometry.h"
#include "alignment/autofocus/Marker.h"
#include "alignment/autofocus/MarkerFileParser.h"
#include "setup/TF_ART_ParameterSet.h"

namespace ettention
{
    namespace stem
    {
        class AutomaticFocusComputation
        {
        public:
            class FittingParameter
            {
            public:
                float focusDifferenceBetweenImages;
                float focusAtFirstImage;
            };

            AutomaticFocusComputation(const TF_ART_ParameterSet* parameterSet, STEMScannerGeometry* baseScannerGeometry, ProjectionSetRandomDirectionSequentialFocus* projectionSet, TF_Datasource* projectionDataSource, Volume* volume);
            virtual ~AutomaticFocusComputation();

			void correctFocus(STEMScannerGeometry* scannerGeometry, int indexOfImageInsideStack);
            FittingParameter getFittingParameterForDirection(int directionIndex);

        protected:
            Ray computeRayCorrespondingToMarker(std::shared_ptr<Marker> marker);
            virtual void compute();

            void compute3DPositionEstimates();
            void mark3DPositionEstimatesInVolume();
            void makeBlobInVolume(Vec3f position, float intensity, int radius);

            float getParticleDistanceFromTiltAxis(Vec3f position, float tiltAngle);

            float determineFocalDifferenceBetweenImages();
            float determineFocalDifferenceBetweenImages(int directionIndex);
            float getTiltAngleForDirectionIndex(int directionIndex);

            unsigned int getNumberOfLandmarkChainsToConsider();

            void outputAlignmentErrorFileHeaderIfRequested();
            void outputAlignmentErrorIfRequested();
            void closeAlignmentErrorLogFileIfRequested();

			void sanitizeMarker( LandmarkChain*, Marker* );

        protected:
            Volume* volume;
            const TF_ART_ParameterSet* parameterSet;
            float averageAlignmentError;
            SingleAxisTiltRotator satRotator;
            ProjectionSetRandomDirectionSequentialFocus* projectionSet;
            TF_Datasource* projectionDataSource;
            std::string markerFileName;

            int markerSetId;
            float alignmentError;
            float currentFirstTiltAngle;
            float currentSecondTiltAngle;

            std::ofstream* alignmentErrorLog;

            std::unordered_map<unsigned int, FittingParameter, HyperStackIndexHash> fittingParameterFromDirection;
            MarkerFileParser* parser;

        private:
            HyperStackIndex getClosestIndexOfTiltAngle(float tiltAngle);
        };

    }
}