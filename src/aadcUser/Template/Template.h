#pragma once

#include <adtffiltersdk/adtf_filtersdk.h>
#include <aadc.h>


using namespace adtf::base;
using namespace adtf::streaming;
using namespace adtf::filter;
using namespace adtf::mediadescription;

class cTemplate : public cFilter
{
    public:
        ADTF_CLASS_ID_NAME(cTemplate, "template.thi.de","Template");
        cTemplate();
        ~cTemplate() = default;

        tResult ProcessInput(ISampleReader* pReader, const iobject_ptr<const ISample>& pSample) override;

    private:
        tFloat some_variable;
};