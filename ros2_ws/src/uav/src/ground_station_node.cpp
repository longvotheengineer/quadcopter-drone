#include <chrono>
#include <stdint.h>
#include <rclcpp/rclcpp.hpp>
#include "std_msgs/msg/float64.hpp"

#include <px4_msgs/msg/offboard_control_mode.hpp>
#include <px4_msgs/msg/trajectory_setpoint.hpp>
#include <px4_msgs/msg/vehicle_command.hpp>

using std::placeholders::_1;
using namespace std::chrono_literals;

class GroundStationNode : public rclcpp::Node {
public:
    GroundStationNode() : Node("ground_station_node") {
        RCLCPP_INFO(this->get_logger(), "The node has started.");
        
        subscriber_ = this->create_subscription<std_msgs::msg::Float64>(
            "uav_altitude", 10, std::bind(
                &GroundStationNode::altitude_callback, this, _1
            )
        );

        // px4 publisher and hearbeat timer
        offboard_control_mode_publisher_ = this->create_publisher<px4_msgs::msg::
            OffboardControlMode>("/fmu/in/offboard_control_mode", 10);
        trajectory_setpoint_publisher_ = this->create_publisher<px4_msgs::msg::
            TrajectorySetpoint>("/fmu/in/trajectory_setpoint", 10);
        vehicle_command_publisher_ = this->create_publisher<px4_msgs::msg::
            VehicleCommand>("/fmu/in/vehicle_command", 10);

        offboard_setpoint_counter_ = 0;

        auto heartbeat_timer_callback = [this]() -> void {
            publish_offboard_control_mode();
            publish_trajectory_setpoint();

            if (offboard_setpoint_counter_ == 10) {
                this->publish_vehicle_command(px4_msgs::msg::VehicleCommand::VEHICLE_CMD_DO_SET_MODE, 1, 6); // 1 = Custom Mode, 6 = OFFBOARD
                this->arm();
            }
            if (offboard_setpoint_counter_ < 11) {
                offboard_setpoint_counter_++;
            }
        };
        heartbeat_timer_ = this->create_wall_timer(100ms, heartbeat_timer_callback);

    }

private:
    void altitude_callback(const std_msgs::msg::Float64 & msg) const {
        // if (msg.data > 5.0) {
        //     RCLCPP_INFO(this->get_logger(), "UAV has passed 5.0 meters. Current altitude: %f", msg.data);
        // } else {
        //     RCLCPP_INFO(this->get_logger(), "UAV is below 5.0 meters. Current altitude: %f", msg.data);
        // }
    }

    void publish_offboard_control_mode() {
        px4_msgs::msg::OffboardControlMode msg{};
        msg.position = true;
        msg.velocity = false;
        msg.acceleration = false;
        msg.attitude = false;
        msg.body_rate = false;
        msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
        offboard_control_mode_publisher_->publish(msg);
    }

    void publish_trajectory_setpoint() {
        px4_msgs::msg::TrajectorySetpoint msg{};
        msg.position = {0.0, 0.0, -5.0};
        msg.yaw = -3.14;
        msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
        trajectory_setpoint_publisher_->publish(msg);
    }

    void publish_vehicle_command(uint16_t command, float param1 = 0.0, float param2 = 0.0) {
        px4_msgs::msg::VehicleCommand msg{};
        msg.param1 = param1;
        msg.param2 = param2;
        msg.command = command;
        msg.target_system = 1;
        msg.target_component = 1;
        msg.source_system = 1;
        msg.source_component = 1;
        msg.from_external = 0;
        msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
        vehicle_command_publisher_->publish(msg);
    }

    void arm() {
        publish_vehicle_command(px4_msgs::msg::VehicleCommand::VEHICLE_CMD_COMPONENT_ARM_DISARM, 1.0);
        RCLCPP_INFO(this->get_logger(), "Sending ARM command. Preparing for liftoff...");
    }

    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr subscriber_;
    // px4 publisher and hearbeat timer
    rclcpp::Publisher<px4_msgs::msg::OffboardControlMode>::SharedPtr offboard_control_mode_publisher_;
    rclcpp::Publisher<px4_msgs::msg::TrajectorySetpoint>::SharedPtr trajectory_setpoint_publisher_;
    rclcpp::Publisher<px4_msgs::msg::VehicleCommand>::SharedPtr vehicle_command_publisher_;
    rclcpp::TimerBase::SharedPtr heartbeat_timer_;
    uint64_t offboard_setpoint_counter_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<GroundStationNode>()); 
    rclcpp::shutdown(); 
    return 0;
}