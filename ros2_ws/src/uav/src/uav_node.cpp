#include <chrono>
#include <functional>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

using namespace std::chrono_literals;

class UavNode : public rclcpp::Node {
public:
    UavNode() : Node("uav_node") {
        RCLCPP_INFO(this->get_logger(), "The node has started.");
        
        publisher_ = this->create_publisher<std_msgs::msg::Float64>(
            "uav_altitude", 10
        );
        timer_ = this->create_wall_timer(
            500ms, std::bind(&UavNode::timer_callback, this)
        );
    }

private:
    void timer_callback() {
        auto message = std_msgs::msg::Float64();
        message.data = 10.0;

        RCLCPP_INFO(this->get_logger(), "[timer] triggered.");
        publisher_->publish(message);
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<UavNode>()); 
    rclcpp::shutdown(); 
    return 0;
}