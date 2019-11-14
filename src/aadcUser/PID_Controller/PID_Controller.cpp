﻿#include "PID_Controller.h"
#include <functional>

ADTF_PLUGIN("PID_ControllerPlugin",PID_Controller);


PID_Controller::PID_Controller()
{
    input_reader1 = CreateInputPin("measured_value", stream_type_plain<tFloat32>());
    input_reader2 = CreateInputPin("target_value", stream_type_plain<tFloat32>());
    output_writer = CreateOutputPin("controlled_value",stream_type_plain<tFloat32>());
    RegisterPropertyVariable("Controller/proportional factor for PI-Controller", m_PIDKp);
    RegisterPropertyVariable("Controller/integral factor for PI-Controller", m_PIDKi);
    RegisterPropertyVariable("Controller/differential factor for PID-Controller", m_PIDKd);
    RegisterPropertyVariable("AWU/minimum output value for the controller [%]", m_PIDMinimumOutput);
    RegisterPropertyVariable("AWU/maximum output value for the controller [%]", m_PIDMaximumOutput);
    //RegisterPropertyVariable("AWU/AWU-DiffSum", m_AWUMaxDiffSum); max/min output value of AWU operation, for later integration
}

tResult PID_Controller::ProcessInput(ISampleReader* pReader)
{
    tFloat64 m_PIDResult = 0;
    tFloat64 m_currentTime = 0;
    tFloat64 m_LastUpdateTime = 0;

    object_ptr<const ISample> pSample1;
    object_ptr<const ISample> pSample2;

    if (IS_OK(input_reader1->GetLastSample(pSample1)) && IS_OK(input_reader2->GetLastSample(pSample2)))
    {
        target_value = sample_data<tFloat32>(pSample1);
        measured_value = sample_data<tFloat32>(pSample2);
    }
        
    // calculating dt for I-Component
    m_currentTime = GetTime();
    tFloat64 dt = (m_currentTime - m_LastUpdateTime);
    //if (std::abs(dt)>1)
    //    dt = 0; 							wieso wirds hier immer auf 0 gesetzt wenn >1 ?
    m_LastUpdateTime = m_currentTime;

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

    // check, if fixed limit is reached
    if(m_PIDResult > m_PIDMaximumOutput)
    {
        m_PIDResult = m_PIDMaximumOutput;
    }
    else if(m_PIDResult < m_PIDMinimumOutput)
    {
        m_PIDResult = m_PIDMinimumOutput;
    }

    LOG_INFO("output_P_Component: %d", m_PIDKp);
    LOG_INFO("output_I_Component: %d", m_PIDKi);
    LOG_INFO("output_D_Component: %d", m_PIDKd);
 
    adtf::streaming::output_sample_data<tFloat32> output_sample(m_PIDResult);
    LOG_INFO("PID Result: %d", m_PIDResult);

    output_writer->Write(output_sample.Release());

    RETURN_NOERROR;
}

tTimeStamp PID_Controller::GetTime()
{
    return adtf_util::cHighResTimer::GetTime();
}
