#include "ros/ros.h"
#include "std_msgs/Int32.h"
#include "sensor_msgs/Imu.h"
#include "tf/transform_broadcaster.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.h"

#define RAD2DEG(x) ((x)*180./M_PI)
#define DEG2RAD(x) ((x)/180.*M_PI)

struct encoderData{
	long frontdata= 0;
	long reardata= 0;
	long frontdata_old= 0;
	long reardata_old= 0;
	long delta= 0;
} encoder;

struct baselinkData{
	float l = 0.0;
	float delta_x = 0.0; 
	float delta_y = 0.0;
}baselink;

double roll,pitch,yaw,yaw_old;
double roll_d,pitch_d,yaw_d,yaw_d_old;
double delta_yaw_d, delta_yaw;

float radius = 0.0;
float distance_ratio= 348.0; //encoder_1m

void imuCallback(const sensor_msgs::Imu::ConstPtr& msg) 
{
	 char buf[8];       
      tf2::Quaternion q(
        msg->orientation.x,
        msg->orientation.y,
        msg->orientation.z,
        msg->orientation.w);
      tf2::Matrix3x3 m(q);     
            
      m.getRPY(roll, pitch, yaw);
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
	encoder.frontdata = encoder_data1.data;
}

void encoder2Callback(const std_msgs::Int32& encoder_data2)
{
	encoder.reardata = encoder_data2.data;
}

void odometry_calculate(void)
{

	encoder.delta = encoder.frontdata - encoder.frontdata_old; //encoder 변화량
	ROS_INFO("encoder_delta : %d , encoder.frontdata : %d , encoder.frontdata_old : %d \n", encoder.delta, encoder.frontdata, encoder.frontdata_old);
	
	baselink.l = encoder.delta / distance_ratio; //호의 길이(l)
	radius = baselink.l / delta_yaw_d; //회전반경 반지름
	
	ROS_INFO("baselink.l : %f , distance_ratio : %f , delta_yaw_d : %lf , radius : %f \n", baselink.l, distance_ratio, delta_yaw_d, radius);
	
	if(delta_yaw < 0)
	{
		baselink.delta_x = radius * sin(delta_yaw_d);
		baselink.delta_y = -radius * (1 - cos(delta_yaw_d));
	}
	else
	{
		baselink.delta_x = radius * sin(delta_yaw_d);
		baselink.delta_y = radius * (1 - cos(delta_yaw_d));
	}

	ROS_INFO("baselink.delta_x : %f  , baselink.delta_y : %f \n", baselink.delta_x, baselink.delta_y);
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "odom_experiment");

  ros::NodeHandle n;
  
  ros::Subscriber subEncoder1 = n.subscribe("/encoder1",10,&encoder1Callback); // front  encoder data susscribe
  ros::Subscriber subEncoder2 = n.subscribe("/encoder2",10,&encoder2Callback); // rear  encoder data susscribe
  ros::Subscriber subIMU = n.subscribe("handsfree/imu", 10, &imuCallback);  // imu data susscribe

  ros::Rate loop_rate(10);  // 10
  while (ros::ok())
  {
	odometry_calculate();
	
    encoder.frontdata_old = encoder.frontdata;
  
	loop_rate.sleep();
    ros::spinOnce();
  }
  
  return 0;
}
