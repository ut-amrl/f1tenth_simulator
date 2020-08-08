//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
\file    simulator.h
\brief   C++ Interface: Simulator
\author  Joydeep Biswas, (C) 2011
*/
//========================================================================

#include <stdio.h>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include "eigen3/Eigen/Dense"
#include "geometry_msgs/Point32.h"
#include "geometry_msgs/PoseStamped.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "nav_msgs/Odometry.h"
#include "ros/package.h"
#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "tf/transform_broadcaster.h"
#include "tf/transform_datatypes.h"
#include "visualization_msgs/Marker.h"

#include "ut_multirobot_sim/AckermannCurvatureDriveMsg.h"
#include "ut_multirobot_sim/Localization2DMsg.h"

#include "shared/math/geometry.h"
#include "shared/util/timer.h"
#include "simulator/vector_map.h"
#include "config_reader/config_reader.h"

#include "entity_base.h"
#include "human_object.h"
#include "robot_model.h"
#include "short_term_object.h"

#ifndef SIMULATOR_H
#define SIMULATOR_H

using namespace std;
using pose_2d::Pose2Df;

class Simulator {
  config_reader::ConfigReader reader_;
  config_reader::ConfigReader init_config_reader_;

  Pose2Df vel_;
  Pose2Df cur_loc_;

  std::vector<std::unique_ptr<EntityBase>> objects;

  ros::Subscriber initSubscriber;

  ros::Publisher odometryTwistPublisher;
  ros::Publisher laserPublisher;
  ros::Publisher viz_laser_publisher_;
  ros::Publisher mapLinesPublisher;
  ros::Publisher posMarkerPublisher;
  ros::Publisher objectLinesPublisher;
  ros::Publisher truePosePublisher;
  ros::Publisher localizationPublisher;
  ros::Publisher humanStateArrayPublisher;
  tf::TransformBroadcaster *br;

  sensor_msgs::LaserScan scanDataMsg;
  nav_msgs::Odometry odometryTwistMsg;
  ut_multirobot_sim::Localization2DMsg localizationMsg;

  vector_map::VectorMap map_;

  visualization_msgs::Marker lineListMarker;
  visualization_msgs::Marker robotPosMarker;
  visualization_msgs::Marker objectLinesMarker;

  static const float DT;
  geometry_msgs::PoseStamped truePoseMsg;

  std::default_random_engine rng_;
  std::normal_distribution<float> laser_noise_;

  std::unique_ptr<robot_model::RobotModel> motion_model_;
  std::string robot_type_;

 private:
  void initVizMarker(visualization_msgs::Marker &vizMarker, string ns, int id,
                     string type, geometry_msgs::PoseStamped p,
                     geometry_msgs::Point32 scale, double duration,
                     std::vector<float> color);
  void initSimulatorVizMarkers();
  void drawMap();
  void drawObjects();
  void InitalLocationCallback(
      const geometry_msgs::PoseWithCovarianceStamped &msg);
  void DriveCallback(const ut_multirobot_sim::AckermannCurvatureDriveMsg &msg);
  void publishOdometry();
  void publishLaser();
  void publishVisualizationMarkers();
  void publishTransform();
  void update();
  void loadObject();

 public:
  Simulator() = delete;
  explicit Simulator(const std::string& sim_config);
  ~Simulator();
  bool init(ros::NodeHandle &n);
  void Run();
};
#endif  // SIMULATOR_H
