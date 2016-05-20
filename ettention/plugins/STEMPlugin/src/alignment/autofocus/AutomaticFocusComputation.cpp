#include "stdafx.h"
#include "AutomaticFocusComputation.h"
#include "framework/error/ProjectionTypeNotSupportedException.h"
#include "framework/geometry/GeometricAlgorithms.h"
#include "framework/RandomAlgorithms.h"
#include "framework/LeastSquaresLinearFit.h"
#include "framework/Logger.h"

namespace ettention
{
    namespace stem
    {

        bool sortByAlignmentError(std::shared_ptr<LandmarkChain> a, std::shared_ptr<LandmarkChain> b)
        {
            return (a->getAlignmentError() < b->getAlignmentError());
        }

        AutomaticFocusComputation::AutomaticFocusComputation(const TF_ART_ParameterSet* parameterSet,
                                                             STEMScannerGeometry* baseScannerGeometry,
                                                             ProjectionSetRandomDirectionSequentialFocus* projectionSet,
                                                             TF_Datasource* projectionDataSource,
                                                             Volume* volume)
            : parameterSet(parameterSet)
            , projectionSet(projectionSet)
            , projectionDataSource(projectionDataSource)
            , volume(volume)
            , alignmentErrorLog(nullptr)
        {
            satRotator.setBaseScannerGeometry(baseScannerGeometry);
            compute();
        }

        AutomaticFocusComputation::~AutomaticFocusComputation()
        {
            if(alignmentErrorLog != nullptr)
            {
                delete alignmentErrorLog;
            }
        }

        void AutomaticFocusComputation::correctFocus(STEMScannerGeometry* geometry, int indexOfImageInsideStack)
        {
            float tiltAngle = geometry->getTiltAngle();
            unsigned int index = this->getClosestIndexOfTiltAngle(tiltAngle)[0];
            FittingParameter parameter = fittingParameterFromDirection[index];

            float focusDifferenceBetweenImages = geometry->getFocalDifferenceBetweenImages();

            float indexOfImageInsideStackf = 19.0f - (float)indexOfImageInsideStack;
            float focus = parameter.focusAtFirstImage + (indexOfImageInsideStackf * focusDifferenceBetweenImages);

            geometry->setFocalDepth(focus);
        }

        HyperStackIndex AutomaticFocusComputation::getClosestIndexOfTiltAngle(float tiltAngle)
        {
            float bestError = 360.0f;
            HyperStackIndex bestIndex((unsigned int)-1);
            for(unsigned int dir = 0; dir < projectionSet->getDirectionCount(); ++dir)
            {
                auto index = projectionSet->getProjectionIndex(dir, 0);
                float thisTiltAngle = projectionDataSource->getTiltAngle(index);
                float thisError = fabs(thisTiltAngle - tiltAngle);
                if(thisError < bestError)
                {
                    bestIndex = index;
                    bestError = thisError;
                }
            }
            return bestIndex;
        }

        AutomaticFocusComputation::FittingParameter AutomaticFocusComputation::getFittingParameterForDirection(int directionIndex)
        {
            return fittingParameterFromDirection[directionIndex];
        }

