#include "ros/ros.h"
#include "std_msgs/Int32.h"
#include "sensor_msgs/Imu.h"
#include "tf/transform_broadcaster.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.h"

#define RAD2DEG(x) ((x)*180./M_PI)
#define DEG2RAD(x) ((x)/180.*M_PI)

long encoder1data= 0;
long encoder2data= 0;
long encoder1data_old= 0;
long encoder2data_old= 0;
long encoder_delta= 0;

double roll,pitch,yaw,yaw_old;
double roll_d,pitch_d,yaw_d,yaw_d_old;
double delta_yaw_d, delta_yaw;

float baselink_l = 0.0;
float radius = 0.0;
float baselink_delta_x = 0.0 , baselink_delta_y = 0.0;
float baselink_x = 0.0 , baselink_y = 0.0;
float imu_offset_angle = 0;  // degree
float distance_ratio= 348.0; //encoder_1m
double odom_x=0.0; //unit: m
double odom_y=0.0; //unit: m


void imuCallback(const sensor_msgs::Imu::ConstPtr& msg) 
{     
      tf2::Quaternion q(
        msg->orientation.x,
        msg->orientation.y,
        msg->orientation.z,
        msg->orientation.w);
      tf2::Matrix3x3 m(q);     
            
      m.getRPY(roll, pitch, yaw);
      yaw = yaw - DEG2RAD(imu_offset_angle);
      roll_d  = RAD2DEG(roll);
      pitch_d = RAD2DEG(pitch);
      yaw_d   = RAD2DEG(yaw); 
      
      delta_yaw = yaw - yaw_old;
      delta_yaw_d = yaw_d - yaw_d_old;

      ROS_INFO("yaw_d: %lf , yaw_d_old: %lf , delta_yaw_d: %lf \n", yaw_d, yaw_d_old, delta_yaw_d);        
      
      yaw_old = yaw;
      yaw_d_old = yaw_d;
   
}

void encoder1Callback(const std_msgs::Int32& encoder_data1)
{
	encoder1data = encoder_data1.data;
}

void encoder2Callback(const std_msgs::Int32& encoder_data2)
{
	encoder2data = encoder_data2.data;
}

void odometry_calculate(void)
{

	encoder_delta = encoder1data - encoder1data_old; //encoder 변화량
	ROS_INFO("encoder_delta : %d , encoder1data : %d , encoder1data_old : %d \n", encoder_delta, encoder1data, encoder1data_old);
	
	baselink_l = encoder_delta / distance_ratio; //호의 길이(l)
	radius = baselink_l / delta_yaw_d; //회전반경 반지름
	
	ROS_INFO("baselink_l : %f , distance_ratio : %f , delta_yaw_d : %lf , radius : %f \n", baselink_l, distance_ratio, delta_yaw_d, radius);
	
	if(delta_yaw < 0)
	{
		baselink_delta_x = radius * sin(yaw_d);
		baselink_delta_y = -radius * (1 - cos(yaw_d));
	}
	else
	{
		baselink_delta_x = radius * sin(yaw_d);
		baselink_delta_y = radius * (1 - cos(yaw_d));
	}
	
	baselink_x = (baselink_delta_x * cos(yaw_d)) + (baselink_delta_y * -sin(yaw_d));
	baselink_x = (baselink_delta_x * sin(yaw_d)) + (baselink_delta_y * cos(yaw_d));
	
	odom_x += baselink_x;
	odom_y += baselink_x;
	
	ROS_INFO("baselink_delta_x : %f , baselink_delta_y : %f \n", baselink_delta_x, baselink_delta_y);
	ROS_INFO("baselink_x :       %f , baselink_x :       %f \n", baselink_x, baselink_x);
	ROS_INFO("odom_x :           %lf , odom_y :          %lf \n", odom_x, odom_y);
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "odom_experiment");

  ros::NodeHandle n;
  
  ros::Subscriber subEncoder1 = n.subscribe("/encoder1",10,&encoder1Callback); // front  encoder data susscribe
  ros::Subscriber subEncoder2 = n.subscribe("/encoder2",10,&encoder2Callback); // rear  encoder data susscribe
  ros::Subscriber subIMU = n.subscribe("handsfree/imu", 20, &imuCallback);  // imu data susscribe

  ros::Rate loop_rate(10);  // 10
  while (ros::ok())
  {
	odometry_calculate();
	
    encoder1data_old = encoder1data;
  
	loop_rate.sleep();
    ros::spinOnce();
  }
  
  return 0;
}
