#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <windows.h>

using namespace cv;
using namespace std;

clock_t start,finish;
void SSR(Mat frame, Mat frame_ssr, Size size_ssr)
{
   GaussianBlur(frame, frame_ssr, size_ssr, 0, 0);
    int i, x, y,z;
    Mat frame1,frame2;
    float log_Initial[3],
          log_Light[3],
          max_R[3] = {0},
          min_R[3] = {255};
    float **temp;
    temp = (float**)calloc(frame.rows, sizeof(float*));
    for(z = 0; z< frame.rows; z++)
        temp[z] = (float*)calloc(frame.cols*3, sizeof(float));


    double duration;
    start =clock();
    frame.convertTo(frame,CV_32F);
    cv::log(frame,frame1);
    frame_ssr.convertTo(frame_ssr,CV_32F);
    cv::log(frame_ssr,frame2);
    for(y = 0; y < frame.rows; y++)
    {
        for(x = 0; x < frame.cols; x++)
        {
            for(i = 0; i < 3; i++)
            {
                //log_Initial[i] = log10(frame.at<Vec3b>(y,x)[i]);
                //log_Light[i] = log10(frame_ssr.at<Vec3b>(y,x)[i]);
                temp[y][x*3+i] =(frame1.at<Vec3b>(y,x)[i]<0||frame2.at<Vec3b>(y,x)[i]<0)?0:frame1.at<Vec3b>(y,x)[i]-frame2.at<Vec3b>(y,x)[i];
                max_R[i] = max(max_R[i],temp[y][x*3+i]);
                min_R[i] = min(min_R[i],temp[y][x*3+i]);
            }
        }
    }
    for(y = 0; y < frame.rows; y++)
    {
        for(x = 0; x < frame.cols; x++)
        {
            for(i = 0; i < 3; i++)
            {
               frame_ssr.at<Vec3b>(y,x)[i] = (int)(255*(temp[y][x*3+i]-min_R[i])/(max_R[i]-min_R[i]));
            }
        }
    }

    for(z = 0; z< frame.rows; z++)
        free(temp[z]);
    free(temp);
}

void MSR(Mat frame, Mat *frame_ssr,Mat frame_msr, int size_num)
{
    int i;
    char s[2];
    for(i = 0; i < size_num-1; i++)
        addWeighted(frame_ssr[i],(i+1)/(float)(i+2),frame_ssr[i+1],1/(float)(i+2),0,frame_ssr[i+1],-1);
    frame_ssr[size_num-1].copyTo(frame_msr);

}

void SSRCR(Mat frame, Mat frame_ssr, Size size_ssr)
{
    GaussianBlur(frame, frame_ssr, size_ssr, 0, 0);
    //imshow("GBlur",frame_ssr);
    int i, x, y;
    float log_Retinex[3],
          log_Initial[3],
          log_Light[3],
          max_R[3] = {0},
          min_R[3] = {255};
    int Retinex[3];


    for(y = 0; y < frame.rows; y++)
    {
        for(x = 0; x < frame.cols; x++)
        {
            for(i = 0; i < 3; i++)
            {
                log_Initial[i] = log10(frame.at<Vec3b>(y,x)[i]);
                log_Light[i] = log10(frame_ssr.at<Vec3b>(y,x)[i]);
                log_Retinex[i] = (log_Initial[i]<0||log_Light[i]<0)?0:log_Initial[i]-log_Light[i];
                log_Retinex[i] = log_Retinex[i]*(log10(125*frame.at<Vec3b>(y,x)[i])-log10(frame.at<Vec3b>(y,x)[0]+frame.at<Vec3b>(y,x)[1]+frame.at<Vec3b>(y,x)[2]));

                max_R[i] = max(max_R[i],log_Retinex[i]);
                min_R[i] = min(min_R[i],log_Retinex[i]);
            }
        }
    }
    for(y = 0; y < frame.rows; y++)
    {
        for(x = 0; x < frame.cols; x++)
        {
            for(i = 0; i < 3; i++)
            {
                log_Initial[i] = log10(frame.at<Vec3b>(y,x)[i]);
                log_Light[i] = log10(frame_ssr.at<Vec3b>(y,x)[i]);
                log_Retinex[i] = (log_Initial[i]<0||log_Light[i]<0)?0:log_Initial[i]-log_Light[i];
                log_Retinex[i] =
                log_Retinex[i]*(log10(125*frame.at<Vec3b>(y,x)[i])-log10(frame.at<Vec3b>(y,x)[0]+frame.at<Vec3b>(y,x)[1]+frame.at<Vec3b>(y,x)[2]));
                Retinex[i] = (int)(255*(log_Retinex[i]-min_R[i])/(max_R[i]-min_R[i]));
                frame_ssr.at<Vec3b>(y,x)[i] = Retinex[i];
            }
        }
    }
}
void SimplestCB(Mat& in, Mat& out, float percent) {
    assert(in.channels() == 3);
    assert(percent > 0 && percent < 100);

    float half_percent = percent / 200.0f;

    vector<Mat> tmpsplit; split(in,tmpsplit);
    for(int i=0;i<3;i++) {
        //find the low and high precentile values (based on the input percentile)
        Mat flat; tmpsplit[i].reshape(1,1).copyTo(flat);
        cv::sort(flat,flat,CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);
        int lowval = flat.at<uchar>(cvFloor(((float)flat.cols) * half_percent));
        int highval = flat.at<uchar>(cvCeil(((float)flat.cols) * (1.0 - half_percent)));
        //cout << lowval << " " << highval << endl;

        //saturate below the low percentile and above the high percentile
        tmpsplit[i].setTo(lowval,tmpsplit[i] < lowval);
        tmpsplit[i].setTo(highval,tmpsplit[i] > highval);

        //scale the channel
        normalize(tmpsplit[i],tmpsplit[i],0,255,NORM_MINMAX);
    }
    merge(tmpsplit,out);
}

void MSRCR(Mat frame, Mat frame_msrcr, Size size_min, Size size_max, int size_num)
{
    int i;
    char s[2];
    Mat temp[size_num];

    for(i = 0; i < size_num; i++)
    {
        frame.copyTo(temp[i]);
        SSRCR(frame, temp[i], Size(size_min.height+i*(size_max.height-size_min.height)/(float)(size_num-1),size_min.height+i*(size_max.height-size_min.height)/(float)(size_num-1)));
        sprintf(s,"%d",i);
        //imshow(s,temp[i]);
    }
    for(i = 0; i < size_num-1; i++)
        addWeighted(temp[i],(i+1)/(float)(i+2),temp[i+1],1/(float)(i+2),0,temp[i+1],-1);

    SimplestCB(temp[size_num-1],frame_msrcr,1);
}
