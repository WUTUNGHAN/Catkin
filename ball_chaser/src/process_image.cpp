#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include "geometry_msgs/Twist.h"
#include <sensor_msgs/Image.h>
#include <iostream>

// Define a global client that can request services
ros::ServiceClient client;
ros::Publisher motor_command_publisher;


// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float linear_x, float angular_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("The robot is chasing the ball......");

    //ros::NodeHandle n;
    // Request linear_x & angular_z
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = linear_x;
    srv.request.angular_z = angular_z;
    if (!client.call(srv))
        ROS_ERROR("Failed to call service");
}

// This callback function continuously executes and reads the image data

void process_image_callback(const sensor_msgs::Image img)
{
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    int white_pixel = 255;
    bool left = true;
    bool mid = true;
    bool right = true;
    bool stop = false;
    uint left_number = 0;
    uint right_number = 0;
    uint mid_number = 0;
    uint not_white_number = 0;

    for (int i = 0; i < img.height; i = i + 100)
    {
        for (int j = 0; j < img.step; j = j + 100)
        {
            if ((img.data[i * img.step + j] == white_pixel) && (img.data[i * img.step + j + 1] == white_pixel) && (img.data[i * img.step + j + 2] == white_pixel)) // && (img.data[(i * img.step) + j + 1] == 255) && (img.data[(i * img.step) + j + 2] == 255))
            {
                if (j <= (img.step / 3)) //if (j <= 1431655765)
                { 
                    left_number = left_number + 1;
                }

                else if (j <= (2 * img.step / 3)) //(j <= 2863311530)
                {
                    mid_number = mid_number + 1;
                }

                else if (j <= (img.step)) //(j <= 4294967295)
                { 
                    right_number = right_number + 1;
                }

                
            }
            else
            {
                not_white_number = not_white_number + 1;
            }
       }    
    }

    ROS_INFO("Left_number:, %d \n", left_number);
    ROS_INFO("Mid_number:, %d \n", mid_number);
    ROS_INFO("Right_number:, %d \n", right_number);
    ROS_INFO("not_white_number:, %d \n", not_white_number);

    if (left_number > mid_number && left_number > right_number){
        //left = true 
        drive_robot(0, 0.3);
        ROS_INFO_STREAM("White ball fall left");
        ROS_INFO_STREAM("Car turn left");
        left_number=0;
        right_number = 0;
        mid_number = 0;
        not_white_number = 0;
        }

    else if (right_number > mid_number && right_number > left_number){
        //right = true 
        drive_robot(0, -0.3);
        ROS_INFO_STREAM("White ball fall right");
        ROS_INFO_STREAM("Car turn right");
        left_number = 0;
        right_number = 0;
        mid_number = 0;
        not_white_number = 0;
    }

    else if (mid_number > right_number && mid_number > left_number){
        //mid = true 
        drive_robot(0.2, 0);
        ROS_INFO_STREAM("White ball fall mid");
        ROS_INFO_STREAM("Car move forward");
        left_number = 0;
        right_number = 0;
        mid_number = 0;
        not_white_number = 0;
    }

    else if (not_white_number > mid_number && not_white_number > right_number && not_white_number > left_number)
    {
        //stop = true;
        ROS_INFO_STREAM("Cannot find the white ball, the car just stopped");
        drive_robot(0, 0);
        left_number = 0;
        right_number = 0;
        mid_number = 0;
        not_white_number = 0;
    }
}


    

    
    int main(int argc, char **argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 100, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}