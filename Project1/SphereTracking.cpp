#include "SphereTracking.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/types_c.h"
#include<iostream>
#include <fstream>
#include <math.h> 
#include <cmath>


using namespace cv;
using namespace std;

 Mat SphereDetection::inRange_cal(Mat imgHSV,int HueL,int HueH,int Satl,int Sath,int Vall,int Valh)
{
	Mat tempThreshold;
	inRange(imgHSV,Scalar(HueL,Satl,Vall),Scalar(HueH,Sath,Valh),tempThreshold);
	return tempThreshold;
}

 Mat SphereDetection::morphologicalOperations(Mat Inputimage)
{
	erode(Inputimage,Inputimage,getStructuringElement(MORPH_ELLIPSE,Size(5,5)));
	dilate( Inputimage,Inputimage,getStructuringElement(MORPH_ELLIPSE,Size(5,5))); 
	dilate(Inputimage,Inputimage,getStructuringElement(MORPH_ELLIPSE,Size(5,5)));		//morphological closing (removes small holes from the foreground)
	erode(Inputimage, Inputimage,getStructuringElement(MORPH_ELLIPSE,Size(5,5)));
	return Inputimage;
}

 Mat SphereDetection::moments_calulator(Mat inputimage,int color_flag)
{
	Mat temp_imgLines=Mat::zeros(inputimage.size(),CV_8UC3);;
	Moments oMoments=moments(inputimage);
	double dM01=oMoments.m01;
	double dM10=oMoments.m10;
	double dArea=oMoments.m00;
	double posX=dM10/dArea;															//calculate the position of the ball					
	double posY=dM01/dArea;  
	circle(temp_imgLines,Point(posX,posY),100,Scalar(255,0,255),1,8,0);
	return temp_imgLines;
}


 
 double* SphereDetection::coordinates_marker(Mat inputimage,int color_flag)
 {
	Moments oMoments=moments(inputimage);
	double dM01=oMoments.m01;
	double dM10=oMoments.m10;
	double dArea=oMoments.m00;
	double posX,posY;
	static double pos[2]={};
	if((dArea>=0.00000001))
	{
		 posX=dM10/dArea;															//calculate the position of the ball	
		 posY=dM01/dArea;
		 pos[0]=posX;
		 pos[1]=posY;
		 return pos;
		
		
	}
	else
		pos[0]=-1;
		pos[1]=-1;
		return pos;	
		
 }
 float* SphereDetection::SerialDataParse(string stdstring)
{
	static float coordinates[3]={};
	std::size_t found = stdstring.find(",");
	stdstring.erase(0,found+1);
	found = stdstring.find(",");									//extract the x coordinates
	std::string xcoordinate = stdstring.substr (0,found);
	float x=stof(xcoordinate);
	stdstring.erase(0,found+1);
	cout<<xcoordinate<<endl;										//Display x coordinate
	found = stdstring.find(",");									//extract the y coordinates
	std::string ycoordinate = stdstring.substr (0,found);
	float y=stof(ycoordinate);
	stdstring.erase(0,found+1);
	cout<<ycoordinate<<endl;										//Display y coordinate
	found = stdstring.find(",");									//extract the z coordinates
	std::string zcoordinate = stdstring.substr (0,found);
	float z=stof(zcoordinate);
	stdstring.erase(0,found+1);
	cout<<zcoordinate<<endl;										//Display z coordinate
	coordinates[0]=x;
	coordinates[1]=y;
	coordinates[2]=z;
	
	return coordinates;
}

