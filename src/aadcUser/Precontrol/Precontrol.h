/*********************************************************************
Master Automatisiertes Fahren und Fahrzeugsicherheit
Gruppenprojekt 1 - AADC
WS 2019/2020
**********************************************************************/

#pragma once

//*************************************************************************************************
#define Precontrol_ID "precontrol.demo.thi.de"
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

class Precontrol : public cFilter
{
public:
    ADTF_CLASS_ID_NAME(Precontrol, Precontrol_ID, "Precontrol");

    /*! Default constructor. */
    Precontrol();

    /*! Destructor. */
    virtual ~Precontrol() = default;


    /**
    * Overwrites the Process
    * You need to implement the Reading and Writing of Samples within this function
    * MIND: Do Reading until the Readers queues are empty or use the IPinReader::GetLastSample()
    * This FUnction will be called if the Run() of the TriggerFunction was called.
    */

    tResult ProcessInput(ISampleReader* pReader);
    tTimeStamp GetTime();


private:
    
    property_variable<tFloat64> m_tm_factor = 1;
    tFloat64 target_value;
    tFloat64 erg;

    ISampleWriter* output_writer = nullptr;
    ISampleReader* input_reader1 = nullptr;
    


};


//*************************************************************************************************
