#ifndef VISION_PROCESSOR_HPP
#define VISION_PROCESSOR_HPP

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/Image.h>
#include <opencv2/opencv.hpp>

class VisionProcessor {
public:
    VisionProcessor(ros::NodeHandle& nh);
    void imageCallback(const sensor_msgs::ImageConstPtr& msg);

private:
    ros::NodeHandle nh_;
    image_transport::ImageTransport it_;
    image_transport::Subscriber sub_;
};

#endif
