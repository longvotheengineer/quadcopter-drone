#include "rclcpp/rclcpp.hpp"
#include <chrono>
#include <functional>

using namespace std::chrono_literals;

class UavNode : public rclcpp::Node {
public:
    UavNode() : Node("my_first_uav_node") {
        RCLCPP_INFO(this->get_logger(), "The node has started.");
        timer_ = this->create_wall_timer(
            500ms, std::bind(&UavNode::timer_callback, this)
        );
    }

private:
    void timer_callback() {
        RCLCPP_INFO(this->get_logger(), "Test timer callback.");
    }

    rclcpp::TimerBase::SharedPtr timer_;
};


int main(int argc, char **argv) {
    rclcpp::init(argc, argv); // Initialize ROS 2
    rclcpp::spin(std::make_shared<UavNode>()); 
    rclcpp::shutdown(); 
    return 0;
}