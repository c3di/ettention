#include "stdafx.h"
#include "TF_SART.h"

namespace ettention
{
    namespace stem
    {
        TF_SART::TF_SART(Framework* framework)
            : TF_ART(framework)
        {
        }

        TF_SART::~TF_SART()
        {
        }

        void TF_SART::handleOneIteration()
        {
            projectionNumber = 0;
            for(directionIndex = 0; directionIndex < projectionSet.GetDirectionCount(); directionIndex++)
            {
                for(projectionPerDirection = 0; projectionPerDirection < projectionSet.GetFocusCount(directionIndex); projectionPerDirection++)
                {
                    if(parameterSet->shouldSkipFocalPosition(projectionPerDirection))
                        continue;

                    projectionIndex = projectionSet.getProjectionIndex(directionIndex, projectionPerDirection);

                    measuredProjection->setObjectOnCPU(projectionDataSource->getProjectionImage(projectionIndex));

                    auto props = projectionDataSource->GetProjectionProperties(projectionIndex);

                    if(parameterSet->shouldSkipDirection(props.GetTiltAngle()))
                        break;

                    if(parameterSet->shouldUseAutofocus())
                        autofocus->correctFocus(props, projectionPerDirection);

                    projectionProperties[projectionPerDirection] = props;

                    geometricSetup->setScannerGeometry(projectionDataSource->getScannerGeometry(projectionIndex, framework->getParameterSet()));
                    forwardProjectionOperator->SetProjectionProperties(props);

                    projectionNumber++;
                    logProjectionInformationIfRequested(projectionNumber, props);
                    writeOutProjectionIfRequested(projectionDataSource->getProjectionImage(projectionIndex));
                    handleForwardProjection();
                    compansateLongObject();
                    computeProjectedError();
                    handleBackProjection();
                }

                writeOutIntermediateVolumeIfRequested("direction_");

            }
            writeIterationResulIfRequested();
        }

    }
}
