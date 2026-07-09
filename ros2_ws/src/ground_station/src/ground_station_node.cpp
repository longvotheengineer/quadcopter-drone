#include "ground_station/ground_station_node.hpp"

using std::placeholders::_1;
using namespace std::chrono_literals;

GroundStationNode::GroundStationNode() : Node("ground_station_node") {
    RCLCPP_INFO(this->get_logger(), "The node has started.");

    // publisher: px4
    offboard_control_mode_publisher_ = this->create_publisher<px4_msgs::msg::
        OffboardControlMode>("/fmu/in/offboard_control_mode", 10);
    trajectory_setpoint_publisher_ = this->create_publisher<px4_msgs::msg::
        TrajectorySetpoint>("/fmu/in/trajectory_setpoint", 10);
    vehicle_command_publisher_ = this->create_publisher<px4_msgs::msg::
        VehicleCommand>("/fmu/in/vehicle_command", 10);
    // publisher: rviz
    rviz_pose_publisher_ = this->create_publisher<geometry_msgs::msg::
        PoseStamped>("rviz_pose", 10);

    // subscriber: odometry
    odometry_subscriber_ = this->create_subscription<px4_msgs::msg::VehicleOdometry>(
        "/fmu/out/vehicle_odometry", rclcpp::QoS(10).best_effort(), std::bind(
            &GroundStationNode::odometry_callback, this, _1
        )
    );
    // subscriber: target position
    target_position_subscriber_ = this->create_subscription<geometry_msgs::msg::Point>(
        "uav_position", 10, std::bind(
            &GroundStationNode::target_position_callback, this, _1
        )
    );

    // timer: heartbeat
    heartbeat_timer_ = this->create_wall_timer(
        100ms, [this]() {heartbeat_timer_callback();});
}

// publisher: px4
void GroundStationNode::publish_offboard_control_mode() {
    px4_msgs::msg::OffboardControlMode msg{};
    msg.position = true;
    msg.velocity = false;
    msg.acceleration = false;
    msg.attitude = false;
    msg.body_rate = false;
    msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
    offboard_control_mode_publisher_->publish(msg);
}

void GroundStationNode::publish_trajectory_setpoint() {
    px4_msgs::msg::TrajectorySetpoint msg{};
    msg.position = {target_x_, target_y_, -target_z_};
    msg.yaw = -3.14;
    msg.timestamp = this->get_clock()->now().nanoseconds() / 1000;
    trajectory_setpoint_publisher_->publish(msg);
}

void GroundStationNode::publish_vehicle_command(uint16_t command, float param1, float param2) {
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

// publisher: rviz
void GroundStationNode::publish_rviz_pose(const px4_msgs::msg::VehicleOdometry & msg) {
    geometry_msgs::msg::PoseStamped pose_msg{};
    pose_msg.header.stamp = this->get_clock()->now();
    pose_msg.header.frame_id = "map";
    pose_msg.pose.position.x = msg.position[1];
    pose_msg.pose.position.y = msg.position[0];
    pose_msg.pose.position.z = -msg.position[2];
    rviz_pose_publisher_->publish(pose_msg);
}

// subscriber
void GroundStationNode::target_position_callback(const geometry_msgs::msg::Point & msg) {
    final_x_ = msg.x;
    final_y_ = msg.y;
    final_z_ = msg.z;
    RCLCPP_INFO(this->get_logger(), "Received target position: %f, %f, %f", 
                                            final_x_, final_y_, final_z_);
}

void GroundStationNode::odometry_callback(const px4_msgs::msg::VehicleOdometry & msg) {
    current_x_ = msg.position[0];
    current_y_ = msg.position[1];
    current_z_ = msg.position[2];

    publish_rviz_pose(msg);
}

// timer
void GroundStationNode::heartbeat_timer_callback() {
    update_target_position();
    publish_offboard_control_mode();
    publish_trajectory_setpoint();

    RCLCPP_INFO(this->get_logger(), "Current Position: x:%.2f | y:%.2f | z:%.2f | %.2f | %.2f | %.2f",       // test: log current position 
                                                current_x_, current_y_, -current_z_, target_x_, target_y_, -target_z_);   // (feedback from px4-odometry)

    if (offboard_setpoint_counter_ == 10) {
        this->publish_vehicle_command(px4_msgs::msg::VehicleCommand::VEHICLE_CMD_DO_SET_MODE, 1, 6); // 1 = Custom Mode, 6 = OFFBOARD
        this->arm();
    }
    if (offboard_setpoint_counter_ < 11) {
        offboard_setpoint_counter_++;
    }
};

// px4 
void GroundStationNode::arm() {
    publish_vehicle_command(px4_msgs::msg::VehicleCommand::VEHICLE_CMD_COMPONENT_ARM_DISARM, 1.0);
    RCLCPP_INFO(this->get_logger(), "Sending ARM command. Preparing for liftoff...");
}

// guidance: trajectory generation
void GroundStationNode::update_target_position() {
    if (target_x_ < final_x_) {
        target_x_ += std::min(pos_step_size_, final_x_ - target_x_);
    } else if (target_x_ > final_x_) {
        target_x_ -= std::min(pos_step_size_, target_x_ - final_x_);
    }

    if (target_y_ < final_y_) {
        target_y_ += std::min(pos_step_size_, final_y_ - target_y_);
    } else if (target_y_ > final_y_) {
        target_y_ -= std::min(pos_step_size_, target_y_ - final_y_);
    }

    if (target_z_ < final_z_) {
        target_z_ += std::min(pos_step_size_, final_z_ - target_z_);
    } else if (target_z_ > final_z_) {
        target_z_ -= std::min(pos_step_size_, target_z_ - final_z_);
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<GroundStationNode>()); 
    rclcpp::shutdown(); 
    return 0;
}