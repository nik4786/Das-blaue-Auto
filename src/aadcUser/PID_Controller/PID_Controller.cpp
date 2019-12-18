#include "PID_Controller.h"
#include <functional>

ADTF_PLUGIN("PID_ControllerPlugin",PID_Controller);


PID_Controller::PID_Controller()
{
    input_reader1 = CreateInputPin("measured_value", description<tSignalValue>());
    input_reader2 = CreateInputPin("target_value", description<tSignalValue>());
    output_writer = CreateOutputPin("controlled_value",description<tSignalValue>());
    RegisterPropertyVariable("Controller/proportional factor for PI-Controller", m_PIDKp);
    RegisterPropertyVariable("Controller/integral factor for PI-Controller", m_PIDKi);
    RegisterPropertyVariable("Controller/differential factor for PID-Controller", m_PIDKd);
    RegisterPropertyVariable("AWU/minimum output value for the controller [%]", m_PIDMinimumOutput);
    RegisterPropertyVariable("AWU/maximum output value for the controller [%]", m_PIDMaximumOutput);
    //RegisterPropertyVariable("AWU/AWU-DiffSum", m_AWUMaxDiffSum); max/min output value of AWU operation, for later integration
    
    
}

tResult PID_Controller::ProcessInput(tNanoSeconds tmTrigger, ISampleReader* pReader)
{

    object_ptr<const ISample> pSample;

    if (pReader == input_reader1)
    {
        if (IS_OK(input_reader1->GetLastSample(pSample)))
        {
            measured_value = sample_data<tSignalValue>(pSample)->f32Value;
        }
    }
    if (pReader == input_reader2)
    {
        if (IS_OK(input_reader2->GetLastSample(pSample)))
        {
            target_value = sample_data<tSignalValue>(pSample)->f32Value;
        }
    }

    LOG_INFO("measured value = %f and target value = %f", measured_value, target_value);

    // calculating delta t for I-Component

    m_currentTime = clock();
    LOG_INFO("currentTime: %f", m_currentTime);
    tFloat64 dt = (static_cast<tFloat64>(m_currentTime - m_LastUpdateTime))/CLOCKS_PER_SEC;
    if (std::abs(dt)>1)
    {
        dt = 0;
    }
    m_LastUpdateTime = m_currentTime;
    LOG_INFO("dt = %f", dt);

    // calculating error out of required target_value and actual measured_value
    m_error = (target_value - measured_value);

    // calculating accumulated error for I-Component
    m_accumulatedError += (m_error*dt);

    // Anti-Wind up
    if(m_accumulatedError > m_PIDMaximumOutput)
        m_accumulatedError = m_PIDMaximumOutput;
    else if(m_accumulatedError < m_PIDMinimumOutput)
        m_accumulatedError = m_PIDMinimumOutput;

    // PID-Controller
    if (dt > 0) 
		m_PIDResult = (m_PIDKp * m_error) + (m_PIDKi * m_accumulatedError) + (m_PIDKd * (m_error-m_erroralt)/dt);
	else	// catch exception (for example in first run) (/0 -> NaN)
		m_PIDResult = (m_PIDKp * m_error) + (m_PIDKi * m_accumulatedError);

    m_erroralt = m_error;

    m_PIDResult = m_PIDResult;

    // Check, if fixed limit is reached
    if(m_PIDResult > m_PIDMaximumOutput)
    {
        m_PIDResult = m_PIDMaximumOutput;
    }
    else if(m_PIDResult < m_PIDMinimumOutput)
    {
        m_PIDResult = m_PIDMinimumOutput;
    }

    // Wirting of the controller output
    adtf::streaming::output_sample_data<tSignalValue> output_sample(tmTrigger);
    LOG_INFO("PID Result: %f", m_PIDResult);
    output_sample->f32Value = m_PIDResult;
    output_sample->ui32ArduinoTimestamp = clock();
    output_writer->Write(output_sample.Release());

    RETURN_NOERROR;
}

/*tTimeStamp PID_Controller::GetTime()
{
    return adtf::services::ant::IReferenceClock::GetTime();
}*/
