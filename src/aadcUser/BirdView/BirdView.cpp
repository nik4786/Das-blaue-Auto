#include "BirdView.h"

ADTF_PLUGIN("BirdView",cBirdView);

cBirdView::cBirdView()
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

 
        //register Variables
        RegisterPropertyVariable("Symmetry Active", m_bSymmetry);
        
        //left points of trapezoid
        RegisterPropertyVariable("Point0/x", m_Point0_x);
        RegisterPropertyVariable("Point0/y", m_Point0_y);
        RegisterPropertyVariable("Point3/x", m_Point3_x);
        RegisterPropertyVariable("Point3/y", m_Point3_y);
        
        //right points of trapezoid
        RegisterPropertyVariable("_Point1/x", m_Point1_x);
        RegisterPropertyVariable("_Point1/y", m_Point1_y);      
        RegisterPropertyVariable("_Point2/x", m_Point2_x);
        RegisterPropertyVariable("_Point2/y", m_Point2_y);      


}

tResult cBirdView::AcceptType(ISampleReader* pReader, const iobject_ptr<const IStreamType>& pType)
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


tResult cBirdView::ProcessInput(tNanoSeconds tmTrigger, ISampleReader* /*pReader*/)
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

            
            //check whether P1 and P2 should be calculated automatically
            if(m_bSymmetry)
            {
                m_Point1_x = inputImage.cols/2 + m_Point0_x;
                m_Point1_y = m_Point0_y;

                m_Point2_x = inputImage.cols/2 + m_Point3_x;
                m_Point2_y = m_Point3_y;
            }

                //Load the image
                // Set the lambda matrix the same type and size as input
                Mat lambda = Mat::zeros( inputImage.rows, inputImage.cols, inputImage.type() );
                
                
                //Output Parameter via LOG_Info:
                //LOG_INFO("Point0 x: %i, y: %i ", (int)m_Point0_x, (int)m_Point0_y);
                //LOG_INFO("Point1 x: %i, y: %i ", (int)m_Point1_x, (int)m_Point1_y);
                //LOG_INFO("Point2 x: %i, y: %i ", (int)m_Point2_x, (int)m_Point2_y);
                //LOG_INFO("Point3 x: %i, y: %i ", (int)m_Point3_x, (int)m_Point3_y);

                // Input Quadilateral or Image plane coordinates
                 Point2f inputQuad[4]; 
                // Output Quadilateral or World plane coordinates
                 Point2f outputQuad[4];
                // The 4 points that select quadilateral on the input , from top-left in clockwise order
                // These four pts are the sides of the rect box used as input 
 
                inputQuad[0] = Point2f( m_Point0_x,m_Point0_y);
                inputQuad[1] = Point2f( m_Point1_x,m_Point1_y); //inputImage.cols inputImage.rows
                inputQuad[2] = Point2f( m_Point2_x,m_Point2_y);  
                inputQuad[3] = Point2f( m_Point3_x,m_Point3_y);

                // The 4 points where the mapping is to be done , from top-left in clockwise order
                outputQuad[0] = Point2f( 0,0);
                outputQuad[1] = Point2f( inputImage.cols, 0);
                outputQuad[2] = Point2f( inputImage.cols,inputImage.rows);
                outputQuad[3] = Point2f( 0,inputImage.rows );
            
                // Get the Perspective Transform Matrix i.e. lambda 
                lambda = getPerspectiveTransform(inputQuad,outputQuad);
                warpPerspective(inputImage, outputImage,lambda,inputImage.size());
            
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



