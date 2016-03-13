class orientation_geometry
{
public:
	orientation_geometry();
	double marker_global_orientation(double *pos1, float *pos2); //function to calculate the orientation of the marker with respect to global axes, pos1 and pos2 gives coodinates of the camera and marker w.r.t global frame .
	double opencv_marker_angle();
	double camera_orientation(double angle1,double angle2);  //takes the angle of marker with respect to the camera frame and global frame and returns the orientation of the camera with respect to global frame which  is needed. 
};