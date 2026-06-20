#include <functional>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

using std::placeholders::_1;

class GroundStationNode : public rclcpp::Node {
public:
    GroundStationNode() : Node("ground_station_node") {
        RCLCPP_INFO(this->get_logger(), "The node has started.");
        
        subscriber_ = this->create_subscription<std_msgs::msg::Float64>(
            "uav_altitude", 10, std::bind(
                &GroundStationNode::altitude_callback, this, _1
            )
        );
    }

private:
    void altitude_callback(const std_msgs::msg::Float64 & msg) const {
        if (msg.data > 5.0) {
            RCLCPP_INFO(this->get_logger(), "UAV has passed 5.0 meters. "
                                            "Current altitude: %f", msg.data);
        } else {
            RCLCPP_INFO(this->get_logger(), "UAV is below 5.0 meters. "
                                            "Current altitude: %f", msg.data);
        }
    }

    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr subscriber_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<GroundStationNode>()); 
    rclcpp::shutdown(); 
    return 0;
}