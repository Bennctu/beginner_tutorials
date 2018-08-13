#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"
#include <tf/tf.h>
#include <math.h>
#include <iostream>
double position_x;
double position_y;
double x;
double y;
double thelta;
double Errorx;
double Errory;
double Errror_thelta;
double roll;
double pitch;
double yaw;
bool state = true;




const double PI = 3.14159265358979323846;


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
		yaw+=2*PI;
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


	

	thelta = atan2(position_y,position_x);
	if(thelta < 0)
		thelta+=2*PI;
	//desired angle
	//ROS_INFO("thelta:%f",thelta);

	

	geometry_msgs::Twist move;
	nav_msgs::Odometry odomer;
	

	//p-control
	double r_1;
	double R_2;
	double R;
	double Kp=0.1;
	double Errorxp;
	double Erroryp;
	double Errror_theltap;

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


	while(state)
	{	

		Errorx = position_x- x;
		Errory = position_y- y;
		

		//p-control
		Errorxp = Kp*Errorx;
		Erroryp = Kp*Errory;
		

		//i-control
		t = ros::Time::now().toSec();
		double duration = t - t0;
		//Errorxi = Errorx *duration;
		//Erroryi = Errory *duration;
		//SumError_x = SumError_x + Errorxi;
		//SumError_y = SumError_y + Erroryi;

		//d-control




		r_1=x*x+y*y;
		R_2=position_x*position_x+position_y*position_y;
		R = sqrt (Errorxp*Errorxp + Erroryp*Erroryp);

		if (r_1 < R_2)
			R = R;
		else if (r_1 > R_2)
			R = -R;
		else 
			R = 0;
		//ROS_INFO("TV:%f",translation_v);

		double rotation_v;
		//double yaw_r;
		//if ( thelta > 0)
		//{	
		//	if(yaw < 0)
		//		yaw_r = 2*PI+yaw;
		//	else
		//		yaw_r = yaw;
		//}		
		//else if (thelta < 0)
		//{
		//	if(yaw > 0)
		//		yaw_r = -2*PI+yaw;
		//	else
		//		yaw_r = yaw;
		//}
			
		Errror_thelta = thelta - yaw;	
		Errror_theltap = Kp*Errror_thelta;
		rotation_v = (Errror_theltap)*180/PI;

		move.linear.x = R;
		move.angular.z = rotation_v;
		velocity_publisher.publish(move);

		//ROS_INFO("motion:%f",move.linear.x);
		odom_publisher.publish(odomer);

		if ((x == position_x) && (y == position_y))
	    	state = false;

        ros::spinOnce();
        loop_rate.sleep();

	}


	move.linear.x =0.0;
	move.angular.z =0.0;
	velocity_publisher.publish(move);
	ROS_INFO("done");
	
	ros::spin();

	
	return 0;

}

