/*********************************************************************
Master Automatisiertes Fahren und Fahrzeugsicherheit
Gruppenprojekt 1 - AADC
WS 2019/2020
**********************************************************************/


#include "Add.h"


ADTF_PLUGIN("AddPlugin", Add);


Add::Add()
{
	//create input and output pin
    input_reader1 = CreateInputPin("faktor1", stream_type_plain<tFloat32>());
    input_reader2 = CreateInputPin("faktor2", stream_type_plain<tFloat32>());
    output_writer = CreateOutputPin("result",stream_type_plain<tFloat32>());
}


tResult Add::ProcessInput(ISampleReader* pReader)
{
	//load input pins
    object_ptr<const ISample> pSample1;
    object_ptr<const ISample> pSample2;

    if (IS_OK(input_reader1->GetLastSample(pSample1)) && IS_OK(input_reader2->GetLastSample(pSample2)))
    {
        fakt1 = sample_data<tFloat32>(pSample1);
        fakt2 = sample_data<tFloat32>(pSample2);
    }

    //calculation of output
    erg = fakt1 + fakt2;

    //create output
    adtf::streaming::output_sample_data<tFloat32> output_sample(erg);
    LOG_INFO("Add Result: %d", erg);

    output_writer->Write(output_sample.Release());
    
    RETURN_NOERROR;
}
