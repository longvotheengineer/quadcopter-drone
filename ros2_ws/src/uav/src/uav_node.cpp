#include "rclcpp/rclcpp.hpp" 

class UavNode : public rclcpp::Node {
public:
    UavNode() : Node("my_first_uav_node") {
        RCLCPP_INFO(this->get_logger(), "The node has started.");
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv); // Initialize ROS 2
    rclcpp::spin(std::make_shared<UavNode>()); 
    rclcpp::shutdown(); 
    return 0;
}