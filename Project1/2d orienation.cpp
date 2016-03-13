#include<math.h>
#include<stdio.h>
#include<iostream>
#include <fstream>
#include"geometry.h"
#include"Serial.h"
#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include"opencv2/core/types_c.h"
#include"SphereTracking.h"
#include <thread>
using namespace cv;
using namespace std;
using namespace SphereDetection;
#define RX_BUFFSIZE 200					//Receiver Buffer size
#define TX_BUFFSIZE 10000

//Program to calculate the orientation of the camera w.r.t global coordinated in the specific 2d case
//The 2d Coordinates of the marker are given by (x1,y1) and (x2,y2) which are obtained from the tachymeter and are static 
//The 2d Coordinates of the camera are (xc,yc) is a variable quantity as the camera would be mounted on the robot, which 
//is moving 
//Angle_marker is the orientation of the marker with respect to global axes
//Angle_camera is the orientation of the marker with respect to camera axes
//Angle_camera_orientation is the orientation of the camera with respect to the global axes

 void main( int argc, char** argv )
{
	ofstream myfile;	//txt file to store the values of tachymeter and spehere detection.
	char output_buffer_on[TX_BUFFSIZE];
	char output_buffer_off[TX_BUFFSIZE];
	for(int i=0; i<TX_BUFFSIZE ; i++)
	{
		output_buffer_on[i]=255;
	}
	for(int i=0; i<TX_BUFFSIZE ; i++)
	{
		output_buffer_off[i]=0;
	}
	if(argc != 2)
	{
		cout <<"press any key and enter to quit"<<endl;
		char temp;
		cin >> temp;
		return;
	}
	orientation_geometry obj1;												//orientation geometry object 
	cout<<"Opening com port"<<endl;											//comport object 
	tstring abc = "\\\\.\\";
	tstring abc1 = "\\\\.\\COM16";
	tstring commPortName(argv[1]);
	abc.append(commPortName);	
	Serial serial(abc);
	Serial serial1(abc1);
	cout<<"Port opened"<<endl;
	const int FRAME_WIDTH = 1280;			//default is 640*480								//setting the opencv object parameters          
	const int FRAME_HEIGHT = 720;
    VideoCapture cap(0); 
	cap.set(CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	cap.set(CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
	
	cap.set(CAP_PROP_AUTO_EXPOSURE, 0 );									//switch off auto exposure.
		
	//double exposure = cap.get(CAP_PROP_AUTO_EXPOSURE);					//check if auto exposure is off or not.
	if (!cap.isOpened())  
    {
         cout<<"Cannot open the web cam"<<endl;
         return;
    }
	 float *camera_coordinates=0;
	while(true)
	{
		
		 Mat imgOriginal,imgOriginal1,imgOriginal2,imgOriginal3;		 
		 try
		 {
			serial.flush();
			char buffer[RX_BUFFSIZE];
			cout<<"Reading from the serial port:";
			for(int i = 0; i < 10; i++)
			{
				int charsRead = serial.read(buffer, RX_BUFFSIZE);
				//int charsRead=5;
				if(buffer[charsRead-2]=='\r'&&buffer[charsRead-1]=='\n')	//(CharsRead ==55) is the total number of charecters in the standard bitstream.
					if(buffer[0]=='A'&& buffer[1]=='u'&& buffer[2]=='t')
					{
						string stdstring = buffer;
						camera_coordinates=SerialDataParse(stdstring);//Camera Coordinates from tachymeter
						serial.flush();
						break;
					}
				Sleep(100);
			}
				
		 }
		 catch(const char *msg)
		 {
			 cout<<msg<<endl;
		 }
		  bool bSuccess = cap.read(imgOriginal);								// read a new frame from video
		 // send 100 packets asynchronously
		 int written_to_output_buffer=serial1.write(output_buffer_off,TX_BUFFSIZE);
		 Sleep(1500); 
		 bool bSuccess1 = cap.read(imgOriginal2);								// read another new frame from video when led is on .
		 imgOriginal= imgOriginal2-imgOriginal;		//delete this part and keep imgOriginal to revert back to sphere detection.
		 imshow("Original", imgOriginal);								//show the original image
		  //written_to_output_buffer=serial1.write(output_buffer_off,TX_BUFFSIZE); //switch off the leds
		 //Sleep(4000);
		 //imgOriginal= imgOriginal2-imgOriginal;
		 //bool bSuccess2 = cap.read(imgOriginal3);
		 //imgOriginal1= imgOriginal3-imgOriginal2;
		 //imgOriginal=imgOriginal+imgOriginal1;

		 //alternate method
		 //bool bSuccess = cap.read(imgOriginal);								// read a new frame from video
		 //imgOriginal=imgOriginal/5;
		 //int written_to_output_buffer=serial.write(output_buffer,20);
		 //bool bSuccess1 = cap.read(imgOriginal2);								// read another new frame from video
		 //imgOriginal= imgOriginal2-imgOriginal;

		if (!bSuccess) //if not success, break loop
		{
             cout << "Cannot read a frame from video stream" << endl;
             break;
		}
		Mat imgHSV;
		cvtColor(imgOriginal,imgHSV,COLOR_BGR2HSV);						//Convert the captured frame from BGR to HSV
		Mat imgthreshold1=inRange_cal(imgHSV,22,32,131,255,0,255);	    //threshold yellow object 
		Mat imgthreshold2=inRange_cal(imgHSV,40,66,152,255,0,255);	    //threshold green object 
		Mat imgthreshold3=inRange_cal(imgHSV,140,179,175,255,0,255);	//threshold pink object
		imgthreshold1=morphologicalOperations(imgthreshold1);			//dilate and erode to reduce noise in the form of small pixels
		imgthreshold2=morphologicalOperations(imgthreshold2);
		imgthreshold3=morphologicalOperations(imgthreshold3);
		//GaussianBlur(imgThresholded, imgThresholded,Size( 27, 27 ),0,0);	// low pass filter if needed
		Mat crosshair_yellow_object=moments_calulator(imgthreshold1,1);
		double* yellow_marker_coordinates=coordinates_marker(imgthreshold1,1);
		double yellow_marker[2]={};
		yellow_marker[0]=*yellow_marker_coordinates;
		yellow_marker_coordinates++;
		yellow_marker[1]=*yellow_marker_coordinates;

		Mat crosshair_green_object=moments_calulator(imgthreshold2,2);
		double* green_marker_coordinates=coordinates_marker(imgthreshold2,2);
		double green_marker[2]={};
		green_marker[0]=*green_marker_coordinates;
		green_marker_coordinates++;
		green_marker[1]=*green_marker_coordinates;
		Mat crosshair_pink_object=moments_calulator(imgthreshold3,3);
		
		double* pink_marker_coordinates=coordinates_marker(imgthreshold3,3);
		double pink_marker[2]={};
		pink_marker[0]=*pink_marker_coordinates;
		pink_marker_coordinates++;
		pink_marker[1]=*pink_marker_coordinates;
		imgOriginal=imgOriginal+crosshair_yellow_object+crosshair_green_object+crosshair_pink_object; 
		Mat imgThresholded=imgthreshold1+imgthreshold2+imgthreshold3;	//thresholded image contaning thresholds of green yellow and pink
		//imshow("Thresholded Image", imgThresholded);					//show the thresholded image
		
		if (waitKey(30) == 27)											//wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break; 
		}					
		 //double marker[3]={};										//Coordinates of the markers fed in from a text file
		 
		//marker[0]=yellow_marker[0];
		//yellow_marker_coordinates++;
		//marker[1]=green_marker[0];
		//marker[2]=pink_marker[0];
		float camera[3];											//camera coordinated as read from serial data port having data from tachymeter tracking the camera.
		double angle_camera=0;
		
		if(!(yellow_marker[0]<=0 && green_marker[0]<=0 && pink_marker[0]<=0))
		{
			camera[0]=*camera_coordinates;
			camera_coordinates++;
			camera[1]=*camera_coordinates;
			camera_coordinates++;
			camera[2]=*camera_coordinates;
			//camera_coordinates++;	
			myfile.open ("Data_Logged.txt", ios::app);
			//the text file format is U,V,X,Y,Z.

			//calculation done from focal length calculation.
			double camera_focal_length= 7.2957532932520610e+002;	//focal length obtained from camera calibration.
													
			if(!(yellow_marker[0]<0))
			{
				for(int j=0;j<3;j++)
			{
				myfile<<to_string(camera[j]);
				double sine_alpha=yellow_marker[1]/camera_focal_length;	// if yellow marker is detected so camera orientation for yellow marker. and yellow has the highest priority.
				double angle_camera=asin(sine_alpha);

				if (j < 2)
				{
					myfile << ",";
				}
				//else
				//{
					//myfile << ".";
				//}
			}
				double yellow_angle_marker=obj1.marker_global_orientation(yellow_marker,camera);
				double angle_camera_orientation=obj1.camera_orientation(yellow_angle_marker,angle_camera);
				myfile <<";"<<to_string(yellow_marker[0])<<","<<to_string(yellow_marker[1]);
				myfile<< ":"<< to_string(angle_camera_orientation)<<";" ;
				//yellow_marker_coordinates++;
				//myfile<<to_string(*yellow_marker_coordinates)<<";";				
			}
			else if(!(green_marker[0]<0))
			{
				for(int j=0;j<3;j++)
			{
				myfile<<to_string(camera[j]);
				double sine_alpha=green_marker[1]/camera_focal_length;
				double angle_camera=asin(sine_alpha);				// if green marker is detected so camera orientation for green marker.

				if (j < 2)
				{
					myfile << ",";
				}
				//else
				//{
					//myfile << ".";
				//}
			}
				double green_angle_marker=obj1.marker_global_orientation(green_marker,camera);
				double angle_camera_orientation=obj1.camera_orientation(green_angle_marker,angle_camera);
				myfile <<";"<<to_string(green_marker[0])<<","<<to_string(green_marker[1]);
				myfile<< ":"<< to_string(angle_camera_orientation)<<";" ;
				//green_marker_coordinates++;
				//myfile<<to_string(*green_marker_coordinates)<<";";			
			}
			else if(!(pink_marker[0]<0))
			{
				for(int j=0;j<3;j++)
			{
				myfile<<to_string(camera[j]);
				double sine_alpha=pink_marker[1]/camera_focal_length;
				double angle_camera=asin(sine_alpha);				// if pink marker is detected so camera orientation for pink marker.

				if (j < 2)
				{
					myfile << ",";
				}
				//else
				//{
					//myfile << ".";
				//}
			}
				double pink_angle_marker=obj1.marker_global_orientation(pink_marker,camera);
				double angle_camera_orientation=obj1.camera_orientation(pink_angle_marker,angle_camera);
				myfile <<";"<<to_string(pink_marker[0])<<","<<to_string(pink_marker[1]);
				myfile<< ":"<< to_string(angle_camera_orientation)<<";" ;
				//pink_marker_coordinates++;
				//myfile<<to_string(*pink_marker_coordinates)<<";";	
			}
			myfile<<endl;
			myfile.close();

		}
		//if(!(marker[0]<0 && marker[1]<0))
		//{
			//camera[0]=*camera_coordinates;
			//camera_coordinates++;
			//camera[1]=*camera_coordinates;
			//camera_coordinates++;
			//camera[2]=*camera_coordinates;
			//camera_coordinates++;	
			//myfile.open ("Data_Logged.txt", ios::app);
			//for(int j=0;j<3;j++)
			//{
				//myfile<<to_string(camera[j]);

				//if (j < 2)
				//{
					//myfile << ", ";
				//}
				//else
				//{
					//myfile << ";";
				//}
			//}
			//myfile <<"Sphere detecton module output "<<to_string(marker[0])<<","<<to_string(marker[1]);		
			//myfile << endl;
			//double angle_camera=30.32;										// from opencv codes.Calculation needs to be done.
			//if(!*yellow_marker_coordinates<0)
			//{
				//double yellow_angle_marker=obj1.marker_global_orientation(yellow_marker_coordinates,camera);
				//double angle_camera_orientation=obj1.camera_orientation(yellow_angle_marker,angle_camera);
			//}
			//else if(!*green_marker_coordinates<0)
			//{
				//double green_angle_marker=obj1.marker_global_orientation(green_marker_coordinates,camera);
				//double angle_camera_orientation=obj1.camera_orientation(green_angle_marker,angle_camera);
			//}
			//else if(!*pink_marker_coordinates<0)
			//{
				//double pink_angle_marker=obj1.marker_global_orientation(pink_marker_coordinates,camera);
				//double angle_camera_orientation=obj1.camera_orientation(pink_angle_marker,angle_camera);
			//}			
			//myfile.close();
		//}		
	}	
}
