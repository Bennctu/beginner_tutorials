#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include <typeinfo>

bool state = true;
bool state2 = true;
double angle; 
#include "nav_msgs/Odometry.h"


void chatterCallback(const nav_msgs::Odometry::ConstPtr &odomer)
{
	
	float x=odomer->pose.pose.position.x;
	float y=odomer->pose.pose.position.y;
	float z=odomer->pose.pose.position.z;
		
	ROS_INFO("%f",x);
	ROS_INFO("%f",y);
	ROS_INFO("%f",z);


}






int main(int argc, char **argv)
{
	const double PI = 3.14159265358979323846;
    
    ROS_INFO("start");

	ros::init(argc, argv, "mover");

	ros::NodeHandle n;

	//ros::Subscriber sub = n.subscribe("pose",1000,poseCb);

	ros::Publisher velocity_publisher = n.advertise<geometry_msgs::Twist>("cmd_vel",1000);
	//ros::Publisher odom_publisher = n.advertise<geometry_msgs::Point>("odom", 1000);
	ros::Subscriber sub = n.subscribe("odom", 1000, chatterCallback);
	ros::Publisher odom_publisher = n.advertise<nav_msgs::Odometry>("odom", 1000);
	ros::Rate loop_rate(10);

	ROS_INFO("move forward");


	double t0;
	t0= ros::Time::now().toSec();
	
	int count = 0;


	geometry_msgs::Twist move;
	//geometry_msgs::Point odomer;
	nav_msgs::Odometry odomer;
	double translationR=0;
	angle = 0;


	while(state)
	{	
		//printf("%B\n", (angle <=aaa));
        move.angular.z = -0.5;
        double t = ros::Time::now().toSec();
		double duration = t - t0;
		double angle =(duration*move.angular.z)*180/PI;
		ROS_INFO("t is: %f sec",duration);
		ROS_INFO("angle is: %f (degree)",angle);
	    velocity_publisher.publish(move);

	    if (angle >= 90)
	    	state = false;

        ros::spinOnce();
        loop_rate.sleep();
       
	}
	
	move.angular.z=0.0;
	velocity_publisher.publish(move);
	ROS_INFO("angle is done");
	t0= ros::Time::now().toSec();

	//ros::Publisher A = n.advertise<geometry_msgs::Point>("chatter", 1000);
	//ros::Rate loop_rate(10);

	while(state2)
	{	

        move.linear.x = 0.5;

        


        double t = ros::Time::now().toSec();
		double duration = t - t0;
		translationR = duration*move.linear.x;
		ROS_INFO("t is: %f sec",duration);
		ROS_INFO("distance is: %f m",translationR);
	    velocity_publisher.publish(move);

	    odom_publisher.publish(odomer);
	    //ros::Subscriber sub = n.subscribe("odom", 1000, chatterCallback);


	    if (translationR >= 3)
	    	state2 = false;
    
        ros::spinOnce();
        loop_rate.sleep();
        ++count;	
	}


	
	move.linear.x =0.0;
	velocity_publisher.publish(move);
	ROS_INFO("translationR is done");
	
	ros::spin();

	
	return 0;
}
