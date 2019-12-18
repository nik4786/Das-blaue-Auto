#include "cTestLongLat.h"
ADTF_PLUGIN("TestLongLat",
            cTestLongLat);


cTestLongLat::cTestLongLat()
{
    RegisterPropertyVariable("time intervall 1 [s]", m_time_int_1);
    RegisterPropertyVariable("time intervall 2 [s]", m_time_int_2);
    RegisterPropertyVariable("time intervall 3 [s]", m_time_int_3);
    RegisterPropertyVariable("time intervall 4 [s]", m_time_int_4);
    RegisterPropertyVariable("desired value 1", m_desired_value_1);
    RegisterPropertyVariable("desired value 2", m_desired_value_2);
    RegisterPropertyVariable("desired value 3", m_desired_value_3);
    RegisterPropertyVariable("desired value 4", m_desired_value_4);

    // Input of TimerRunner
    CreateRunner("step");

    output_writer = CreateOutputPin("desired value",description<tSignalValue>());

}

//Initialization of further variables
tResult cTestLongLat::Init(tInitStage eStage)
{
    if (eStage == StageNormal)
    {
        //TimerRunner property must be the init value 100000 ms
        //computing the threshold values for the counter
        m_time_threshold_1 =  m_time_threshold_0 + static_cast<tInt32>(m_time_int_1*10);
        m_time_threshold_2 =  m_time_threshold_0 + static_cast<tInt32>(m_time_int_1*10 + m_time_int_2*10);
        m_time_threshold_3 =  m_time_threshold_0 + static_cast<tInt32>(m_time_int_1*10 + m_time_int_2*10 + m_time_int_3*10);
        m_time_threshold_4 =  m_time_threshold_0 + static_cast<tInt32>(m_time_int_1*10 + m_time_int_2*10 + m_time_int_3*10 + m_time_int_4*10);
    }
    return cFilter::Init(eStage);
}

tResult cTestLongLat::Process(adtf::base::tNanoSeconds tmTrigger, adtf::streaming::IRunner* /*pRunner*/)
{
    ++m_count;

    LOG_INFO("m_time_int_1 = %f", m_time_int_1);
    LOG_INFO("counter = %d", m_count);
    LOG_INFO("m_time_threshold_1 = %d", m_time_threshold_1);


    //counting until reaching the respective time threshold
    if (m_count <= m_time_threshold_0)
    {
        // pre-phase for arduino init of speed value
        // important -> otherwise no function in first time intervall
        m_desired_value = 0;
    }
    else if (m_count <= m_time_threshold_1)
    {
        m_desired_value = m_desired_value_1;
    }
    else if (m_count <= m_time_threshold_2)
    {
        m_desired_value = m_desired_value_2;
    }
    else if (m_count <= m_time_threshold_3)
    {
        m_desired_value = m_desired_value_3;
    }
    else if (m_count <= m_time_threshold_4)
    {
        m_desired_value = m_desired_value_4;
    }
    else
    {
        m_desired_value = 0;
    }
    // return value only at the last update of the desired value
    if (m_desired_value != m_last_desired_value)
    {
        LOG_INFO("Writing...");
        adtf::streaming::output_sample_data<tSignalValue> output_sample(tmTrigger);
        output_sample->f32Value = static_cast<tFloat32>(m_desired_value);
        output_sample->ui32ArduinoTimestamp = GetTime();
        output_writer->Write(output_sample.Release());
    }

    m_last_desired_value = m_desired_value;

    RETURN_NOERROR;
}

tTimeStamp cTestLongLat::GetTime()
{
    return adtf_util::cHighResTimer::GetTime();
}
