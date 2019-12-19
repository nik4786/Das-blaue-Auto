
#pragma once

//*************************************************************************************************
//Some basic includes for ADTF and AADC
#define TraceCalQF_ID "TraceCalQF.demo.thi.de"
#include "stdafx.h"
#include <adtffiltersdk/adtf_filtersdk.h>
#include <adtffiltersdk/filter.h>
#include <adtfmediadescription/codec_sample_streamer.h>
#include <aadc.h>

//namespaces so you can use them dircet in the code
using namespace adtf::util;
using namespace ddl;
using namespace adtf::ucom;
using namespace adtf::base;
using namespace adtf::streaming;
using namespace adtf::filter;
using namespace adtf::mediadescription;
using namespace std;

class TraceCalQF : public cFilter
{
    //Filter and method is public
    public:
        ADTF_CLASS_ID_NAME(TraceCalQF,TraceCalQF_ID,"TraceCalQF");
        
        TraceCalQF();

        tResult ProcessInput(tNanoSeconds tmTrigger, ISampleReader* pReader);

    //The variables are privte
    private:
        //Define in- and output
        ISampleWriter* output_writer = nullptr;
        ISampleReader* input_reader1 = nullptr;

        //Create vars for the method(s)
        tFloat64 LaneInput;
        tFloat64 SollPos;

};
