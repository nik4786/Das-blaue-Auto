/**
 * @file ObjectDetect.cpp
 *
 * @brief Implementation of the YOLO Object detector
 * *
 * @author Florian Braun
 * Contact: flb8127@thi.de
 *
 */

#include "ObjectDetect.h"
#include <easy/profiler.h>

ADTF_PLUGIN("YOLODetector",YOLODetector);

YOLODetector::YOLODetector()
{
    // konstruktor

	//create input pin and set inital input format type
	m_InPinVideoFormat.m_strFormatName = ADTF_IMAGE_FORMAT(RGB_24);
	object_ptr<IStreamType> pTypeInput = make_object_ptr<cStreamType>(stream_meta_type_image());
	set_stream_type_image_format(*pTypeInput, m_InPinVideoFormat);    
	m_pReaderVideo = CreateInputPin("in", pTypeInput);

	//Create output pin
	object_ptr<IStreamType> pTypeOutput = make_object_ptr<cStreamType>(stream_meta_type_image());
	set_stream_type_image_format(*pTypeOutput, m_OutPinVideoFormat);
	m_pWriterVideo = CreateOutputPin("out", pTypeOutput);

    m_writerDetections = CreateOutputPin("detections",description<tObjectDetectionResult>());


	initPropertyVariables();	
}

tResult YOLODetector::AcceptType(ISampleReader* pReader, const iobject_ptr<const IStreamType>& pType)
{
    if (pReader == m_pReaderVideo)
    {
        if (pType == stream_meta_type_image())
        {
            object_ptr<const IStreamType> pTypeInput;
            
            // get pType from input reader
            *m_pReaderVideo >> pTypeInput;
            get_stream_type_image_format(m_InPinVideoFormat, *pTypeInput);

            //set also output format 
            get_stream_type_image_format(m_OutPinVideoFormat, *pTypeInput);
            //we always have a grayscale output image
            m_OutPinVideoFormat.m_strFormatName = ADTF_IMAGE_FORMAT(RGB_24);
            // and set pType also to samplewriter
            *m_pWriterVideo << pTypeInput;
            
            RETURN_NOERROR;
        }        
    }

    RETURN_ERROR(ERR_INVALID_TYPE);
}

tResult YOLODetector::Init(tInitStage eStage)
{
    if (eStage == StageNormal)
    {
        // RETURN_IF_FAILED(_runtime->GetObject(m_pClock));
        cFilename filenameClassnames = m_strNames;
        adtf::services::ant::adtf_resolve_macros(filenameClassnames);
        if (!cFileSystem::Exists(filenameClassnames))
        {
            RETURN_ERROR_DESC(ERR_INVALID_FILE, cString::Format("Names file could not be loaded from %s", filenameClassnames.GetPtr()));
        }

        // load relevant classes
        cString relevantClassesString = cString(m_relevantClasses);
        if(relevantClassesString.IsNotEmpty())
        {
            auto start = 0U;
            tSize end;
            do
            {
                end = relevantClassesString.Find(',',start);
                relevantClasses.push_back(string(relevantClassesString.SubString(start, end - start).GetPtr()));
                start = end + 1;
                
            } while(end != std::string::npos);
            
        }

        // check given weights-file path
        cFilename filenameWeights = m_strWeights;
        adtf::services::ant::adtf_resolve_macros(filenameWeights);
        if (!cFileSystem::Exists(filenameWeights))
        {
            RETURN_ERROR_DESC(ERR_INVALID_FILE, cString::Format("Weights file could not be loaded from %s", filenameWeights.GetPtr()));
        }

        // check given cfg-file path
        cFilename filenameCfg = m_strCfg;
        adtf::services::ant::adtf_resolve_macros(filenameCfg);
        if (!cFileSystem::Exists(filenameCfg))
        {
            RETURN_ERROR_DESC(ERR_INVALID_FILE, cString::Format("Cfg file could not be loaded from %s", filenameCfg.GetPtr()));
        }

        if (m_target == TARGET_GPU)
        {
             if (!detector.setup(filenameClassnames.GetPtr(),
                        filenameCfg.GetPtr(),
                        filenameWeights.GetPtr(),
                        m_nmsThreshold,
                        m_confThreshold,
                        0.5,
                        1280,
                        960)) 
            {
                LOG_ERROR("Setup of detector failed!");
            }
            LOG_INFO("Detector: Width %d, Height %d",detector.get_width(), detector.get_height());
            converter.setup(1280, 960, detector.get_width(), detector.get_height());
        }
        else
        {   
            //load classes
            
            ifstream ifs(filenameClassnames.GetPtr());
            string line;
            while (getline(ifs, line)) classes.push_back(line);


                // load the actual net
            m_oNet = readNetFromDarknet(filenameCfg.GetPtr(), filenameWeights.GetPtr());
            m_outputnames = getOutputsNames();
            m_oNet.setPreferableBackend(DNN_BACKEND_OPENCV);
            
            if(m_target==TARGET_OpenCL)
            {
                m_oNet.setPreferableTarget(DNN_TARGET_OPENCL);
            }
            else
            {
                m_oNet.setPreferableTarget(DNN_TARGET_CPU);
            }
            
        }

    }
    else if(eStage == StageGraphReady)
    {

    }
    return cFilter::Init(eStage);
}

