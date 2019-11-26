/**
 * @file ObjectLocalizer.cpp
 *
 * @brief Implementation of the camera-based object localizer 
 * *
 * @author Florian Braun
 * Contact: flb8127@thi.de
 *
 */
#include "ObjectLocalizer.h"

ADTF_PLUGIN("Object Localizer",cObjectLocalizerCamera);

cObjectLocalizerCamera::cObjectLocalizerCamera()
{
    // konstruktor
    m_selectedCamera.SetValueList({{SELECTION_FrontCamera,"Front Camera"},
                                    {SELECTION_RearCamera,"Rear Camera"}});
    m_selectedCamera.SetDescription("Choose which camera produces the input so the corect mapping is used.");
    RegisterPropertyVariable("Camera",m_selectedCamera);

    m_readerDetectionList = CreateInputPin("detections",description<tObjectDetectionResult>());
    m_writerObjectList = CreateOutputPin("object_list",description<tObjectListPhys>());
}

// tResult cObjectLocalizerCamera::AcceptType(ISampleReader* /*pReader*/, const adtf::ucom::iobject_ptr<const adtf::streaming::IStreamType>& pType)
// {
//     m_oSampleDataFactory = adtf::mediadescription::md_sample_data_factory<tObjectDetectionResult>(pType);
//     RETURN_NOERROR;

// }

tResult cObjectLocalizerCamera::ProcessInput(ISampleReader* pReader, const iobject_ptr<const ISample>& pSample)
{
    sample_data<tObjectDetectionResult> detection_results(pSample);

    output_sample_data<tObjectListPhys> output_sample(get_sample_time(pSample));
    output_sample->numResults = detection_results->numResults;

    // set values in object list to a defined state (everything 0)
    memset(output_sample->objects, 0, sizeof(output_sample->objects)); // works because length of array "objects" is static (see owndesc.h)

    LOG_INFO("array size: %d",sizeof(output_sample->objects));

    for(uint8_t i=0;i<detection_results->numResults;++i)
    {
         strcpy(output_sample->objects[i].className, detection_results->objectsDetected[i].className);
         output_sample->objects[i].classificationProb = detection_results->objectsDetected[i].prob;

        // propose a pose (rather bad because of low resolution but better than nothing?)
        tObjectPose pose;
        auto center_x = static_cast<uint16_t>(detection_results->objectsDetected[i].left+detection_results->objectsDetected[i].width/2);
        auto bottom = static_cast<uint16_t>(detection_results->objectsDetected[i].top+detection_results->objectsDetected[i].height);
        processForFrontCamera(center_x,bottom,pose);
        output_sample->objects[i].pose = pose;
    }

    m_writerObjectList->Write(output_sample.Release());

    RETURN_NOERROR;
}

void cObjectLocalizerCamera::processForFrontCamera(uint16_t x,uint16_t y,tObjectPose& result)
{
    // calculate the x-coordinate in the sensor frame by the y-coordinate of the bbox in the image
    result.x = (57.82) / (y - 473.9)*(1-(624-x)/424)+ (50.44) / (y - 476.3)*((624-x)/465); 

    // catch the case that the fit function is only valid for x<=625
    double angle = 1;
    if(x>625)
    {
        x = 1250-x;
        angle = -1;
    }

    angle *= tan(-0.00000337*x*x + 0.0003849*x + 1.071); //fit in rad

    result.y = result.x / angle;
}
void cObjectLocalizerCamera::processForRearCamera(uint16_t x,uint16_t y,tObjectPose& result)
{
    
}
