//see TraceCalQF.cpp for general comments ;) thanks!
#include "TraceClassQF.h"
#include <functional>
#include <math.h>

ADTF_PLUGIN("TraceClassQFPlugin",TraceClassQF);


TraceClassQF::TraceClassQF()
{
    input_reader1 = CreateInputPin("LaneInput", description<tSignalValue>());
    input_reader2 = CreateInputPin("SollPos", description<tSignalValue>());
    output_writer = CreateOutputPin("TraceClass",description<tSignalValue>());
}

tResult TraceClassQF::ProcessInput(adtf::base::tNanoSeconds tmTrigger,ISampleReader* pReader)
{

    object_ptr<const ISample> pSample1;
    object_ptr<const ISample> pSample2;

    if (IS_OK(input_reader1->GetLastSample(pSample1)) && IS_OK(input_reader2->GetLastSample(pSample1)))
    {
        LaneInput = sample_data<tSignalValue>(pSample1)->f32Value;
        SollPos = sample_data<tSignalValue>(pSample2)->f32Value;
    }

    //add here logic
    TraceClass = LaneInput + SollPos / 2;
    

    adtf::streaming::output_sample_data<tSignalValue> output_sample(tmTrigger);
    LOG_INFO("Output", TraceClass);
    output_sample->f32Value = TraceClass;
    output_writer->Write(output_sample.Release());

    RETURN_NOERROR;
}
