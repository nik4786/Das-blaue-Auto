/*********************************************************************
Master Automatisiertes Fahren und Fahrzeugsicherheit
Gruppenprojekt 1 - AADC
WS 2019/2020
**********************************************************************/

#pragma once

//*************************************************************************************************
#define Add_ID "Add.demo.thi.de"
#include "stdafx.h"
#include <adtffiltersdk/adtf_filtersdk.h>
#include <adtffiltersdk/filter.h>
#include <adtfmediadescription/codec_sample_streamer.h>
#include <aadc.h>

using namespace adtf_util;
using namespace ddl;
using namespace adtf::ucom;
using namespace adtf::base;
using namespace adtf::streaming;
using namespace adtf::mediadescription;
using namespace adtf::filter;
using namespace std;

class Add : public cFilter
{
public:
    ADTF_CLASS_ID_NAME(Add, Add_ID, "Add");

    /*! Default constructor. */
    Add();

    /*! Destructor. */
    virtual ~Add() = default;


    /**
    * Overwrites the Process
    * You need to implement the Reading and Writing of Samples within this function
    * MIND: Do Reading until the Readers queues are empty or use the IPinReader::GetLastSample()
    * This FUnction will be called if the Run() of the TriggerFunction was called.
    */

    tResult ProcessInput(ISampleReader* pReader);
    tTimeStamp GetTime();


private:
    tFloat64 fakt1;
    tFloat64 fakt2;
    tFloat64 erg;
    ISampleWriter* output_writer = nullptr;
    ISampleReader* input_reader1 = nullptr;
    ISampleReader* input_reader2 = nullptr;


};


//*************************************************************************************************
