#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/types_c.h"

using namespace cv;
using namespace std;

namespace SphereDetection
{
 Mat inRange_cal(Mat imgHSV, int HueL,int HueH,int Satl,int Sath,int Vall,int Valh);	//Inrange calculator
 Mat morphologicalOperations(Mat Inputimage);											//Morphological opening (removes small objects from the foreground)
 Mat moments_calulator(Mat inputimage,int color_flag);									//Calculate moments of the detected object and calculate its centre
 double* coordinates_marker(Mat inputimage,int color_flag);								//Calculate the coordinates of the ball in the image plane
 float* SerialDataParse(string stdstring);
};