tResult YOLODetector::ProcessInput(tNanoSeconds tmTrigger, ISampleReader* /*pReader*/)
{ 
    // @stefan: begin copy
    object_ptr<const ISample> pReadSample;
    if (IS_OK(m_pReaderVideo->GetNextSample(pReadSample)))
    {
        object_ptr_shared_locked<const ISampleBuffer> pReadBuffer;
        Mat outputImage;
        //lock read buffer
        if (IS_OK(pReadSample->Lock(pReadBuffer)))
        {
            //create a opencv matrix from the media sample buffer
            Mat inputImage(cv::Size(m_InPinVideoFormat.m_ui32Width, m_InPinVideoFormat.m_ui32Height),
                CV_8UC3, (uchar*)pReadBuffer->GetPtr());
    // @stefan: end copy

            EASY_BLOCK("Inferencing", profiler::colors::Blue500); // Blue block
            vector<detection_result> detections;
            if(m_target != TARGET_GPU)
            {
                processOpenCV(inputImage,detections);
            }
            else
            {
                processDarknet(inputImage,detections);
            }
            EASY_END_BLOCK;

            if(m_renderResults)
            {
                // render output video
                output_detections_image(inputImage,detections,tmTrigger);
            }

            output_detections(detections,tmTrigger);
		    pReadBuffer->Unlock();
        }
    }

    RETURN_NOERROR;
}

vector<detection_result> YOLODetector::processDetections(const Mat& frame, const vector<Mat>& outs)
{   
    vector<detection_result> output;

    vector<Point> centers;
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;
    
    for (size_t i = 0; i < outs.size(); ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if(relevantClasses.empty() || std::find(relevantClasses.begin(), relevantClasses.end(), classes[classIdPoint.x]) != relevantClasses.end()) 
            {
                if (confidence > m_confThreshold)
                {
                    int centerX = (int)(data[0] * frame.cols);
                    int centerY = (int)(data[1] * frame.rows);
                    int width = (int)(data[2] * frame.cols);
                    int height = (int)(data[3] * frame.rows);
                    int left = centerX - width / 2;
                    int top = centerY - height / 2;
                    
                    centers.push_back(Point(centerX,centerY));
                    classIds.push_back(classIdPoint.x);
                    confidences.push_back((float)confidence);
                    boxes.push_back(Rect(left, top, width, height));
                }
            }
        }
    }
    
    // Perform non maximum suppression to eliminate redundant overlapping boxes with
    // lower confidences
    vector<int> indices;
    NMSBoxes(boxes, confidences, m_confThreshold, m_nmsThreshold, indices);
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        output.push_back(detection_result(
            boxes[idx],
            centers[idx].x,
            centers[idx].y,
            classes[classIds[idx]],
            classIds[idx],
            confidences[idx]));
    }

    return output;
}


// Draw the predicted bounding box
void YOLODetector::drawPreds(vector<detection_result>& detections, Mat& frame)
{
    for(auto detection : detections)
    {
        auto left = detection.bbox.x;
        auto top = detection.bbox.y;
        auto right = detection.bbox.x + detection.bbox.width;
        auto bottom = detection.bbox.y + detection.bbox.height;

        //Draw a rectangle displaying the bounding box
        rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0, 128, 255), 3);
        
        //Get the label for the class name and its confidence
        string label = format("%.2f", detection.confidence);
        label = detection.className + ":" + label;

        
        //Display the label at the top of the bounding box
        int baseLine;
        Size labelSize = getTextSize(label, FONT_HERSHEY_DUPLEX, 0.8, 1, &baseLine);
        top = max(top, labelSize.height);
        rectangle(frame, Point(left, top - round(1.5*labelSize.height)), Point(left + round(1.5*labelSize.width), top + baseLine), Scalar(70,70,70), FILLED);
        putText(frame, label, Point(left, top), FONT_HERSHEY_DUPLEX, 1, Scalar(230,230,230),1);
    }
}

