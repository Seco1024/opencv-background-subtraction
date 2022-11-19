#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#define THRESHOLD 50

using namespace std;

void showOutput(cv::Mat &origin_frame, bool hsv){
    cv::Mat img = cv::imread("./img/output.jpg", cv::IMREAD_COLOR);
    cv::resize(img, img, cv::Size(), 0.5, 0.5);
    cv::Mat insert(origin_frame, cv::Rect(0, 80, img.size().width, img.size().height));
    if(hsv){
        cv::cvtColor(img, img, cv::COLOR_BGR2HSV);
    }
    img.copyTo(insert);
}


void takeScreenshot(cv::VideoCapture &cap, cv::Mat &origin_frame, cv::Mat &subtracted_frame, cv::Mat &screenshot){
    cv::Mat insert(origin_frame, cv::Rect(10, 10, screenshot.size().height, screenshot.size().width));
    screenshot.copyTo(insert);
    string subtitle = "camera";
    cv::putText(origin_frame, subtitle, cv::Point(22, 82), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.6, cv::Scalar(0, 0, 0), 0.4, cv::LINE_AA);

    cv::Mat hot_zone = subtracted_frame(cv::Range(10, 10+screenshot.size().width), cv::Range(10, 10+screenshot.size().height));
    if (cv::countNonZero(hot_zone) > 3){
        cv::Mat image;
        cap.read(image);
        cv::flip(image, image, 1);
        cv::imwrite("./img/output.jpg", image);
    }
}

void getOutput(cv::Mat &origin_frame, cv::Mat &subtracted_frame, cv::Mat &photo){
    cv::Mat insert(origin_frame, cv::Rect(550, 10, photo.size().height, photo.size().width));
    photo.copyTo(insert);
    string subtitle = "picture";
    cv::putText(origin_frame, subtitle, cv::Point(562, 82), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.6, cv::Scalar(0, 0, 0), 0.4, cv::LINE_AA);

    cv::Mat hot_zone = subtracted_frame(cv::Range(10, 10+photo.size().width), cv::Range(550, 550+photo.size().height));
    if (cv::countNonZero(hot_zone) > 3) showOutput(origin_frame, false);
}

void getHSVOutput(cv::Mat &origin_frame, cv::Mat &subtracted_frame, cv::Mat &hsv){
    cv::Mat insert(origin_frame, cv::Rect(460, 10, hsv.size().height, hsv.size().width));
    hsv.copyTo(insert);
    string subtitle = "HSV";
    cv::putText(origin_frame, subtitle, cv::Point(485, 82), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.6, cv::Scalar(0, 0, 0), 0.4, cv::LINE_AA);

    cv::Mat hot_zone = subtracted_frame(cv::Range(10, 10+hsv.size().height), cv::Range(460, 460+hsv.size().width));
    if(cv::countNonZero(hot_zone) > 3) showOutput(origin_frame, true);
}

int main(int argc, char *argv[]){

    cv::VideoCapture cap;
    cv::Mat origin_frame, ref_frame, cur_frame, subtracted_frame;
    cv::Mat screenshot = cv::imread("./img/screenshot.png", cv::IMREAD_COLOR);
    cv::Mat photo = cv::imread("./img/image.png", cv::IMREAD_COLOR);
    cv::Mat hsv = cv::imread("./img/hsv.png", cv::IMREAD_COLOR);    
    cv::resize(screenshot, screenshot, cv::Size(), 0.3, 0.3);
    cv::resize(photo, photo, cv::Size(), 0.3, 0.3);
    cv::resize(hsv, hsv, cv::Size(), 0.3, 0.3);
    bool first = true;

    cap.open(0);
    cv::namedWindow("Background Subtraction", cv::WINDOW_AUTOSIZE);

    while(true){

        if(!cap.read(origin_frame)){
            cout << "Error occurs while capturing" << endl;
            break;
        }

        cv::flip(origin_frame, origin_frame, 1);

        if(first){
            first = false;
            cur_frame = cv::Mat(origin_frame.size().width, origin_frame.size().height, origin_frame.depth(), 1);
            ref_frame = cv::Mat(origin_frame.size().width, origin_frame.size().height, origin_frame.depth(), 1);
            subtracted_frame = cv::Mat(origin_frame.size().width, origin_frame.size().height, origin_frame.depth(), 1);
            cv::cvtColor(origin_frame, cur_frame, cv::COLOR_BGR2GRAY);
        }

        cv::cvtColor(origin_frame, ref_frame, cv::COLOR_BGR2GRAY);
        cv::absdiff(cur_frame, ref_frame, subtracted_frame);
        cv::threshold(subtracted_frame, subtracted_frame, THRESHOLD, 255, cv::THRESH_BINARY);

        takeScreenshot(cap, origin_frame, subtracted_frame, screenshot);
        getOutput(origin_frame, subtracted_frame, photo);
        getHSVOutput(origin_frame, subtracted_frame, hsv);

        cv::imshow("Background Subtraction", origin_frame);
        if(cv::waitKey(1) == 'q') break;

    }
    
    cap.release();
    cv::destroyWindow("Background Subtraction");
    return 0;
}

