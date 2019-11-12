#include "ObjectLocalizer.h"

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


tResult cObjectLocalizerCamera::ProcessInput(ISampleReader* pReader, const iobject_ptr<const ISample>& pSample)
{
    RETURN_NOERROR;
}

