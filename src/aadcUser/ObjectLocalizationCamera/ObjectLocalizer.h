#pragma once

#include <adtffiltersdk/adtf_filtersdk.h>
#include <owndesc.h>
#include <aadc.h>
#include <cmath>
#include <cstring>

using namespace adtf::base;
using namespace adtf::streaming;
using namespace adtf::filter;
using namespace adtf::mediadescription;

#define PI 3.141592654
#define DEG2RAD PI/180

enum tCameraSelection : int
{
    SELECTION_FrontCamera = 0,
    SELECTION_RearCamera = 1
};

class cObjectLocalizerCamera : public cFilter
{
    public:
        ADTF_CLASS_ID_NAME(cObjectLocalizerCamera, "ObjectLocalizerCamera.thi.de","Camera ObjectLocalizer");
        cObjectLocalizerCamera();
        ~cObjectLocalizerCamera() = default;

        tResult ProcessInput(ISampleReader* pReader, const iobject_ptr<const ISample>& pSample) override;
        // tResult AcceptType(ISampleReader* /*pReader*/, const adtf::ucom::iobject_ptr<const adtf::streaming::IStreamType>& pType) override;
    private:
        property_variable<tCameraSelection> m_selectedCamera;

        ISampleReader* m_readerDetectionList;
        ISampleWriter* m_writerObjectList;

        void processForFrontCamera(uint16_t x,uint16_t y,tObjectPose& result);
        void processForRearCamera(uint16_t x,uint16_t y,tObjectPose& result);

        // adtf::mediadescription::md_sample_data_factory<tObjectDetectionResult> m_oSampleDataFactory;
};