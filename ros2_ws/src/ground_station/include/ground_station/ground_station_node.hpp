#include <chrono>
#include <stdint.h>
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float64.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>

#include <px4_msgs/msg/offboard_control_mode.hpp>
#include <px4_msgs/msg/trajectory_setpoint.hpp>
#include <px4_msgs/msg/vehicle_command.hpp>
#include <px4_msgs/msg/vehicle_odometry.hpp>

class GroundStationNode : public rclcpp::Node {
public:
    GroundStationNode();

private:
    // publisher: px4
    void publish_offboard_control_mode();
    void publish_trajectory_setpoint();
    void publish_vehicle_command(uint16_t command, float param1 = 0.0, float param2 = 0.0);
    // publisher: rviz
    void publish_rviz_pose(const px4_msgs::msg::VehicleOdometry & msg);

    // subscriber
    void target_position_callback(const geometry_msgs::msg::Point & msg);
    void odometry_callback(const px4_msgs::msg::VehicleOdometry & msg);

    // timer
    void heartbeat_timer_callback();

    // px4 
    void arm();

    // guidance: trajectory generation
    void update_target_position();

    // declare: final target position
    float final_x_ = 0.0;
    float final_y_ = 0.0;
    float final_z_ = 0.0;
    float pos_step_size_ = 0.1;
    // declare: current target position (send to px4-offboard)
    float target_x_ = 0.0;
    float target_y_ = 0.0;
    float target_z_ = 0.0;
    // declare: current position (feedback from px4-odemetry)
    float current_x_ = 0.0;
    float current_y_ = 0.0;
    float current_z_ = 0.0;
    // declare: publisher: px4
    rclcpp::Publisher<px4_msgs::msg::OffboardControlMode>::SharedPtr offboard_control_mode_publisher_;
    rclcpp::Publisher<px4_msgs::msg::TrajectorySetpoint>::SharedPtr trajectory_setpoint_publisher_;
    rclcpp::Publisher<px4_msgs::msg::VehicleCommand>::SharedPtr vehicle_command_publisher_;
    // declare: publisher: rviz
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr rviz_pose_publisher_;
    // declare: subscriber: target position
    rclcpp::Subscription<geometry_msgs::msg::Point>::SharedPtr target_position_subscriber_;
    // declare: subscriber: px4
    rclcpp::Subscription<px4_msgs::msg::VehicleOdometry>::SharedPtr odometry_subscriber_;
    // declare: timer: heartbeat
    rclcpp::TimerBase::SharedPtr heartbeat_timer_;
    uint64_t offboard_setpoint_counter_ = 0;
};