#include "ArduinoSignalConverter.h"

ADTF_PLUGIN("ArduinoSignalConverterPlugin",cArduinoSigConv);

cArduinoSigConv::cArduinoSigConv()
{
    // konstruktor
    CreateInputPin("tSignalValue",description<tSignalValue>());
    output_writer = CreateOutputPin("f32_output",stream_type_plain<tFloat32>());
    RegisterPropertyVariable("Use Arduino timestamp",m_use_arduino_timestamp);
}

tResult cArduinoSigConv::AcceptType(ISampleReader* pReader, const iobject_ptr<const IStreamType>& pType)
{
    m_sampleFactory = md_sample_data_factory<tSignalValue>(pType);
    RETURN_NOERROR;
}

tResult cArduinoSigConv::ProcessInput(ISampleReader* pReader, const iobject_ptr<const ISample>& pSample)
{
    LOG_INFO("Doing work");
    auto sample = m_sampleFactory.Make(pSample);

    // set sample time to time of meassurement from Arduino
    // because that's the time of the meassurement

    tNanoSeconds time;
    if(m_use_arduino_timestamp)
    {
        time = tNanoSeconds(static_cast<tInt64>(sample.ui32ArduinoTimestamp()));
    }
    else
    {
        time = get_sample_time(pSample);
    }

    output_sample_data<tFloat32> output_sample(time);
    output_sample = sample.f32Value();
    //LOG_INFO("Received value: %f",sample.f32Value());

    output_writer->Write(output_sample.Release());

    RETURN_NOERROR;
}

