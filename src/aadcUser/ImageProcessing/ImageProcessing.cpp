#include "ImageProcessing.h"

ADTF_PLUGIN("ImageProcessing",cImageProcessing);

cImageProcessing::cImageProcessing()
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
        m_pWriterVideo = CreateOutputPin("AdjustedImage", pTypeOutput);

 
        RegisterPropertyVariable("Gaussian/Active", m_ActiveGaussianBlur);
        RegisterPropertyVariable("Gaussian/kSize", m_GaussianBlurkSize);
        RegisterPropertyVariable("ColorSpace/Active", m_ActiveColorSpace);
        RegisterPropertyVariable("ColorSpace/SelectColorSpace", m_colorspace);
        RegisterPropertyVariable("ColorSpace/_Channel/Active", m_ActiveChannelSelect);
        RegisterPropertyVariable("ColorSpace/_Channel/SelectChannel", m_channelselect);
        RegisterPropertyVariable("RangeMask/Active", m_ActiveRangeMask);
        RegisterPropertyVariable("RangeMask/Lower Threshold 1", m_lowerthreshold_1);
        RegisterPropertyVariable("RangeMask/Lower Threshold 2", m_lowerthreshold_2);
        RegisterPropertyVariable("RangeMask/Lower Threshold 3", m_lowerthreshold_3);    
        RegisterPropertyVariable("RangeMask/Upper Threshold 1", m_upperthreshold_1);
        RegisterPropertyVariable("RangeMask/Upper Threshold 2", m_upperthreshold_2);
        RegisterPropertyVariable("RangeMask/Upper Threshold 3", m_upperthreshold_3);
 

}

tResult cImageProcessing::AcceptType(ISampleReader* pReader, const iobject_ptr<const IStreamType>& pType)
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


tResult cImageProcessing::ProcessInput(tNanoSeconds tmTrigger, ISampleReader* /*pReader*/)
{ 

    object_ptr<const ISample> pReadSample;
    Mat outputImage;
    Mat trans; 

     if (IS_OK(m_pReaderVideo->GetNextSample(pReadSample)))
    {
        object_ptr_shared_locked<const ISampleBuffer> pReadBuffer;
        //lock read buffer
        if (IS_OK(pReadSample->Lock(pReadBuffer)))
        {
            //create a opencv matrix from the media sample buffer
            Mat inputImage = Mat(cv::Size(m_InPinVideoFormat.m_ui32Width, m_InPinVideoFormat.m_ui32Height),
                                   CV_8UC3, const_cast<unsigned char*>(static_cast<const unsigned char*>(pReadBuffer->GetPtr())));

            //Do the image processing and copy to destination image buffer
            outputImage = inputImage.clone();


            //Changes the Colorspace if it's activated by m_ActiveColorSpace
            if(m_ActiveColorSpace)
            {
                tInt8 CV_value = get_user_channel_selection_value(m_colorspace);
                if(CV_value!=0)
                {
                    cvtColor(outputImage , outputImage , CV_value);
                    LOG_INFO("cvtColor");
                }
            }

            //compute the gaussianblur if it's activated by m_ActiveGaussianBlur
             if(m_ActiveGaussianBlur)
                {
                    GaussianBlur(outputImage, outputImage, Size((int)m_GaussianBlurkSize, (int)m_GaussianBlurkSize), 0);
                    LOG_INFO("Gaussian");
                }


            //Changes the outputImage to a selected Subchannel 
            if(m_ActiveChannelSelect)
            {
                //LOG_INFO("m_ActiveChannelSelect %i", (int)m_EdgeThreshold2);
                if(m_channelselect>=0||m_channelselect<3)
                {
                    // Split the image into different channels
                    vector<Mat> rgbChannels;
                    split(outputImage, rgbChannels);
                    outputImage = rgbChannels[m_channelselect];
                }
            }


            //do the RangeMask, depending on parameters by user
            if(m_ActiveRangeMask)
            {    
                //LOG_INFO("m_ActiveRangeMask %i", (int)m_EdgeThreshold2);
                Scalar lower(m_lowerthreshold_1,m_lowerthreshold_2,m_lowerthreshold_3);
                Scalar upper(m_upperthreshold_1, m_upperthreshold_2, m_upperthreshold_3);
                inRange(outputImage, lower, upper, outputImage);
            }

         }
    }

    //Write processed Image to Output Pin
    if (!outputImage.empty())
    {
        //update output format if matrix size does not fit to
        if (outputImage.total() * outputImage.elemSize() != m_InPinVideoFormat.m_szMaxByteSize)
        {
            setTypeFromMat(*m_pWriterVideo, outputImage);
        }
        // write to pin
        writeMatToPin(*m_pWriterVideo, outputImage, tmTrigger.nCount);
    }
    
    RETURN_NOERROR;
}


//get Intvalues from user input for opencv function 'cvtColor()'
 tInt8 cImageProcessing::get_user_channel_selection_value(cString colorchannel)
 {
    tInt8 value;
    if(colorchannel == "GRAY") value = 11;  // CV_RGBA2GRAY  
    if(colorchannel == "HSV") value = 41;   //CV_RGB2HSV;
    if(colorchannel == "HLS") value = 53;   //CV_RGB2HLS
    if(colorchannel == "Lab") value = 45;   //CV_RGB2Lab
    if(colorchannel == "YCrCb") value = 37; //CV_RGB2YCrCb
    if(colorchannel == "Luv") value = 51;   //CV_BGR2Luv
    if(colorchannel == "") value = 0;       //no Input

    return value;
 }


//########################################################################
//OLD commments in order to output R Channel of an RGB image
                        // Show individual channels
                        // Mat g, fin_img;
                        // g = Mat::zeros(Size(outputImage.cols, outputImage.rows), inputImage.type());
                        
                        // // Showing Red Channel
                        // // G and B channels are kept as zero matrix for visual perception
                        
                        // vector<Mat> channels;
                        // channels.push_back(g);
                        // channels.push_back(g);
                        // channels.push_back(rgbChannels[2]);
                    
                        // /// Merge the three channels
                        // merge(channels, outputImage);
                        
                     //   outputImage = rgbChannels[1];


//########################################################################