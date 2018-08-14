#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"
#include <tf/tf.h>
#include <math.h>
#include <iostream>
#define Kp 0.1


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
	//ROS_INFO(" now_angle: %f ",yaw);

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
	double Error_x_p;
	double Error_y_p;
	double Error_thelta_p;

	//i-control
	//double Ki=1;
	//double Errorxi;
	//double Erroryi;
	double t;
	double t0;
	t0= ros::Time::now().toSec();
	//double SumError_x =0;
	//double SumError_y =0;
	//d-control
	//double Kd=0;
	//double Errorxd;
	//double Erroryd;

	
	while(ros::ok())
	{	

		Error_x = position_x- x;
		Error_y = position_y- y;
		Error_R = sqrt( pow(Error_x,2) + pow(Error_y,2));
		thelta = atan2(Error_y,Error_x);
		if(thelta < 0)
			thelta+=2*M_PI;


		Error_thelta = thelta - yaw;

		

		//p-control
		Error_x_p = Kp*Error_R;
		Error_thelta_p = 5*Kp*Error_thelta;

		//i-control
		t = ros::Time::now().toSec();
		double duration = t - t0;
		//Errorxi = Errorx *duration;
		//Erroryi = Errory *duration;
		//SumError_x = SumError_x + Errorxi;
		//SumError_y = SumError_y + Erroryi;

		//d-control




		
		move.linear.x = Error_x_p;
		move.angular.z = Error_thelta_p;
		velocity_publisher.publish(move);

		odom_publisher.publish(odomer);

		if ((Error_x == 0) && (Error_y == 0))
		{
			move.linear.x = 0.0;
			move.angular.z = 0.0;
			velocity_publisher.publish(move);
	    	break;
	    }


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

