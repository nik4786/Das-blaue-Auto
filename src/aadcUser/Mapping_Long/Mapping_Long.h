
#pragma once

//*************************************************************************************************
#define MAPPING_LONG_ID "Mapping_Long.demo.thi.de"
#include "stdafx.h"
#include <adtffiltersdk/adtf_filtersdk.h>
#include <adtffiltersdk/filter.h>
#include <adtfmediadescription/codec_sample_streamer.h>
#include <aadc.h>

using namespace adtf::util;
using namespace ddl;
using namespace adtf::ucom;
using namespace adtf::base;
using namespace adtf::streaming;
using namespace adtf::filter;
using namespace adtf::mediadescription;
//using namespace adtf::services::ant;

class Mapping_Long : public cFilter
{
    public:
        ADTF_CLASS_ID_NAME(Mapping_Long ,MAPPING_LONG_ID,"Mapping Long");
        
	// Constructor
        Mapping_Long();
	
	//Processing
        tResult ProcessInput(tNanoSeconds tmTrigger, ISampleReader* pReader);
        
        //virtual tTimeStamp GetTime();

    private:
        
	// Declaration of variables
        tFloat64 m_controlled_speed = 0;
        tFloat64 desired_speed_percent = 0;
        tFloat64 input_pid = 0;
        tFloat64 m_MinimumOutput = -100;
        tFloat64 m_MaximumOutput = 100;
	
	// Declaration of Writer and Readers
        ISampleWriter* output_writer = nullptr;
        ISampleReader* input_reader1 = nullptr;
        ISampleReader* input_reader2 = nullptr;
};

//*************************************************************************************************
