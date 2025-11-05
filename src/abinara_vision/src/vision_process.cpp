#include "abinara_vision/vision_processor.hpp"

VisionProcessor::VisionProcessor(ros::NodeHandle& nh)
    : nh_(nh), it_(nh_) {
    sub_ = it_.subscribe("/camera_frame", 1, &VisionProcessor::imageCallback, this);
}

void VisionProcessor::imageCallback(const sensor_msgs::ImageConstPtr& msg) {
    cv::Mat frame = cv_bridge::toCvShare(msg, "bgr8")->image.clone();
    cv::Mat gray, blurImg, thresh;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, blurImg, cv::Size(5,5), 0);
    cv::threshold(blurImg, thresh, 100, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area < 1000) continue;

        std::vector<cv::Point> approx;
        cv::approxPolyDP(contour, approx, 0.03 * cv::arcLength(contour, true), true);
        cv::Rect box = cv::boundingRect(approx);

        std::string shape = "Unknown";
        if (approx.size() == 3)
            shape = "Triangle";
        else if (approx.size() == 4)
            shape = "Square";
        else if (approx.size() > 6)
            shape = "Circle";

        cv::rectangle(frame, box, cv::Scalar(0,255,0), 2);
        cv::putText(frame, shape, cv::Point(box.x, box.y - 10),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,255,0), 2);
    }
    
    cv::Mat hsv, mask_orange, mask_white;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    // Filter warna oranye dan putih
    cv::inRange(hsv, cv::Scalar(5, 100, 100), cv::Scalar(25, 255, 255), mask_orange);
    cv::inRange(hsv, cv::Scalar(0, 0, 200), cv::Scalar(180, 40, 255), mask_white);
    cv::Mat mask = mask_orange | mask_white;

    // Bersihkan noise kecil
    cv::erode(mask, mask, cv::Mat(), cv::Point(-1,-1), 2);
    cv::dilate(mask, mask, cv::Mat(), cv::Point(-1,-1), 2);

    std::vector<std::vector<cv::Point>> colorContours;
    cv::findContours(mask, colorContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (auto& c : colorContours) {
        if (cv::contourArea(c) < 800) continue;
        cv::drawContours(frame, std::vector<std::vector<cv::Point>>{c}, -1, cv::Scalar(255,0,0), 2);
    }

    // Tampilkan hasil
    cv::imshow("Vision Processor", frame);
    cv::waitKey(1);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "vision_processor");
    ros::NodeHandle nh;
    VisionProcessor vp(nh);
    ros::spin();
    return 0;
}
