#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"
#include <tf/tf.h>
#include <math.h>
#include <iostream>
#define Kp 0.1
#define Ki 0
#define Kd 0.1


double position_x;
double position_y;
double x;
double y;         
double thelta;    //odomer
double Error_x;
double Error_y;
double Error_thelta;
double roll;
double pitch;
double yaw;
double yaw_1;


bool is_nan(double dVal)
{
	if (dVal==dVal)
		return false;


	return true;
}



void odometryCallback(const nav_msgs::Odometry::ConstPtr &odomer)
{
	
	x = odomer->pose.pose.position.x;
	y = odomer->pose.pose.position.y;
		
	//ROS_INFO("x=%f",x);
	//ROS_INFO("y=%f",y);
	tf::Quaternion Q(
	odomer->pose.pose.orientation.x,
	odomer->pose.pose.orientation.y,
	odomer->pose.pose.orientation.z,
	odomer->pose.pose.orientation.w);

	tf::Matrix3x3 M(Q);
	M.getRPY(roll,pitch,yaw);
	if(yaw < 0)
		yaw+=2*M_PI;
	else
		yaw=yaw;

	
	if (is_nan(yaw))
		yaw=yaw_1;
	else
		yaw = yaw;

	//ROS_INFO(" now_angle: %f ",yaw);
	yaw_1 = yaw;

}


int main(int argc, char **argv)
{	
    
    ROS_INFO("start");
	ros::init(argc, argv,"PID");
	ros::NodeHandle n;
	ros::Publisher velocity_publisher = n.advertise<geometry_msgs::Twist>("cmd_vel",10);
	ros::Publisher odom_publisher = n.advertise<nav_msgs::Odometry>("odom", 10);
	ros::Subscriber sub = n.subscribe("odom", 1000, odometryCallback);
	ros::Rate loop_rate(10);



	ROS_INFO("move forward"); 
	std::cout <<"Enter desire position_x:";
	std::cin >> position_x;
	std::cout <<"Enter desire position_y:";
	std::cin >> position_y;

	//desired angle
	//ROS_INFO("thelta:%f",thelta);

	geometry_msgs::Twist move;
	nav_msgs::Odometry odomer;



	//p-control
	double Error_R;
	double linear_p;
	double angular_p;
	
	//i-control
	double SumError_linear_i=0;
	double SumError_angular_i=0;
	double Error_linear_i;
	double Error_angular_i;
	double linear_i;
	double angular_i;

	//d-control
	double Error_R_last = 0;
	double Error_thelta_last = 0;
	double Error_linear_d;
	double Error_angular_d;
	double linear_d;
	double angular_d;
	
	double linear_vel;
	double angular_vel;

	while(ros::ok())
	{	

		Error_x = position_x- x;
		Error_y = position_y- y;
		Error_R = sqrt( pow(Error_x,2) + pow(Error_y,2));
		thelta = atan2(Error_y,Error_x);
		if (thelta < 0)
			thelta+=2*M_PI;

		Error_thelta = thelta - yaw;


		if ( Error_thelta <0.001)
			Error_thelta = 0;


		ROS_INFO("Error_thelta is %f",Error_thelta);
		

		//p-control
		linear_p = Kp*Error_R;
		angular_p = 5*Kp*Error_thelta;

		//i-control
		//t = ros::Time::now().toSec();
		//double duration = t - t0;
		Error_linear_i = Error_R/10;
		Error_angular_i = Error_thelta/10;
		SumError_linear_i = SumError_linear_i + Error_linear_i;
		SumError_angular_i = SumError_angular_i + Error_angular_i;
		
		if (SumError_linear_i >0.1)
			SumError_linear_i=0.1;

		if (SumError_angular_i>0.1)
			SumError_angular_i=0.1;


		linear_i = Ki*(SumError_linear_i);
		angular_i = Ki*(SumError_angular_i);

		//d-control
		Error_linear_d =(Error_R -Error_R_last)*10;
		Error_angular_d =(Error_thelta -Error_thelta_last)*10;
		linear_d = Kd*(Error_linear_d);
		angular_d = Kd*(Error_angular_d);

		

		Error_R_last = Error_R;
		Error_thelta_last = Error_thelta;  //stored


		//summation
		linear_vel = linear_p + linear_i + linear_d;
		angular_vel = angular_p + angular_i + angular_d;

		if (Error_R <0.001)
			break;


		move.linear.x = linear_vel;
		move.angular.z = angular_vel;
		velocity_publisher.publish(move);
		odom_publisher.publish(odomer);



		//if (Error_R <0.01)
		//{
		//	move.linear.x = 0.0;
		//	move.angular.z = 0.0;
		//	velocity_publisher.publish(move);
	    //	break;
	    //}


        ros::spinOnce();
        loop_rate.sleep();

	}


	move.linear.x = 0.0;
	move.angular.z = 0.0;
	velocity_publisher.publish(move);
	ROS_INFO("done");


	//ros::spin();

	
	//return 0;

}

