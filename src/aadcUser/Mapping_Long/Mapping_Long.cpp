#include "Mapping_Long.h"
#include <functional>

ADTF_PLUGIN("Mapping_LongPlugin", Mapping_Long);


Mapping_Long::Mapping_Long()
{
    input_reader1 = CreateInputPin("desired speed m_s", description<tSignalValue>());
    input_reader2 = CreateInputPin("PID", description<tSignalValue>());
    output_writer = CreateOutputPin("controlled speed",description<tSignalValue>());
}

tResult Mapping_Long::ProcessInput(tNanoSeconds tmTrigger, ISampleReader* pReader)
{

    object_ptr<const ISample> pSample;

    // Returend value of the respective Reader
    if (pReader == input_reader1)
    {
        if (IS_OK(input_reader1->GetLastSample(pSample)))
        {
            // Mapping: m/s -> %
            desired_speed_percent = 100 / 6 * sample_data<tSignalValue>(pSample)->f32Value;
        }
    }
    if (pReader == input_reader2)
    {
        if (IS_OK(input_reader2->GetLastSample(pSample)))
        {
            //Control the error of the velocity
            input_pid = sample_data<tSignalValue>(pSample)->f32Value;
        }
    }

    LOG_INFO("desired speed in percent = %f und input pid = %f", desired_speed_percent, input_pid);

    // Addition of both inputs
    m_controlled_speed = desired_speed_percent + input_pid;

    // Limiting the output
    if(m_controlled_speed > m_MaximumOutput)
    {
        m_controlled_speed = m_MaximumOutput;
    }
    else if(m_controlled_speed < m_MinimumOutput)
    {
        m_controlled_speed = m_MinimumOutput;
    }
    // Writing the output
    adtf::streaming::output_sample_data<tSignalValue> output_sample(tmTrigger);
    LOG_INFO("controlled speed: %f", m_controlled_speed);
    output_sample->f32Value = m_controlled_speed;
    output_sample->ui32ArduinoTimestamp = clock();
    output_writer->Write(output_sample.Release());

    RETURN_NOERROR;
}

/*tTimeStamp Mapping_Long::GetTime()
{
    return adtf::services::ant::IReferenceClock::GetTime();
}*/
