#include "stdafx.h"
#include "TF_SART.h"
#include <framework/Framework.h>

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
            for (directionIndex = 0; directionIndex < projectionSet.getDirectionCount(); directionIndex++)
            {
                projectionProperties.resize(projectionSet.getFocusCount(directionIndex));
                for (projectionPerDirection = 0; projectionPerDirection < projectionSet.getFocusCount(directionIndex); projectionPerDirection++)
                {
                    if(parameterSet->shouldSkipFocalPosition(projectionPerDirection))
                        continue;

                    projectionIndex = projectionSet.getProjectionIndex(directionIndex, projectionPerDirection);

                    measuredProjection->setObjectOnCPU(projectionDataSource->getProjectionImage(projectionIndex));

                    auto props = (STEMScannerGeometry*) projectionDataSource->GetScannerGeometry(projectionIndex)->clone();

                    if(parameterSet->shouldSkipDirection(props->getTiltAngle()))
                        break;

                    if(parameterSet->shouldUseAutofocus())
                        autofocus->correctFocus(props, projectionPerDirection);

                    projectionProperties[projectionPerDirection] = props;

                    geometricSetup->setScannerGeometry(props);
                    forwardProjectionOperator->setScannerGeometry(props);

                    projectionNumber++;
                    logProjectionInformationIfRequested(props);
                    writeOutProjectionIfRequested(projectionDataSource->getProjectionImage(projectionIndex));
                    handleForwardProjection();
                    compansateLongObject();
                    computeProjectedError();
                    handleBackProjection();
                    writeOutIntermediateVolumeIfRequested("direction_");
                }

            }
            writeIterationResulIfRequested();
        }

    }
}
