//see TraceCalQF.cpp for general comments ;) thanks!
#include "PredictQF.h"
#include <functional>
#include <math.h>

ADTF_PLUGIN("PredictQFPlugin",PredictQF);


PredictQF::PredictQF()
{
    input_reader1 = CreateInputPin("LaneInput", description<tSignalValue>());
    input_reader2 = CreateInputPin("PrevLaneInput", description<tSignalValue>());
    input_reader3 = CreateInputPin("Speed", description<tSignalValue>());
    output_writer = CreateOutputPin("IstPos",description<tSignalValue>());
}

tResult PredictQF::ProcessInput(adtf::base::tNanoSeconds tmTrigger,ISampleReader* pReader)
{

    object_ptr<const ISample> pSample1;
    object_ptr<const ISample> pSample2;
    object_ptr<const ISample> pSample3;

    if (IS_OK(input_reader1->GetLastSample(pSample1)) &&
        IS_OK(input_reader2->GetLastSample(pSample2)) &&
        IS_OK(input_reader3->GetLastSample(pSample3))
        )
    {
        LaneInput = sample_data<tSignalValue>(pSample1)->f32Value;
        PrevLaneInput = sample_data<tSignalValue>(pSample2)->f32Value;
        Speed = sample_data<tSignalValue>(pSample3)->f32Value;
    }
    
    //logic here
    IstPos = LaneInput * PrevLaneInput * Speed;

    adtf::streaming::output_sample_data<tSignalValue> output_sample(tmTrigger);
    LOG_INFO("IstPos: %d", IstPos);
    output_sample->f32Value = IstPos;
    output_writer->Write(output_sample.Release());

    RETURN_NOERROR;
}
