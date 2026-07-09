#include "uav/uav_node.hpp"

using namespace std::chrono_literals;


UavNode::UavNode() : Node("uav_node") {
    RCLCPP_INFO(this->get_logger(), "The node has started.");
    
    // parameter
    this->declare_parameter("param_altitude", 0.0);

    // publisher
    publisher_ = this->create_publisher<std_msgs::msg::Float64>(
        "uav_altitude", 10
    );

    // subscriber
    // currently empty

    // service
    service_ = this->create_service<std_srvs::srv::Trigger>(
        "uav_service",
        std::bind(&UavNode::uav_service_callback, this,
        std::placeholders::_1, std::placeholders::_2)
    );

    // timer
    timer_ = this->create_wall_timer(
        500ms, std::bind(&UavNode::timer_callback, this)
    );
}

void UavNode::timer_callback() {
    // double param_altitude = this->get_parameter("param_altitude").as_double();
    // this->altitude_ = 7.0; // Simulate altitude change
    // auto message = std_msgs::msg::Float64();
    // message.data = this->altitude_;

    // RCLCPP_INFO(this->get_logger(), "[timer] triggered.");
    // publisher_->publish(message);
}

void UavNode::uav_service_callback(
    const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
    std::shared_ptr<std_srvs::srv::Trigger::Response> response) {
    (void) request;
    // action will be performed here
    response->success = true;
    response->message = "Successfully triggered the uav_service.";
}

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<UavNode>()); 
    rclcpp::shutdown(); 
    return 0;
}