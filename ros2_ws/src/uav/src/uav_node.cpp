#include <chrono>
#include <functional>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"
#include "std_srvs/srv/trigger.hpp"

using namespace std::chrono_literals;

class UavNode : public rclcpp::Node {
public:
    UavNode() : Node("uav_node") {
        RCLCPP_INFO(this->get_logger(), "The node has started.");
        
        // declare parameter
        this->declare_parameter("param_altitude", 0.0);

        publisher_ = this->create_publisher<std_msgs::msg::Float64>(
            "uav_altitude", 10
        );
        timer_ = this->create_wall_timer(
            500ms, std::bind(&UavNode::timer_callback, this)
        );

        // delare service
        service_ = this->create_service<std_srvs::srv::Trigger>(
            "uav_service",
            std::bind(&UavNode::uav_service_callback, this,
            std::placeholders::_1, std::placeholders::_2)
        );
    }

private:
    void timer_callback() {
        double param_altitude = this->get_parameter("param_altitude").as_double();
        this->altitude_ += param_altitude; // Simulate altitude change
        auto message = std_msgs::msg::Float64();
        message.data = this->altitude_;

        RCLCPP_INFO(this->get_logger(), "[timer] triggered.");
        publisher_->publish(message);
    }

    void uav_service_callback(
        const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
        std::shared_ptr<std_srvs::srv::Trigger::Response> response
    ) {
        (void) request;
        
        this->altitude_ = 0.0; // Reset altitude
        response->success = true;
        response->message = "Successfully reset the altitude to 0.0.";
        RCLCPP_WARN(this->get_logger(), "[service] triggered. "
                                        "Altitude reset to 0.0.");
    }

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher_;
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr service_;
    double altitude_ = 0.0;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<UavNode>()); 
    rclcpp::shutdown(); 
    return 0;
}