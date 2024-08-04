from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='rclcpp_components',
            executable='component_container_mt',
            name='remap_container',
            output='screen',
            parameters=[{'use_sim_time': True}],
            remappings=[
                ('/map', '/unused_map'),
                ('/map_metadata', '/unused_map_metadata'),
                ('/entropy', '/unused_entropy')
            ]
        )
    ])
