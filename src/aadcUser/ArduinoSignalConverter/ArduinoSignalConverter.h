#pragma once

#include <adtffiltersdk/adtf_filtersdk.h>
#include <aadc.h>


using namespace adtf::base;
using namespace adtf::streaming;
using namespace adtf::filter;
using namespace adtf::mediadescription;

class cArduinoSigConv : public cFilter
{
    public:
        ADTF_CLASS_ID_NAME(cArduinoSigConv, "arduinoSigConv.common.thi.de","Arduino tSignal Converter");
        cArduinoSigConv();
        ~cArduinoSigConv() = default;

        tResult AcceptType(ISampleReader* pReader, const iobject_ptr<const IStreamType>& pType) override;
        tResult ProcessInput(ISampleReader* pReader, const iobject_ptr<const ISample>& pSample) override;

    private:
        md_sample_data_factory<tSignalValue> m_sampleFactory;
        property_variable<tBool> m_use_arduino_timestamp = false;
        ISampleWriter* output_writer = nullptr;
};