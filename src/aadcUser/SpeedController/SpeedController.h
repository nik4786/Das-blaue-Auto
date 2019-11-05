
#pragma once


//*************************************************************************************************
#define SPEEDCONTROLLER_ID "speedcontroller.demo.thi.de"
//#include "stdafx.h"
#include <adtffiltersdk/adtf_filtersdk.h>
#include <adtffiltersdk/filter.h>
//#include <adtfmediadescription/codec_sample_streamer.h>
#include <aadc.h>

using namespace adtf::base;
using namespace adtf::streaming;
using namespace adtf::filter;
using namespace adtf::mediadescription;

class cSpeedController : public cFilter
{
    public:
        ADTF_CLASS_ID_NAME(cSpeedController,SPEEDCONTROLLER_ID,"SpeedController");
        
        cSpeedController();

        ~cSpeedController() = default;

        tResult ProcessInput(ISampleReader* pReader, const iobject_ptr<const ISample>& pSample) override;
        //tResult ProcessInput(tNanoSeconds timestamp, ISampleReader* reader ) override;
    
    private:

        tfloat64 m_erroralt = 0;
        tfloat64 m_accumulatedError = 0;
        tfloat64 m_error = 0;
        tfloat64 m_PIDKp = 0;
        tfloat64 m_PIDKi = 0;
        tfloat64 m_PIDKd = 0;
        tfloat64 m_PIDMinimumOutput = 0;
        tfloat64 m_PIDMaximumOutput = 0;
        tfloat64 m_AWUMaxDiffSum = 0;
        tfloat64 m_error = 0;
        ISampleWriter* output_writer = nullptr;
        ISampleReader* input_reader1 = nullptr;
        ISampleReader* input_reader2 = nullptr;
    


};

//*************************************************************************************************
