from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='uav',
            executable='uav_node',
            name='uav_node',
            parameters=[{'use_sim_time': True}]
        ),

        Node(
            package='uav',
            executable='ground_station_node',
            name='ground_station_node',
            parameters=[{'use_sim_time': True}]
        )
    ])