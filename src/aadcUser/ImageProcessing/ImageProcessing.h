#pragma once

#include "stdafx.h"
#include <adtffiltersdk/adtf_filtersdk.h>


using namespace adtf::base;
using namespace adtf::streaming;
using namespace adtf::filter;
using namespace adtf::mediadescription;
using namespace adtf::util;
using namespace adtf::ucom;
using namespace adtf::system;

using namespace cv;
using namespace std;

#include <ADTF3_OpenCV_helper.h>


class cImageProcessing : public cFilter
{
    public:
        ADTF_CLASS_ID_NAME(cImageProcessing, "cImageProcessing.thi.de","cImageProcessing");
        cImageProcessing();
        ~cImageProcessing() = default;

        tResult AcceptType(ISampleReader* pReader, const iobject_ptr<const IStreamType>& pType) override;
            tResult ProcessInput(tNanoSeconds /*tmTrigger*/, ISampleReader* /*pReader*/) override;
            tInt8 get_user_channel_selection_value(cString colorchannel);


    private:
   //Stream Formats
        /*! The input format */
   // adtf::streaming::tStreamImageFormat m_sImageFormat;

    /*! The clock */
	 adtf::ucom::object_ptr<adtf::services::IReferenceClock> m_pClock;
	
    
    //Pins
	/*! Reader for the input video. */
	ISampleReader* m_pReaderVideo;
	/*! Writer for the output video. */
	cSampleWriter* m_pWriterVideo;
	//Stream Formats
	/*! The input format */
	tStreamImageFormat m_InPinVideoFormat;
	/*! The output format */
	tStreamImageFormat m_OutPinVideoFormat;


    property_variable<tBool> m_ActiveGaussianBlur = false;
    property_variable<tInt> m_GaussianBlurkSize = 3;
    property_variable<tBool> m_ActiveColorSpace = false;	
    property_variable<cString> m_colorspace = cString("");
    property_variable<tBool> m_ActiveChannelSelect = false;
    property_variable<tInt> m_channelselect;
    property_variable<tBool> m_ActiveRangeMask = false;
    property_variable<tInt> m_lowerthreshold_1 = 0;
    property_variable<tInt> m_lowerthreshold_2 = 0;
    property_variable<tInt> m_lowerthreshold_3 = 0;
    property_variable<tInt> m_upperthreshold_1 = 255;
    property_variable<tInt> m_upperthreshold_2 = 255;
    property_variable<tInt> m_upperthreshold_3 = 255;


};

