#pragma once


//*************************************************************************************************
#define TESTLONGLAT_FILTER_ID "TestLongLat.demo.thi.de"
#include "stdafx.h"
#include <adtffiltersdk/adtf_filtersdk.h>
#include <adtffiltersdk/filter.h>
#include <aadc.h>
#include <adtfmediadescription/codec_sample_streamer.h>

using namespace adtf_util;
using namespace adtf::base;
using namespace adtf::streaming;
using namespace adtf::filter;
using namespace ddl;
using namespace adtf::mediadescription;


class cTestLongLat : public cFilter
{
    public:
        ADTF_CLASS_ID_NAME(cTestLongLat,TESTLONGLAT_FILTER_ID,"TestLongLat");
        
	// Constructor
        cTestLongLat();
	
	// Destructor
        ~cTestLongLat() = default;
	
	// Initialization of variables
 	tResult Init(tInitStage eStage) override;

	// Processing 
        tResult Process(tNanoSeconds tmTimeOfTrigger,
                IRunner* pRunner) override;
        
        
    
    private:
        
          /*! A signal value identifier. */
        struct tSignalValueId
        {
            tSize timeStamp;
            tSize value;
        } m_ddlSignalValueId;
	
	// declaration of variables
        tInt32 m_count = 0;
	tInt32 m_time_threshold_0 = 100;
        tInt32 m_time_threshold_1 = 0;
        tInt32 m_time_threshold_2 = 0;
        tInt32 m_time_threshold_3 = 0;
        tInt32 m_time_threshold_4 = 0;
        tFloat64 m_desired_value = 0;
        tFloat64 m_last_desired_value = 0;
	
	// declaration of property variables
        adtf::base::property_variable<tFloat64>    m_desired_value_1 = 3;
        adtf::base::property_variable<tFloat64>    m_desired_value_2 = 1.5;
        adtf::base::property_variable<tFloat64>    m_desired_value_3 = 3;
        adtf::base::property_variable<tFloat64>    m_desired_value_4 = 1.5;
        adtf::base::property_variable<tFloat64>    m_time_int_1 = 10;
        adtf::base::property_variable<tFloat64>    m_time_int_2 = 10;
        adtf::base::property_variable<tFloat64>    m_time_int_3 = 10;
        adtf::base::property_variable<tFloat64>    m_time_int_4 = 10;

	
	// declaration of writer
        ISampleWriter* output_writer = nullptr;

        tTimeStamp GetTime();
};
