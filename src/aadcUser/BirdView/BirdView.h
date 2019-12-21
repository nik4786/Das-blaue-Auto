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


class cBirdView : public cFilter
{
    public:
        ADTF_CLASS_ID_NAME(cBirdView, "cBirdView.thi.de","cBirdView");
        cBirdView();
        ~cBirdView() = default;

        tResult AcceptType(ISampleReader* pReader, const iobject_ptr<const IStreamType>& pType) override;
            tResult ProcessInput(tNanoSeconds /*tmTrigger*/, ISampleReader* /*pReader*/) override;



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

    //define and set default values for parameter
    property_variable<tBool> m_bSymmetry = tTrue;	
    property_variable<tInt> m_Point0_x = 467;
    property_variable<tInt> m_Point0_y = 516;
    property_variable<tInt> m_Point1_x = 741;
    property_variable<tInt> m_Point1_y = 516;
    property_variable<tInt> m_Point2_x = 1096;
    property_variable<tInt> m_Point2_y = 658;
    property_variable<tInt> m_Point3_x = 30;
    property_variable<tInt> m_Point3_y = 658;

};

