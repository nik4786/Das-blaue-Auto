#include "MovingAvgFilter.h"

ADTF_PLUGIN("MovingAvgPlugin",cMovingAvgFilter);

cMovingAvgFilter::cMovingAvgFilter()
{
    // konstruktor
    output_writer = CreateOutputPin("data",description<tGaussianVariable>());

    RegisterPropertyVariable("WindowSize", m_windowsize);
}

cMovingAvgFilter::~cMovingAvgFilter()
{
    delete m_buffer;
}

tResult cMovingAvgFilter::Init(tInitStage eStage)
{
    if(eStage == StageNormal)
    {
        if(m_windowsize<=0)
        {
            LOG_ERROR("WindowSize must be greater or equal 1");
            RETURN_ERROR(ERR_INVALID_ARG);
        }
        m_buffer = new boost::circular_buffer<tFloat32>(m_windowsize);
    }

    RETURN_NOERROR;
}

tResult cMovingAvgFilter::ProcessInput(ISampleReader* pReader, const iobject_ptr<const ISample>& pSample)
{
    auto data = sample_data<tFloat32>(pSample);

    
    if(m_buffer->full())
    {
        m_sum -= m_buffer->front();
        m_stdSum -= m_buffer->front()*m_buffer->front();
        m_buffer->pop_front();
    }


    m_buffer->push_back(data);
    m_sum += data;
    m_stdSum += data*data;

    output_sample_data<tGaussianVariable> output_data(get_sample_time(pSample));
    tFloat32 avg = static_cast<tFloat32>(m_sum / m_buffer->size());
    output_data->mean = avg;
    output_data->stddev = static_cast<tFloat32>(std::sqrt(m_stdSum/m_buffer->size() - avg*avg));

    output_writer->Write(output_data.Release());
    // tFloat32 stdSum = 0;
    // // calculate std
    // for(tInt i = 0;i<m_buffer->size();++i)
    // {
    //     stdSum += ((*m_buffer)[i]-avg)*((*m_buffer)[i]-avg);
    // }
    // tFloat32 std = std::sqrt(stdSum);



    RETURN_NOERROR;
}

