//some basic includes (math.h for calcualtion)
#include "TraceCalQF.h"
#include <functional>
#include <math.h>

ADTF_PLUGIN("TraceCalQFPlugin",TraceCalQF);

//create the in- and output with the described pins
TraceCalQF::TraceCalQF()
{
    input_reader1 = CreateInputPin("LaneInput", description<tSignalValue>());
    output_writer = CreateOutputPin("SollPos",description<tSignalValue>());
}

tResult TraceCalQF::ProcessInput(adtf::base::tNanoSeconds tmTrigger,ISampleReader* pReader)
{

    object_ptr<const ISample> pSample1;

    //check if the sample is valid
    if (IS_OK(input_reader1->GetLastSample(pSample1)))
    {
        LaneInput = sample_data<tSignalValue>(pSample1)->f32Value;
    }
    
    //logic here
    SollPos = LaneInput;

    //releas the calc data
    adtf::streaming::output_sample_data<tSignalValue> output_sample(tmTrigger);
    LOG_INFO("SollPos: %d", SollPos);
    output_sample->f32Value = SollPos;
    output_writer->Write(output_sample.Release());

    RETURN_NOERROR;
}
