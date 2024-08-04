#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <rf2o_laser_odometry/rf2o_laser_odometry_node.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

class EnhancedRf2o : public rf2o_laser_odometry::Rf2oLaserOdometryNode
{
public:
  EnhancedRf2o() : rf2o_laser_odometry::Rf2oLaserOdometryNode()
  {
    // Subscribe to IMU data
    imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
      "imu/data", 10, std::bind(&EnhancedRf2o::imuCallback, this, std::placeholders::_1));

    // Subscribe to Odometry data
    odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
      "odom", 10, std::bind(&EnhancedRf2o::odomCallback, this, std::placeholders::_1));

    // Publisher for the updated pose
    pose_pub_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("updated_pose", 10);
  }

private:
  rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
  rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pose_pub_;
  sensor_msgs::msg::Imu::SharedPtr last_imu_msg_;
  geometry_msgs::msg::Pose current_pose_;

  void imuCallback(const sensor_msgs::msg::Imu::SharedPtr msg)
  {
    last_imu_msg_ = msg;
  }

  void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg)
  {
    current_pose_ = msg->pose.pose;
  }

  void processLaserScan(const sensor_msgs::msg::LaserScan::SharedPtr scan_msg)
  {
    if (isLargeOpenSpace(scan_msg))
    {
      // Use supplementary IMU data
      if (last_imu_msg_)
      {
        integrateIMUData(scan_msg, last_imu_msg_);
      }
    }

    // Use rf2o odometry
    rf2o_laser_odometry::Rf2oLaserOdometryNode::processLaserScan(scan_msg);
  }

  bool isLargeOpenSpace(const sensor_msgs::msg::LaserScan::SharedPtr scan_msg)
  {
    int far_points = 0;
    const float threshold_distance = 4.0; // 4 meters threshold can be different

    for (const auto &range : scan_msg->ranges)
    {
      // Check if the range is greater than the threshold distance
      if (range > threshold_distance)
      {
        far_points++;
      }
    }

    // If more than 50% of the points are far, consider it a large open space
    return far_points > (scan_msg->ranges.size() * 0.5);
  }

  void integrateIMUData(const sensor_msgs::msg::LaserScan::SharedPtr scan_msg, const sensor_msgs::msg::Imu::SharedPtr imu_msg)
  {
    // Extract orientation from IMU
    tf2::Quaternion imu_orientation;
    tf2::convert(imu_msg->orientation, imu_orientation);

    // Integrate the IMU orientation with the current pose
    current_pose_.orientation = imu_orientation;

    // Publish the updated pose
    publishPose(current_pose_);
  }

  void publishPose(const geometry_msgs::msg::Pose &pose)
  {
    geometry_msgs::msg::PoseStamped pose_stamped;
    pose_stamped.header.stamp = this->now();
    pose_stamped.header.frame_id = "base_link"; 
    pose_stamped.pose = pose;

    pose_pub_->publish(pose_stamped);
  }
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<EnhancedRf2o>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
