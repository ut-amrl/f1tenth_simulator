#include <math.h>
#include <random>
#include <string>

#include "config_reader/config_reader.h"
#include "geometry_msgs/Quaternion.h"
#include "geometry_msgs/Twist.h"
#include "nav_msgs/Odometry.h"
#include "ros/publisher.h"
#include "ros/ros.h"
#include "simulator/robot_model.h"

#ifndef SRC_SIMULATOR_DIFFDRIVE_MODEL_H_
#define SRC_SIMULATOR_DIFFDRIVE_MODEL_H_

namespace diffdrive {

class DiffDriveModel : public robot_model::RobotModel {
 private:
    geometry_msgs::Twist last_cmd_;
    double t_last_cmd_;
    std::default_random_engine rng_;
    std::normal_distribution<float> angular_error_;
    ros::Subscriber drive_subscriber_;
    config_reader::ConfigReader config_reader_;
    ros::Publisher odom_publisher_;
    geometry_msgs::TransformStamped odom_trans;
    nav_msgs::Odometry odom_msg;

    float yaw_rate;
    float odometry_x;
    float odometry_y;
    float odometry_w;
    float vel_x;
    float vel_y;
    float target_linear_vel;
    float target_angular_vel;
    double linear_vel;
    double angular_vel;
    tf::TransformBroadcaster odom_broadcaster;
    geometry_msgs::Quaternion quat;
    ros::Time last_time;


  // Receives drive callback messages and stores them
  void DriveCallback(const geometry_msgs::Twist& msg);

 public:
  DiffDriveModel() = delete;
  // Intialize a default object reading from a file
  DiffDriveModel(const std::vector<std::string>& config_files, ros::NodeHandle* n);
  ~DiffDriveModel() = default;
  // define Step function for updating
  void Step(const double& dt);
  void PublishOdom(const float dt);
};

}

#endif  // SRC_SIMULATOR_DIFFDRIVE_MODEL_H_