        void AutomaticFocusComputation::compute()
        {
            LOGGER("loading projections for marker computation");

            parser = new MarkerFileParser(parameterSet->MarkerFileName(), projectionSet, projectionDataSource);

            compute3DPositionEstimates();
            // mark3DPositionEstimatesInVolume();

            std::ofstream ofs("alignment.csv");
            ofs << "tilt angle;marker id;alignment error;z_prime_focus;ray direction;particle pos 2D;particle pos 3D.x;particle pos 3D.y;particle pos 3D.z;f(z_prime_focus);t maximum pos;v_0;v_1;v_2;v_3;v_4;v_5;v_6;v_7;v_8;v_9;v_10;v_11;v_12;v_13;v_14;v_15;v_16;v_17;v_18;v_19" << std::endl;

            std::ofstream precisionLog("precision.csv");
            precisionLog << "tilt_angle;precision" << std::endl;

            // float focalDifferenceBetweenImages = determineFocalDifferenceBetweenImages();

            for(unsigned int directionIndex = 0; directionIndex < projectionSet->getDirectionCount(); directionIndex++)
            {
                auto index = projectionSet->getOriginalProjectionIndex(directionIndex, 0);
                auto tiltAngle = projectionDataSource->getTiltAngle(index);
                auto focalDiffBetweenImages = projectionDataSource->getScannerGeometry(index)->getFocalDifferenceBetweenImages();

                unsigned int numberOfEstimatesToConsider = getNumberOfLandmarkChainsToConsider();
                std::list<float> focusBaseEstimates;

                std::cout << "Estimating focus for direction " << tiltAngle;

                LandmarkChainIterator it = parser->begin();
                for(unsigned int i = 0; i < numberOfEstimatesToConsider; i++)
                {
                    std::shared_ptr<LandmarkChain> markerSet = *it;
                    it++;

                    if(!markerSet->hasEntryForTiltAngle(tiltAngle))
                        continue;

                    std::shared_ptr<Marker> marker = markerSet->getEntryForTiltAngle(tiltAngle);

                    sanitizeMarker(markerSet.get(), marker.get());

                    int markerPos = marker->getPositionOfGlobalMaximum();
                    Vec2f markerPos2D = marker->getPositionInImage();

                    Vec3f particleDirection = markerSet->getParticlePosition() - marker->getRay().getOrigin();
                    float markerPosInRayCoordinates = particleDirection.getLengthF();

                    particleDirection = doNormalize(particleDirection);
                    float directionalError = dotProduct(marker->getRay().getNormalizedDirection(), particleDirection);
                    if(directionalError < 0.99f)
                        throw Exception("using misaligned markers");

                    float intensityMaximumPosInRayCoordinates = ( (float) markerPos ) * focalDiffBetweenImages;

                    ofs << tiltAngle << ";" << marker->getId() << ";" << markerSet->getAlignmentError() << ";" << markerPos << ";" << marker->getRay().getNormalizedDirection() << ";" << markerPos2D << ";" << markerSet->getParticlePosition().x << ";" << markerSet->getParticlePosition().y << ";" << markerSet->getParticlePosition().z << ";" << markerPosInRayCoordinates << ";" << intensityMaximumPosInRayCoordinates << ";";
                    for(unsigned int i = 0; i < marker->getIntensityZProjection().size(); i++)
                        ofs << marker->getIntensityZProjection()[i] << ";";
                    ofs << std::endl;

                    float particlePositionRelativeToTiltAxis = getParticleDistanceFromTiltAxis(markerSet->getParticlePosition(), tiltAngle);
                    float focusBaseEstimateRelativeToTiltAxis = particlePositionRelativeToTiltAxis - intensityMaximumPosInRayCoordinates;
                    
                    /* if (directionIndex == 5)
                    {
                        float imagePlaneToTiltAxis = satRotator.getBaseScannerGeometry()->getDetectorBase().z;
                        Vec3f tiltAxisPosition = marker->getRay().getOrigin() + (marker->getRay().getNormalizedDirection() * (imagePlaneToTiltAxis));
                        makeBlobInVolume(tiltAxisPosition, 50, 2);

                        Vec3f secondOpinionOnParticlePosition = marker->getRay().getOrigin() + (marker->getRay().getNormalizedDirection() * ( imagePlaneToTiltAxis + particlePositionRelativeToTiltAxis ) );
                        makeBlobInVolume(secondOpinionOnParticlePosition, 200, 1);

                        for (float i = 0.0f; i < 2048.0f; i += 2.0f)
                        {
                            Vec3f positionAlongRay = marker->getRay().getOrigin() + (marker->getRay().getNormalizedDirection() * i);
                            makeBlobInVolume(positionAlongRay, 40, 0);
                        }
                    } */

                    focusBaseEstimates.push_back(focusBaseEstimateRelativeToTiltAxis);
                }

                float focusBaseAverage = 0.0f;
                for(auto it = focusBaseEstimates.begin(); it != focusBaseEstimates.end(); ++it)
                    focusBaseAverage += *it;
                focusBaseAverage /= (float)focusBaseEstimates.size();

                float errorAvg = 0.0f;
                for(auto it = focusBaseEstimates.begin(); it != focusBaseEstimates.end(); ++it)
                {
                    float estimateError = fabs((*it) - focusBaseAverage);
                    errorAvg += estimateError;
                    precisionLog << tiltAngle << ";" << estimateError << std::endl;
                }
                errorAvg /= (float)focusBaseEstimates.size();
                std::cout << " avg: " << focusBaseAverage << " (+- " << errorAvg << ") " << std::endl;

                AutomaticFocusComputation::FittingParameter fittingParameter;
                fittingParameter.focusAtFirstImage = focusBaseAverage;

                if(parameterSet->getSimulatedFocusDefectAmount() > 0.0f)
                {
                    fittingParameter.focusAtFirstImage += RandomAlgorithms::generateRandomNumbersFromMinusOneToOne(1)[0] * parameterSet->getSimulatedFocusDefectAmount();
                }
                fittingParameterFromDirection[directionIndex] = fittingParameter;
            }
            delete parser;
            ofs.close();
            precisionLog.close();
        }

