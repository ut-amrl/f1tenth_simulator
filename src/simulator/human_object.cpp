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
  \file    human_object.cpp
  \brief   C++ Interface: Human Object
  \author  Yifeng Zhu, (C) 2020
  \email   yifeng.zhu@utexas.edu
*/
//========================================================================

#include "simulator/human_object.h"

HumanObject::HumanObject() {
  // angle, (x, y)
  pose_ = Pose2Df(0., Eigen::Vector2f(0., 0.));
  // just a cylinder for now
  const float r = 0.3;
  const int num_segments = 20;

  const float angle_increment = 2 * M_PI / num_segments;

  Eigen::Vector2f v0(r, 0.);
  Eigen::Vector2f v1;
  const float eps = 0.001;
  for (int i = 1; i < num_segments; i++) {
    v1 = Eigen::Rotation2Df(angle_increment * i) * Eigen::Vector2f(r, 0.0);

    // TODO(yifeng): Fix the vector map bug that closed shape would have wrong occlusion
    Eigen::Vector2f eps_vec = (v1 - v0).normalized() * eps;
    template_lines_.push_back(geometry::line2f(v0 + eps_vec, v1 - eps_vec));
    v0 = v1;
  }
  template_lines_.push_back(geometry::line2f(v1, Eigen::Vector2f(r, 0.0)));
  pose_lines_ = template_lines_;
  this->Initialize();
}

HumanObject::HumanObject(const std::string& config_file) {
  // TODO(yifeng): Load the shape, start point, goal point and walking mode from a config file
  this->Initialize();
}


HumanObject::~HumanObject() {
}

void HumanObject::Initialize() {
  start_pose_ = pose_;
  mode_ = HumanMode::Singleshot;
  max_speed_ = 1.5;
  avg_speed_ = 1.0;
  reach_goal_threashold_ = 0.3;
}

void HumanObject::SetMode(const HumanMode& mode) {
  mode_ = mode;
}

void HumanObject::SetGoalPose(const Pose2Df& goal_pose) {
  goal_pose_ = goal_pose;
}

void HumanObject::SetPose(const Pose2Df& pose) {
  pose_ = pose;
  // update the shape according to the new pose
  this->Initialize();
  this->Transform();
}

void HumanObject::Transform() {
  Eigen::Rotation2Df R(math_util::AngleMod(pose_.angle));
  Eigen::Vector2f T = pose_.translation;

  for (size_t i=0; i < template_lines_.size(); i++) {
    pose_lines_[i].p0 = R * (template_lines_[i].p0) + T;
    pose_lines_[i].p1 = R * (template_lines_[i].p1) + T;
  }
}


void HumanObject::SetVel(const Eigen::Vector2f& trans_vel, const double& rot_vel) {
  trans_vel_ = trans_vel;
  rot_vel_ = rot_vel;
}

void HumanObject::Step(const double& dt) {
  // very simple dynamic update
  trans_vel_ = (goal_pose_.translation - pose_.translation).normalized() * avg_speed_;
  // TODO(yifeng): Add gaussian noise to the velocity

  // clip velocity if it is larger than max speed
  if (trans_vel_.norm() > max_speed_) {
    trans_vel_ = trans_vel_.normalized() * max_speed_;
  }

  pose_.Set(pose_.angle + rot_vel_ * dt, pose_.translation + trans_vel_ * dt);

  this->Transform();
  this->CheckReachGoal();
}

void HumanObject::SetSpeed(const double& max_speed, const double& avg_speed, const double& max_omega, const double& avg_omega) {
  max_speed_ = max_speed;
  avg_speed_ = avg_speed;
  max_omega_ = max_omega;
  avg_omega_ = avg_omega;
}

double HumanObject::GetMaxSpeed() {
  return max_speed_;
}

double HumanObject::GetAvgSpeed() {
  return avg_speed_;
}

bool HumanObject::CheckReachGoal() {
  if ((pose_.translation - goal_pose_.translation).norm() < this->reach_goal_threashold_) {
    // reached goal
    if (mode_ == HumanMode::Singleshot) {
      trans_vel_.setZero();
      rot_vel_ = 0.;
      return true;
    } else if (mode_ == HumanMode::Repeat) {
      Pose2Df temp = goal_pose_;
      goal_pose_ = start_pose_;
      start_pose_ = temp;
    }
  }
  return false;
}
