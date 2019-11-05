#include "stdafx.h"
#include "SpeedController.h"

ADTF_PLUGIN("SpeedControllerPlugin",cSpeedController);


cSpeedController::cSpeedController()
{
    input_reader1 = CreateInputPin("measured_value",description<tSignalValue>());
    input_reader2 = CreateInputPin("target_value",description<tSignalValue>());
    output_writer = CreateOutputPin("controlled_value",description<tSignalValue>());
    RegisterPropertyVariable("Controller/proportional factor for PI-Controller", m_PIDKp);
    RegisterPropertyVariable("Controller/integral factor for PI-Controller", m_PIDKi);
    RegisterPropertyVariable("Controller/differential factor for PID-Controller", m_PIDKd);
    RegisterPropertyVariable("AWU/minimum output value for the controller [%]", m_PIDMinimumOutput);
    RegisterPropertyVariable("AWU/maximum output value for the controller [%]", m_PIDMaximumOutput);
    RegisterPropertyVariable("AWU/AWU-Constant", m_AWUMaxDiffSum);

}

tResult cSpeedController::ProcessInput(ISampleReader* pReader, const iobject_ptr<const ISample>& pSample)
{
    tFloat64 m_PIDResult = 0;
    tfloat64 m_currentTime = 0;
    tfloat64 m_LastUpdateTime = 0;

    adtf::streaming::input_sample_data<tInt16> input_sample(pSample);
    input_reader1->Read(input_sample.Release());
    input_reader2->Read(input_sample.Release());

    

        
    // calculating dt for I-Component
    m_currentTime = GetTime();
    float dt = (m_currentTime - m_LastUpdateTime)
    if (std::abs(dt)>1)
        dt = 0;
    m_LastUpdateTime = m_currentTime;

    // calculating error out of required target_value and actual measured_value
    m_error = (target_value - measured_value);
    // calculating accumulated error for I-Component
    m_accumulatedError += (m_error*dt);

    // Anti-Wind up
    if(m_f64accumulatedError > m_AWUMaxDiffSum)
        m_f64accumulatedError = m_AWUMaxDiffSum;
    else if(m_f64accumulatedError < -m_AWUMaxDiffSum)
        m_f64accumulatedError = m_AWUMaxDiffSum;

    // PI-Controller
    m_PIDResult = (m_PIDKp * m_error) + (m_PIDKi * m_accumulatedError) + (m_PIDKd * (m_error-m_erroralt)/dt); 

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


    LOG_INFO("output_P_Component: %d", m_PIKp*m_error);
    LOG_INFO("output_I_Component: %d", m_PIDKi * m_accumulatedError);
    LOG_INFO("output_D_Component: %d", m_PIDKd * (m_error-m_erroralt)/dt);
 

    adtf::streaming::output_sample_data<tInt16> output_sample(pSample);
    LOG_INFO("PID Result: %d", m_PIDResult);
    output_sample = m_PIDResult;

    output_writer->Write(output_sample.Release());

    RETURN_NOERROR;

}


tTimeStamp cWheelSpeedController::GetTime()
{
    return adtf_util::cHighResTimer::GetTime();
}