        float AutomaticFocusComputation::getParticleDistanceFromTiltAxis(Vec3f position, float tiltAngle)
        {
            ScannerGeometry* rotatedGeometry = satRotator.createRotatedScannerGeometry(tiltAngle);
            Matrix4x4 transform = rotatedGeometry->getProjectionMatrix();
            Vec4f pos = Vec4f(position.x, position.y, position.z, 1.0f);
            Vec4f transformedPosition = multiply(transform, pos);
            return transformedPosition.z / transformedPosition.w;
        }

        void AutomaticFocusComputation::compute3DPositionEstimates()
        {
            outputAlignmentErrorFileHeaderIfRequested();

            markerSetId = 0;
            for(LandmarkChainIterator markerSet = parser->begin(); markerSet != parser->end(); ++markerSet)
            {
                markerSetId++;
                // pairwise iteration
                float totalError = 0.0f;
                Vec3f particlePosition(0.0f, 0.0f, 0.0f);
                int nComparisons = 0;
                for(auto markerIteratorA = (*markerSet)->begin(); markerIteratorA != (*markerSet)->end(); ++markerIteratorA)
                {
                    currentFirstTiltAngle = markerIteratorA->first;

                    std::shared_ptr<Marker> markerA = markerIteratorA->second;
                    Ray rayA = computeRayCorrespondingToMarker(markerA);
                    markerA->setRay(rayA);

                    auto markerIteratorB = markerIteratorA;
                    markerIteratorB++;
                    while(markerIteratorB != (*markerSet)->end())
                    {
                        currentSecondTiltAngle = markerIteratorB->first;
                        Ray rayB = computeRayCorrespondingToMarker(markerIteratorB->second);
                        auto points = GeometricAlgorithms::getClosestPointsOfRays(rayA, rayB);
                        Vec3f particlePositionEstimage = (points.first + points.second) * 0.5f;
                        particlePosition += particlePositionEstimage;
                        alignmentError = (points.first - points.second).getLengthF();
                        totalError += alignmentError;
                        outputAlignmentErrorIfRequested();
                        nComparisons++;
                        markerIteratorB++;
                    }
                }
                (*markerSet)->getParticlePosition(particlePosition / (float)nComparisons);
                (*markerSet)->getAlignmentError(totalError / (float)nComparisons);
            }
            closeAlignmentErrorLogFileIfRequested();

            std::sort(parser->begin(), parser->end(), sortByAlignmentError);

            unsigned int count = 0;
            float errorMin = boost::numeric::bounds<float>::highest();
            float errorMax = boost::numeric::bounds<float>::lowest();
            float errorAvg = 0.0f;

            for(auto it = parser->begin(); it != parser->end(); ++it)
            {
                if((*it)->getAlignmentError() > errorMax)
                    errorMax = (*it)->getAlignmentError();
                if ((*it)->getAlignmentError() < errorMin)
                    errorMin = (*it)->getAlignmentError();
                errorAvg += (*it)->getAlignmentError();
                count++;
            }
            errorAvg /= (float)count;
            LOGGER("Alignment Error:  " << errorMin << " - " << errorMax << " (avg: " << errorAvg << ")");
        }

        float AutomaticFocusComputation::determineFocalDifferenceBetweenImages()
        {
            std::ofstream ofs;

            if(parameterSet->shouldOutputAutoFocusValue())
            {
                ofs.open("vertical_alignment.csv");
                ofs << "tiltAngle" << ";" << "deltaF" << std::endl;
            }

            std::vector<float> deltaFEstimages;

            for (unsigned int directionIndex = 0; directionIndex < projectionSet->getDirectionCount(); ++directionIndex)
            {
                float tiltAngle = getTiltAngleForDirectionIndex(directionIndex);

                float deltaF = determineFocalDifferenceBetweenImages(directionIndex);
                deltaFEstimages.push_back(deltaF);

                if(parameterSet->shouldOutputAutoFocusValue())
                    ofs << tiltAngle << ";" << deltaF << std::endl;
            }

            std::sort(deltaFEstimages.begin(), deltaFEstimages.end());

            if(parameterSet->shouldOutputAutoFocusValue())
                ofs.close();

            float medianOfDeltaFEstimates = deltaFEstimages[deltaFEstimages.size() / 2];
            return medianOfDeltaFEstimates;
        }

