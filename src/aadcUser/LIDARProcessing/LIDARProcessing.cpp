#include "LIDARProcessing.h"

#include <math.h>



ADTF_PLUGIN("LIDARProcessing",cLIDARProcessing);

class detectedObject{

    public:
    detectedObject(int xstart_,int ystart_,int xmin_,int ymin_,int xend_,int yend_):
    xstart(xstart_),ystart(ystart_),xmin(xmin_), ymin(ymin_), xend(xend_),yend(yend_)
    {}
    private:
    int xstart;
    int ystart;
    int xmin;
    int ymin;
    int xend;
    int yend;

};

cLIDARProcessing::cLIDARProcessing()
{
    // Konstruktor
    CreateInputPin("Lidar_Input", description<tLaserScannerData>());
    //m_writerDetections =
    CreateOutputPin("Lidar_Output", description<tLaserScannerData>());
    //Create output pin for visualisation
    m_OutPinVideoFormat.m_strFormatName = ADTF_IMAGE_FORMAT(RGB_24);
     //Create output pin
    object_ptr<IStreamType> pTypeOutput = make_object_ptr<cStreamType>(stream_meta_type_image());
    set_stream_type_image_format(*pTypeOutput, m_OutPinVideoFormat);
    m_pWriterVideo = CreateOutputPin("AdjustedImage", pTypeOutput);

    //RegisterPropertyVariable("Lambda ", Lambda);   
}

tResult cLIDARProcessing::AcceptType(ISampleReader* pReader, const iobject_ptr<const IStreamType>& pType)
{
    RETURN_NOERROR;
}

