#include "simulator/omnidirectional_model.h"
#include <eigen3/Eigen/src/Geometry/Rotation2D.h>
#include "shared/util/timer.h"
#include "shared/math/math_util.h"
#include "ut_multirobot_sim/CobotOdometryMsg.h"

#include <string>

using Eigen::Vector2f;
using Eigen::Rotation2Df;
using ut_multirobot_sim::CobotDriveMsg;
using ut_multirobot_sim::CobotOdometryMsg;
using math_util::AngleDiff;
using math_util::AngleMod;
using math_util::Sign;
using geometry::Heading;
using std::isfinite;
using std::string;
using std::vector;

namespace omnidrive {

CONFIG_FLOAT(max_accel, "co_max_accel");
CONFIG_FLOAT(max_angle_accel, "co_max_angle_accel");
CONFIG_FLOAT(max_speed, "co_max_speed");
CONFIG_FLOAT(max_angle_vel, "co_max_angle_vel");
CONFIG_FLOAT(w0, "co_w0");
CONFIG_FLOAT(w1, "co_w1");
CONFIG_FLOAT(w2, "co_w2");
CONFIG_FLOAT(w3, "co_w3");
CONFIG_FLOAT(base_r, "co_base_radius");
CONFIG_STRING(drive_topic, "co_drive_callback_topic");
CONFIG_STRING(odom_topic, "co_cobot_odom_topic");
CONFIG_FLOAT(radius, "cobot_radius");
CONFIG_FLOAT(num_segments, "cobot_num_segments");
CONFIG_VECTOR2F(offset_vec, "cobot_offset");


OmnidirectionalModel::OmnidirectionalModel(const vector<string>& config_files, ros::NodeHandle* n, string topic_prefix = "") :
    RobotModel(),
    last_cmd_(),
    t_last_cmd_(0),
    angular_error_(0, 1),
    config_reader_(config_files){
  // Use the config reader to initialize the subscriber
  drive_subscriber_ = n->subscribe(
      topic_prefix + CONFIG_drive_topic,
      1,
      &OmnidirectionalModel::DriveCallback,
      this);
  odom_publisher_ = n->advertise<CobotOdometryMsg>(topic_prefix + CONFIG_odom_topic, 1);
  this->SetTemplateLines(CONFIG_radius, CONFIG_num_segments);
  this->Transform();
}

void OmnidirectionalModel::DriveCallback(const CobotDriveMsg& msg) {
  if (!isfinite(msg.velocity_x) ||
      !isfinite(msg.velocity_y) ||
      !isfinite(msg.velocity_r)) {
    printf("Ignoring non-finite drive values: %f, %f, %f\n",
        msg.velocity_x, msg.velocity_y, msg.velocity_r);
  }
  last_cmd_ = msg;
  t_last_cmd_ = GetMonotonicTime();
}

void OmnidirectionalModel::PublishOdom(const float dt) {
  const Vector2f w0 = Heading(CONFIG_w0);
  const Vector2f w1 = Heading(CONFIG_w1);
  const Vector2f w2 = Heading(CONFIG_w2);
  const Vector2f w3 = Heading(CONFIG_w3);
  CobotOdometryMsg msg;
  msg.dr = vel_.angle * dt;
  msg.dx = vel_.translation.x() * dt;
  msg.dy = vel_.translation.y() * dt;
  msg.v0 = vel_.translation.dot(w0)+CONFIG_base_r*vel_.angle;
  msg.v1 = vel_.translation.dot(w1)+CONFIG_base_r*vel_.angle;
  msg.v2 = vel_.translation.dot(w2)+CONFIG_base_r*vel_.angle;
  msg.v3 = vel_.translation.dot(w3)+CONFIG_base_r*vel_.angle;
  msg.vr = vel_.angle;
  msg.vx = vel_.translation.x();
  msg.vy = vel_.translation.y();
  msg.VBatt = 32.0;
  msg.status = 0x04;
  odom_publisher_.publish(msg);
}

//TODO(jaholtz) Add noise
void OmnidirectionalModel::Step(const double &dt) {
  // TODO(jaholtz) For faster than real time simulation we may need
  // a wallclock invariant method for this.
  static const double kMaxCommandAge = 0.1;
  if (GetMonotonicTime() > t_last_cmd_ + kMaxCommandAge) {
    last_cmd_.velocity_x = 0;
    last_cmd_.velocity_y = 0;
    last_cmd_.velocity_r = 0;
  }

  // Cap Velocity to max speed
  Vector2f desired_vel(last_cmd_.velocity_x, last_cmd_.velocity_y);
  if (desired_vel.norm() > CONFIG_max_speed) {
    desired_vel = CONFIG_max_speed * desired_vel.normalized();
  }

  // Cap acceleration to max accel
  Vector2f delta_v = desired_vel - vel_.translation;
  const float max_accel = CONFIG_max_accel * dt;
  if (delta_v.norm() > max_accel) {
    delta_v = max_accel * delta_v.normalized();
  }

  // Update tranlastional velocity
  vel_.translation += delta_v;

  // Cap the rotational velocity and acceleration
  float desired_ang_vel = last_cmd_.velocity_r;
  if (fabs(desired_ang_vel) > CONFIG_max_angle_vel) {
    desired_ang_vel = Sign(desired_ang_vel) * CONFIG_max_angle_vel;
  }
  const float max_angle_accel = CONFIG_max_angle_accel * dt;
  float delta_ang_v = desired_ang_vel - vel_.angle;
  if (fabs(desired_ang_vel) > max_angle_accel) {
    delta_ang_v = Sign(delta_ang_v) * max_angle_accel;
  }
  vel_.angle += delta_ang_v;

  pose_.translation += Rotation2Df(pose_.angle) * vel_.translation * dt;
  pose_.angle = AngleMod(pose_.angle + vel_.angle * dt);
  PublishOdom(dt);
  this->Transform();
}

void OmnidirectionalModel::Transform() {
  Eigen::Rotation2Df R(math_util::AngleMod(pose_.angle));
  Eigen::Vector2f T = pose_.translation;

  for (size_t i=0; i < template_lines_.size(); i++) {
    pose_lines_[i].p0 = R * (template_lines_[i].p0) + T;
    pose_lines_[i].p1 = R * (template_lines_[i].p1) + T;
  }
}

void OmnidirectionalModel::SetTemplateLines(const float r, const int num_segments){
  // copied directly from human model. In future refractor the code to 
  // enable inherentance might be more optimal. For now there is an excessive
  // amount of unwanted code in HumanModel, which disables inherenting directly.

  const float angle_increment = 2 * M_PI / num_segments;

  Eigen::Vector2f v0(r, 0.);
  Eigen::Vector2f v1;
  Eigen::Vector2f offset_vec = CONFIG_offset_vec;
  const float eps = 0.0005; // lets see if this causes this issue.
  for (int i = 1; i < num_segments; i++) {
    v1 = Eigen::Rotation2Df(angle_increment * i) * Eigen::Vector2f(r, 0.0);

    // TODO(yifeng): Fix the vector map bug that closed shape would have wrong occlusion
    Eigen::Vector2f eps_vec = (v1 - v0).normalized() * eps;
    template_lines_.push_back(geometry::Line2f(v0 + eps_vec + offset_vec, v1 - eps_vec + offset_vec));
    v0 = v1;
  }
  template_lines_.push_back(geometry::Line2f(v1, Eigen::Vector2f(r, 0.0)));
  pose_lines_ = template_lines_;
}

} // namespace omnidrive
