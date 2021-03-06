//see TraceCalQF.h for general coments in header ;) thanks!
#pragma once

//*************************************************************************************************
#define TraceClassQF_ID "TraceClassQF.demo.thi.de"
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
using namespace std;

class TraceClassQF : public cFilter
{
    public:
        ADTF_CLASS_ID_NAME(TraceClassQF,TraceClassQF_ID,"TraceClassQF");
        
        TraceClassQF();

        tResult ProcessInput(tNanoSeconds tmTrigger, ISampleReader* pReader);

    private:
        ISampleWriter* output_writer = nullptr;
        ISampleReader* input_reader1 = nullptr;
        ISampleReader* input_reader2 = nullptr;
        tFloat64 LaneInput;
        tFloat64 SollPos;
        tFloat64 TraceClass;

};