tResult cLIDARProcessing::ProcessInput(ISampleReader* pReader, const iobject_ptr<const ISample>& pSample)
{
    auto Lidar_input = sample_data<tLaserScannerData>(pSample); //save the input data
    auto *Lidar_data = Lidar_input->tScanArray; //save the data array of Lidar input
    //--------------------------------------------------------------------------------------------------

    int Lambda= 30; // Adapitive Breakpoint Detection
    int counter=0;
    int max_measuring_distance= 12000;
    int min_measuring_distance= 150;
    int ego_length=500;
    int Lidar_Radius[Lidar_input->ui32Size]; // new Array for all radius of measured points
    double Lidar_Angle[Lidar_input->ui32Size]; // new Array for all angles of measured points

    for(int i=0;i<(int) Lidar_input->ui32Size;i++){ //cutting of the Array of measurement, when it ends
         if(Lidar_data[i].f32Radius >= min_measuring_distance && Lidar_data[i].f32Radius <=max_measuring_distance &&  Lidar_data[i].f32Angle>=0 && Lidar_data[i].f32Angle<=360) // Filtern von Messwerten mit Radius kleiner 15 cm (min Reichweite) && Radius größer 12 m (max Reichweite)
         {
             Lidar_Radius[counter]=(int) Lidar_data[i].f32Radius;
             Lidar_Angle[counter]=(double) Lidar_data[i].f32Angle;
             counter++;
         }
    }
    int Grad0Measurement=0; //position in the sorted and filtered Array at 0 Grad
    int Grad90Measurement=0; //position in the sorted and filtered Array at 90 Grad
    int Grad270Measurement=0; //position in the sorted and filtered Array at 270 Grad
    int Grad360Measurement=0; //position in the sorted and filtered Array at 360 Grad

    //adaptive breakpoint detection
    int breakpoints[counter]={0};//for saving the position of breakpoints and the kind of the breakpoint 
    
    for(int i=0;i<counter-1;i++){

    //calculating sigma
    double sigma=0;
    if(Lidar_Radius[i] < 1500 && Lidar_Radius[i+1] < 1500)
    {sigma=0.5;}
    else
    {
        double sigma_old=0.01*Lidar_Radius[i]; //distance resulution (1% of the distance)
        double sigma_new=0.01*Lidar_Radius[i+1]; //distance resulution (1% of the distance)
        if(sigma_old>sigma_new){
            sigma=sigma_old;
        }
        else
        {
            sigma=sigma_new;
        }
    }
    //calculating the distance between two measured points
    float Distance_square=abs(Lidar_Radius[i]*Lidar_Radius[i]+Lidar_Radius[i+1]*Lidar_Radius[i+1]-2*Lidar_Radius[i]*Lidar_Radius[i+1]*cos((Lidar_Angle[i]-Lidar_Angle[i+1])*M_PI/180));
    float Distance= sqrt(Distance_square);
    //calculating the max distance of breakpoint detection
    float D_max=Lidar_Radius[i]*(abs(sin(((Lidar_Angle[i]-Lidar_Angle[i+1])*M_PI/180)))/abs(sin((Lambda-(Lidar_Angle[i]-Lidar_Angle[i+1]))*M_PI/180)))+3*sigma;

    if(Distance > D_max || D_max > ego_length) //Breakpoint is found
    {
        breakpoints[i]=1;
        breakpoints[i+1]=1;
    }     
    }
    
    Grad0Measurement=0;
    Grad360Measurement=counter-1;

for(int i=1; i<counter; i++){

    if(Lidar_Angle[i]>= 270.0)
    {Grad270Measurement=i;
    Grad90Measurement=Grad270Measurement-1;
    i=counter;
    }
}
    //setting the edges of measurement as breakpoints

    breakpoints[Grad0Measurement]=1; 
    breakpoints[Grad90Measurement]=1;
    breakpoints[Grad270Measurement]=1;
    breakpoints[Grad360Measurement-1]=1;

        for(int i=1; i<counter-1; i++){
        if(breakpoints[i-1]==1 && breakpoints[i]==1 && breakpoints[i+1]==1){
            int flag=0;

            while(flag==0){
                breakpoints[i]=2; //breakpoint, but no real object
                i++;
                if(breakpoints[i+1]!=1){
                flag=1;
                }
            }
        }
        }

//--------------------------------------------------------------------------------------------------
   //image output with Open CV

    Mat outputImage;
    Mat inputImage = Mat(cv::Size(640,480),CV_8UC3,Scalar(255,255,255));
    outputImage=inputImage;

    double xCoord[counter]={0.0}; //x coordiante
    double yCoord[counter]={0.0}; //y coordinate
    double xCoord_img[counter]={0.0}; //x coordiante
    double yCoord_img[counter]={0.0}; //y coordinate

    for(int i=0;i<=counter;i++){
       //calculating in kartesian coordnates //max: 24m x 12m
        xCoord[i]=sin(Lidar_Angle[i]*M_PI/180)*Lidar_Radius[i];
        yCoord[i]=cos(Lidar_Angle[i]*M_PI/180)*Lidar_Radius[i];
       //Mapping at image size

    if(Lidar_Angle[i] < 180){

        xCoord_img[i]=320+(abs(xCoord[i])*640/max_measuring_distance);
        yCoord_img[i]=480-(abs(yCoord[i])*480/max_measuring_distance);

    }

    else{

        xCoord_img[i]=320-(abs(xCoord[i])*640/max_measuring_distance);
        yCoord_img[i]=480-(abs(yCoord[i])*480/max_measuring_distance);   

    }  
/*
    if(breakpoints[i]==1){

           //circle(inputImage, Point(xCoord_img[i],yCoord_img[i]), 5, Scalar(0,0,255), 1, 8, 0); // breakpoint-> blue
       }
       if(breakpoints[i]==0){
           //circle(inputImage, Point(xCoord_img[i],yCoord_img[i]), 5, Scalar(0,255,0), 1, 8, 0); //no breakpoint-> green
       }
       */
    }
int min_Radius=12001; //more than the possible Radius measurement
int istart;
int imin;
int iend;

detectedObject* Lidar_Output_[120]={}; 

int count_Obj=-1;
int flag=0;

//print lines & Clustering// 1-0-0-1 with random amount of zeros=object

for(int i=0;i<counter-2;i++){

    if((breakpoints[i] == 1 && breakpoints[i+1] == 0) && i!= Grad90Measurement){

        line(inputImage, Point(xCoord_img[i],yCoord_img[i]), Point(xCoord_img[i+1],yCoord_img[i+1]), Scalar(0,0,255), 2, 8, 0);
        istart=i;
        flag=1;
   }

   if((breakpoints[i] == 0 && breakpoints[i+1] == 0) && i!= Grad90Measurement){
        line(inputImage, Point(xCoord_img[i],yCoord_img[i]), Point(xCoord_img[i+1],yCoord_img[i+1]), Scalar(0,0,255), 2, 8, 0);
            if(min_Radius>Lidar_Radius[i]){
             min_Radius=Lidar_Radius[i];
             imin=i;
             if(flag==1){
                flag=2;
            }

         }
   }

   if( (breakpoints[i] == 0 && breakpoints[i+1] == 1) && i!= Grad90Measurement&& flag==2){

        line(inputImage, Point(xCoord_img[i],yCoord_img[i]), Point(xCoord_img[i+1],yCoord_img[i+1]), Scalar(0,0,255), 2, 8, 0);
        iend=i+1;
        count_Obj++;
        detectedObject* Objekt= new detectedObject((int)xCoord[istart],(int)yCoord[istart],(int)xCoord[imin],(int)yCoord[imin],(int)xCoord[iend],(int)yCoord[iend]);
        Lidar_Output_[count_Obj]=Objekt; //output
        min_Radius=12001;
        flag=0;   
   }
}
    circle(inputImage, Point(320,480), 5, Scalar(255,0,0), 3, 8, 0); //draw Ego position
    //--------------------------------------------------------------------------------------------------  
    //Output: returning the object data of Lidar Processing in sensor Coordinates to Output Pin
    //Lidar_Output=*Lidar_Output_;  //write Object data to Output Pin
    //--------------------------------------------------------------------------------------------------

     //Write processed Image to Output Pin
    if (!outputImage.empty())
    {
        // write to pin
        setTypeFromMat(*m_pWriterVideo, outputImage);

        writeMatToPin(*m_pWriterVideo, outputImage,get_sample_time(pSample).nCount);
    }
    RETURN_NOERROR;
}