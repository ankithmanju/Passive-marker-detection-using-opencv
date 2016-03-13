#include"geometry.h"
#include <math.h>

orientation_geometry::orientation_geometry()
{
 //do nothing
}
double orientation_geometry::marker_global_orientation(double *pos1, float *pos2)
{
	//pos1 and pos2 are arrays having x,y,z information in it
	
	float x= *pos1-*pos2;
	++pos1;
	++pos2;
	
	float y= *pos1-*pos2;
	return atan2(y,x);
}

double orientation_geometry::camera_orientation(double angle1,double angle2)
{
	//angle2 is marker orientation w.r.t to global coordinates always ( else wrong results might come up)
	//angle1 is marker orinatation w.r.t to camera coordinates.
	return angle2-angle1;
}