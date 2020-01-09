#pragma once

#include <adtffiltersdk/adtf_filtersdk.h>
#include <aadc.h>
#include "stdafx.h"
#include <adtffiltersdk/adtf_filtersdk.h>

using namespace cv;
using namespace std;

using namespace adtf::base;
using namespace adtf::streaming;
using namespace adtf::filter;
using namespace adtf::mediadescription;
using namespace adtf::util;
using namespace adtf::ucom;
using namespace adtf::system;

#include <ADTF3_OpenCV_helper.h>

class cLIDARProcessing : public cFilter
{
    public:
        ADTF_CLASS_ID_NAME(cLIDARProcessing, "LIDARProcessing.thi.de","LIDARProcessing");
        cLIDARProcessing();
        ~cLIDARProcessing() = default;

        tResult ProcessInput(ISampleReader* pReader, const iobject_ptr<const ISample>& pSample) override;
	tResult AcceptType(ISampleReader* pReader, const iobject_ptr<const IStreamType>& pType) override;

    private:
        tFloat some_variable;
/*! Writer for the output video. */
	cSampleWriter* m_pWriterVideo;
	/*! The output format */
	tStreamImageFormat m_OutPinVideoFormat;

	property_variable<tInt> Lambda = 10;
};
