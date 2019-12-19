/*********************************************************************
Master Automatisiertes Fahren und Fahrzeugsicherheit
Gruppenprojekt 1 - AADC
WS 2019/2020
**********************************************************************/


#include "Precontrol.h"


ADTF_PLUGIN("PrecontrolPlugin", Precontrol);


Precontrol::Precontrol()
{
    //create input and output pin
    input_reader1 = CreateInputPin("target_value", stream_type_plain<tFloat32>());
    output_writer = CreateOutputPin("result_value",stream_type_plain<tFloat32>());
    RegisterPropertyVariable("track_model/factor", m_tm_factor);
}


tResult Precontrol::ProcessInput(ISampleReader* pReader)
{
	//load input pin
    object_ptr<const ISample> pSample1;
    object_ptr<const ISample> pSample2;

    if (IS_OK(input_reader1->GetLastSample(pSample1)))
    {
        target_value = sample_data<tFloat32>(pSample1);
    }

    //calculation of output
	//if you want to implement a track model, here is the place
    erg = target_value * m_tm_factor;

    //create output
    adtf::streaming::output_sample_data<tFloat32> output_sample(erg);
    LOG_INFO("Add Result: %d", erg);

    output_writer->Write(output_sample.Release());
    
    RETURN_NOERROR;
}
