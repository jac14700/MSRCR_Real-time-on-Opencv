#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace cv;
using namespace std;

void SSR(Mat frame, Mat frame_ssr, Size size_ssr);
void MSR(Mat frame, Mat *frame_ssr,Mat frame_msr, int size_num);
void SSRCR(Mat frame, Mat frame_ssr, Size size_ssr);
void MSRCR(Mat frame, Mat frame_msrcr, Size size_min, Size size_max, int size_num);

int main( int argc, char** argv )
{
    Mat A,B;
    Mat frame, frame_ssr[3], frame_msr, frame_msrcr;
    VideoCapture cap(1); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
	{
	    return -1;
	    }

    while (1)
	{
	    cap>> frame;
	    resize(frame,frame,Size(frame.size().width*0.5,frame.size().height*0.5));
//        frame.copyTo(frame_ssr[0]);
//        frame.copyTo(frame_msr);
        frame.copyTo(frame_msrcr);
//        SSRCR(frame, frame_ssr[0], Size(601,601));
//        SSR(frame, frame_ssr[1], Size(101,101));
//        SSR(frame, frame_ssr[2], Size(11,11));

        MSRCR(frame, frame_msrcr, Size(201,201), Size(601,601), 3);
        imshow("frame_origin", frame);
        imshow("frame_msrcr",  frame_msrcr);
//        imshow("frame_ssr",frame_ssr);
//        imshow("frame_msr",frame_ssr);
//        imshow("frame_msrcr",frame_msrcr);
//        imwrite("msrcr.jpg",frame_msrcr);
        waitKey(10);
	}

    return 0;
}