        float AutomaticFocusComputation::determineFocalDifferenceBetweenImages(int directionIndex)
        {
            float tiltAngle = getTiltAngleForDirectionIndex(directionIndex);

            std::vector<Vec2f> points;

            unsigned int numberOfEstimatesToConsider = getNumberOfLandmarkChainsToConsider();
            std::list<float> focusBaseEstimates;

            LandmarkChainIterator it = parser->begin();
            for(unsigned int i = 0; i < numberOfEstimatesToConsider; i++)
            {
                std::shared_ptr<LandmarkChain> markerSet = *it;
                it++;

                if(!markerSet->hasEntryForTiltAngle(tiltAngle))
                    continue;

                std::shared_ptr<Marker> marker = markerSet->getEntryForTiltAngle(tiltAngle);
                int markerPos = marker->getPositionOfGlobalMaximum();

                Vec3f particleDirection = markerSet->getParticlePosition() - marker->getRay().getOrigin();

                float f_z_prime = particleDirection.getLengthF();
                float z_prime_focus = (float)marker->getPositionOfGlobalMaximum();
                points.push_back(Vec2f(z_prime_focus, f_z_prime));
            }

            std::shared_ptr<LeastSquaresLinearFit> fit = std::make_shared<LeastSquaresLinearFit>(points);
            return fit->getSlope();
        }

        float AutomaticFocusComputation::getTiltAngleForDirectionIndex(int directionIndex)
        {
            auto index = projectionSet->getOriginalProjectionIndex(directionIndex, 0);
            return projectionDataSource->getTiltAngle(index);
        }

        unsigned int AutomaticFocusComputation::getNumberOfLandmarkChainsToConsider()
        {
            unsigned int count = 0;
            auto it = parser->begin();
            while (it != parser->end() && (*it)->getAlignmentError() < 1.0f)
            {
                count++;
                ++it;
            }
            return count;
        }

        void AutomaticFocusComputation::outputAlignmentErrorFileHeaderIfRequested()
        {
            if(!parameterSet->ShouldOutputAlignmentError())
                return;

            if(alignmentErrorLog != nullptr)
            {
                delete alignmentErrorLog;
            }
            alignmentErrorLog = new std::ofstream(parameterSet->OutputAlignmentErrorFileName().string());
            (*alignmentErrorLog) << "markerSetId" << ";" << "first_angle" << ";" << "second_angle" << ";" << "alignmentError" << std::endl;
        }

        void AutomaticFocusComputation::outputAlignmentErrorIfRequested()
        {
            if(!parameterSet->ShouldOutputAlignmentError())
                return;

            (*alignmentErrorLog) << markerSetId << ";" << currentFirstTiltAngle << ";" << currentSecondTiltAngle << ";" << alignmentError << std::endl;
        }


        void AutomaticFocusComputation::closeAlignmentErrorLogFileIfRequested()
        {
            if(!parameterSet->ShouldOutputAlignmentError())
                return;

            alignmentErrorLog->close();
            delete alignmentErrorLog;
            alignmentErrorLog = nullptr;
        }

        void AutomaticFocusComputation::sanitizeMarker(LandmarkChain* markerSet, Marker* marker)
        {
            float alignmentError = GeometricAlgorithms::getDistanceBetween(marker->getRay(), markerSet->getParticlePosition());
            if (alignmentError > 5.0f)
                throw Exception("using misaligned markers: ray through marker " );
        }

        Ray AutomaticFocusComputation::computeRayCorrespondingToMarker(std::shared_ptr<Marker> marker)
        {
            auto geo = satRotator.createRotatedScannerGeometry(marker->getTiltAngle());
            return geo->getRayThroughPixelCenter(marker->getPositionInImage());
        }

        void AutomaticFocusComputation::mark3DPositionEstimatesInVolume()
        {
            for(LandmarkChainIterator markerSet = parser->begin(); markerSet != parser->end(); ++markerSet)
            {
                Vec3f particlePosition = markerSet->get()->getParticlePosition();
                makeBlobInVolume(particlePosition, 100.0f, 2);
            }
        }

        void AutomaticFocusComputation::makeBlobInVolume(Vec3f position, float intensity, int radius)
        {
            Vec3ui resolution = volume->getProperties().getVolumeResolution();
            Vec3ui voxelIndex = Vec3ui((unsigned int)position.x + (resolution.x / 2),
                                       (unsigned int)position.y + (resolution.y / 2),
                                       (unsigned int)position.z + (resolution.z / 2));
            for (int z = -radius; z <= radius; z++)
            {
                for (int y = -radius; y <= radius; y++)
                {
                    for (int x = -radius; x <= radius; x++)
                    {
                        Vec3ui finalIndex = voxelIndex + Vec3ui(x, y, z);
                        if (finalIndex.x < 0 || finalIndex.y < 0 || finalIndex.z < 0)
                            continue;
                        if (finalIndex.x >= volume->getProperties().getVolumeResolution().x || finalIndex.y >= volume->getProperties().getVolumeResolution().y || finalIndex.z >= volume->getProperties().getVolumeResolution().z )
                            continue;
                        volume->setVoxelToValue(finalIndex, intensity);
                    }
                }
            }
        }

    }
}