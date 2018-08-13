#include "ros/ros.h"
#include "std_msgs/String.h"
#include "turtlesim/Pose.h"
#include<sstream>
// declare global variable
double t0;

void chatterCallback(const std_msgs::String::ConstPtr &msg)
{
	int t = ros::Time::now().toSec();
	int duration = t - t0;
	int min=duration/60;
	int sec=duration%60;
	ROS_INFO("t0 is: %i min [%i] sec",min,sec);
	ROS_INFO("I heard: [%s]", msg->data.c_str());
}

int main(int argc, char **argv)
{

	ros::init(argc, argv, "listener");
	ros::NodeHandle n;
	ros::Subscriber sub = n.subscribe("chatter", 1000, chatterCallback);
	t0 = ros::Time::now().toSec();
	ros::spin();
	return 0;
}
