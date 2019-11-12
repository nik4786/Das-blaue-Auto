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

    // const tObjectPhys* out_objects = reinterpret_cast<tObjectPhys*>(output_sample->objects);
    memset(output_sample->objects, 0, sizeof(output_sample->objects)); // works because length of array objects is static

    for(uint8_t i=0;i<detection_results->numResults;++i)
    {
         strcpy(output_sample->objects[i].className, detection_results->objectsDetected[i].className);
         output_sample->objects[i].classificationProb = detection_results->objectsDetected[i].prob;
    }


    RETURN_NOERROR;
}

void cObjectLocalizerCamera::processForFrontCamera(uint16_t x,uint16_t y,tObjectPose& result)
{
    result.x = 57.82/(y-473.9);
    float angle = tan((-0.0002*x*x + 0.0185*x + 62.301)*DEG2RAD);
    result.y = result.x / angle;
}
void cObjectLocalizerCamera::processForRearCamera(uint16_t x,uint16_t y,tObjectPose& result)
{
    
}