tResult YOLODetector::output_detections(vector<detection_result>& detections, tNanoSeconds tm)
{
    output_sample_data<tObjectDetectionResult> output(tm);
    output->numResults = detections.size();

    if((sizeof(output->objectsDetected)/sizeof(tDetectedObject))<detections.size())
    {
        LOG_WARNING("More objects detected than would fit in tObjectDetectionResult struct. Consider increasing array-size");
    }

    tDetectedObject* out_detections = reinterpret_cast<tDetectedObject*>(output->objectsDetected);
    memset(out_detections, 0, sizeof(output->objectsDetected)); // works because length of array objectsDetected is static

    for(uint8_t i=0;i<detections.size();++i)
    {
        memcpy(out_detections[i].className,detections[i].className.c_str(),strlen(detections[i].className.c_str())+1);
        out_detections[i].prob = detections[i].confidence;
        out_detections[i].top = detections[i].bbox.y;
        out_detections[i].left = detections[i].bbox.x;
        out_detections[i].width = detections[i].bbox.width;
        out_detections[i].height = detections[i].bbox.height;
    }

    return m_writerDetections->Write(output.Release());
}

tResult YOLODetector::processOpenCV(Mat& image, vector<detection_result>& detections)
{
    try
    {
        Mat oBlob = blobFromImage(image, 1/255.0, Size(416, 416),Scalar(0, 0, 0), true, false);

        if (!m_oNet.empty() && oBlob.size > 0)
        {
            // do neural net stuff
            m_oNet.setInput(oBlob);
    
            // Runs the forward pass to get output of the output layers
            vector<Mat> outs;
            m_oNet.forward(outs, m_outputnames);

            // store results in a more structured way
            detections = processDetections(image,outs); 
        }
    }
    catch (cv::Exception ex)
    {
        const char* err_msg = ex.what();
        LOG_ERROR(cString("OpenCV exception caught: ") + err_msg);
    }

    RETURN_NOERROR;
}
tResult YOLODetector::processDarknet(Mat& image, vector<detection_result>& detections)
{
    // convert and resize opencv image to darknet image
    if (!converter.convert(image, dnimage)) 
    {
        LOG_WARNING("Failed to convert opencv image to darknet image");
    }

    // run detector
    if (!detector.detect(dnimage)) 
    {
        LOG_WARNING("Failed to run detector");
    }
    vector<Darknet::Detection> dnDetections;
    detector.get_detections(dnDetections);
    LOG_INFO("Number of detections: %d",dnDetections.size());

    for(auto d : dnDetections)
    {
        detections.push_back(detection_result(d));
    }

    RETURN_NOERROR;
}

void YOLODetector::output_detections_image(Mat& image, vector<detection_result>& detections, tNanoSeconds tm)
{
    EASY_BLOCK("Output image", profiler::colors::Blue500);
    //Darknet::image_overlay(detections, image);
    drawPreds(detections,image);
    //update output format if matrix size does not fit to
    if (image.total() * image.elemSize() != m_OutPinVideoFormat.m_szMaxByteSize)
    {
        setTypeFromMat(*m_pWriterVideo, image);
    }
    // write to pin
    writeMatToPin(*m_pWriterVideo, image, tm.nCount);
}

// Get the names of the output layers, used by OpenCV detection method
vector<String> YOLODetector::getOutputsNames()
{
    static vector<String> names;
    if (names.empty())
    {
        //Get the indices of the output layers, i.e. the layers with unconnected outputs
        vector<int> outLayers = m_oNet.getUnconnectedOutLayers();
        
        //get the names of all the layers in the network
        vector<String> layersNames = m_oNet.getLayerNames();
        
        // Get the names of the output layers in names
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
        names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}
void YOLODetector::initPropertyVariables()
{
    m_strWeights.SetDescription("Path to the .weights-file of the neural net");
    RegisterPropertyVariable("Network/Weights",m_strWeights);

    m_strCfg.SetDescription("Path to the .cfg-file of the neural net");
	RegisterPropertyVariable("Network/Cfg",m_strCfg);

    m_strNames.SetDescription("Path to the .names-file the network was trained with");
	RegisterPropertyVariable("Network/Names",m_strNames);

    m_confThreshold.SetValidRange(0.0,1.0);
	RegisterPropertyVariable("Detection/Confidence Threshold",m_confThreshold);

    m_nmsThreshold.SetValidRange(0.0,1.0);
	RegisterPropertyVariable("Detection/NMS Threshold",m_nmsThreshold);

	RegisterPropertyVariable("Detection/Input Width",m_inpWidth);
	RegisterPropertyVariable("Detection/Input Height",m_inpHeight);

    m_relevantClasses.SetDescription("Filters out unwanted detections. Only classes mentioned"
    "in this property (seperated by comma)  be detected. If the propertry is empty"
    "all classes will be detected");
    RegisterPropertyVariable("Detection/Relevant classes",m_relevantClasses);
	
    m_target.SetValueList({{TARGET_CPU,"CPU"},
                                    {TARGET_OpenCL,"OpenCL (not recommended)"},
                                    {TARGET_GPU, "GPU (requiers Darknet)"}});
    m_target.SetDescription("Choose which component does the computation for object detection");
    RegisterPropertyVariable("Misc/Computation target",m_target);
	// RegisterPropertyVariable("Misc/Use Darknet",m_bDarknet);
    RegisterPropertyVariable("Misc/Render results",m_renderResults);
}

