#pragma once

#include <adtffiltersdk/adtf_filtersdk.h>
#include <owndesc.h>

using namespace adtf::base;
using namespace adtf::streaming;
using namespace adtf::filter;
using namespace adtf::mediadescription;

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

    private:
        property_variable<tCameraSelection> m_selectedCamera;

        ISampleReader* m_readerDetectionList;
        ISampleWriter* m_writerObjectList;
};