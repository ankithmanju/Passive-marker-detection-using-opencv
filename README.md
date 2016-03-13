# Passive-marker-detection-using-opencv
Passive marker detection using opencv and coordinate estimate using opencv 

Tool for object(Passive marker detection) using opencv 

2d orienation.cpp 
  Main source file for 2d orientation , object detection using webcam video stream
  
SphereTracking.cpp
    inRange_cal -- used for threholding AN HSV image for object detection
    morphologicalOperations -- used for erosion and dilation of the binary image
    moments_calulator --- used for first order statistic calculation 
    coordinates_marker  --- used to get coordinates of the marker in the image plane
    SerialDataParse --- parse incoming data from the serial port
    
geometry.cpp
    marker_global_orientation -- get global coordinates of the marker using image coordinates 
                                  from object detection tool and incming tachymeter data from serial port
    camera_orientation  --- returns orientation of the camera in the global coordinates                       
