
#pragma once

//*************************************************************************************************
#define PID_CONTROLLER_ID "PID_Controller.demo.thi.de"
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

class PID_Controller : public cFilter
{
    public:
        ADTF_CLASS_ID_NAME(PID_Controller,PID_CONTROLLER_ID,"PID_Controller");
        
        PID_Controller();

        tResult ProcessInput(ISampleReader* pReader);
        
        tTimeStamp GetTime();

    private:
        tFloat64 m_erroralt = 0;
        tFloat64 m_accumulatedError = 0;
        tFloat64 m_error = 0;
        tFloat64 target_value;
        tFloat64 measured_value;
        property_variable<tFloat64> m_PIDKp = 1;
        property_variable<tFloat64> m_PIDKi = 0.1;
        property_variable<tFloat64> m_PIDKd = 0;
        property_variable<tFloat64> m_PIDMinimumOutput = -100;
        property_variable<tFloat64> m_PIDMaximumOutput = 100;
        //property_variable<tFloat64> m_AWUMaxDiffSum = 0; see .cpp
        ISampleWriter* output_writer = nullptr;
        ISampleReader* input_reader1 = nullptr;
        ISampleReader* input_reader2 = nullptr;
};
