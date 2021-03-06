#pragma once

#include "stdafx.h"
#include <adtffiltersdk/adtf_filtersdk.h>
#include <algorithm>
#include <owndesc.h>


using namespace adtf::base;
using namespace adtf::streaming;
using namespace adtf::filter;
using namespace adtf::mediadescription;
using namespace adtf::util;
using namespace adtf::ucom;
using namespace adtf::system;

using namespace cv;
using namespace cv::dnn;
using namespace std;

//#include <aadc.h>
#include <ADTF3_OpenCV_helper.h>

struct detection_result
{
	Rect bbox;
	string className;
	float confidence;

	detection_result(Rect _bbox,string _classname,float _confidence)
	{
		bbox = _bbox;
		className=_classname;
		confidence=_confidence;
	}
};

class YOLODetector : public cFilter
{
    public:
        ADTF_CLASS_ID_NAME(YOLODetector, "yolodetector.thi.de","YoloDetector");
        YOLODetector();
        ~YOLODetector() = default;

	tResult AcceptType(ISampleReader* pReader, const iobject_ptr<const IStreamType>& pType) override;
        tResult ProcessInput(tNanoSeconds /*tmTrigger*/, ISampleReader* /*pReader*/) override;
        //tResult ProcessInput(ISampleReader* pReader, const iobject_ptr<const ISample>& pSample) override;
        tResult Init(tInitStage eStage) override;
	//tResult Inite(tInitStage eStage);

    private:

	/*! The clock */
    adtf::ucom::object_ptr<adtf::services::IReferenceClock> m_pClock;
	//Pins
	/*! Reader for the input video. */
	ISampleReader* m_pReaderVideo;
	/*! Writer for the output video. */
	cSampleWriter* m_pWriterVideo;
	ISampleWriter* m_writerDetections;
	//Stream Formats
	/*! The input format */
	tStreamImageFormat m_InPinVideoFormat;
	/*! The output format */
	tStreamImageFormat m_OutPinVideoFormat;


	
	// Network settings
	property_variable<cFilename> m_strWeights = cFilename("yolov3.weights");
	property_variable<cFilename> m_strCfg = cFilename("yolov3.cfg");
	property_variable<cFilename> m_strNames = cFilename("coco.names");
	// Detection settings
	property_variable<tFloat> m_confThreshold = 0.5;
	property_variable<tFloat> m_nmsThreshold = 0.4;
	property_variable<tInt> m_inpWidth = 416;
	property_variable<tInt> m_inpHeight = 416;
	property_variable<cString> m_relevantClasses = cString("");
	// Misc settings
	property_variable<tBool> m_bOpenCL = tTrue;	
	property_variable<tBool> m_renderResults = false;
	


	Net m_oNet; // the neural net
	vector<string> classes;
	vector<String> m_outputnames;
	vector<string> relevantClasses;
	tNanoSeconds m_last_inference = tNanoSeconds(0);

	adtf::streaming::tStreamImageFormat m_sCurrentFormat;

	vector<detection_result> processDetections(const Mat& frame, const vector<Mat>& out);
	tResult output_detections(vector<detection_result>& detections, tNanoSeconds tm);
	// Draw the predicted bounding box
	void drawPreds(vector<detection_result>& detections, Mat& frame);
	// Get the names of the output layers
	vector<String> getOutputsNames();


};
