#pragma once

#include <adtffiltersdk/adtf_filtersdk.h>
#include <boost/circular_buffer.hpp>
#include <owndesc.h>

using namespace adtf::base;
using namespace adtf::streaming;
using namespace adtf::filter;
using namespace adtf::mediadescription;

class cMovingAvgFilter : public cFilter
{
    public:
        ADTF_CLASS_ID_NAME(cMovingAvgFilter, "movingavg.common.thi.de","Moving Average");
        cMovingAvgFilter();
        ~cMovingAvgFilter();


        tResult Init(tInitStage eStage) override;
        tResult ProcessInput(ISampleReader* pReader, const iobject_ptr<const ISample>& pSample) override;

    private:
        ISampleWriter* output_writer = nullptr;

        property_variable<tInt> m_windowsize = 5;
        tFloat m_sum = 0;
        tFloat m_stdSum = 0;

        boost::circular_buffer<tFloat32>* m_buffer = nullptr;
};