import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    use_sim_time = LaunchConfiguration('use_sim_time', default='true')
    lidar_param_file = LaunchConfiguration('lidar_param_file', default=os.path.join(
        get_package_share_directory('turtlebot3_gazebo'), 'config', 'lidar_0.9deg.yaml'))

    turtlebot3_model = os.environ.get('TURTLEBOT3_MODEL', 'burger')
    
    # Ensure correct world file path
    world_file_name = 'test_zone.world'
    world = os.path.join(
        get_package_share_directory('turtlebot3_gazebo'),
        'worlds', 
        world_file_name
    )

    if not os.path.exists(world):
        raise FileNotFoundError(f"World file '{world}' not found at {world}.")
    
    launch_file_dir = os.path.join(get_package_share_directory('turtlebot3_gazebo'), 'launch')
    pkg_gazebo_ros = get_package_share_directory('gazebo_ros')

    return LaunchDescription([
        DeclareLaunchArgument(
            'lidar_param_file',
            default_value=os.path.join(get_package_share_directory('turtlebot3_gazebo'), 'config', 'lidar_0.9deg.yaml'),
            description='Path to the Lidar parameter file'
        ),

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                os.path.join(pkg_gazebo_ros, 'launch', 'gzserver.launch.py')
            ),
            launch_arguments={'world': world}.items(),
        ),

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                os.path.join(pkg_gazebo_ros, 'launch', 'gzclient.launch.py')
            ),
        ),

        IncludeLaunchDescription(
            PythonLaunchDescriptionSource([launch_file_dir, '/robot_state_publisher.launch.py']),
            launch_arguments={'use_sim_time': use_sim_time}.items(),
        ),

        Node(
            package='hls_lfcd_lds_driver',
            executable='hlds_laser_publisher',
            name='turtlebot3_laserscan',
            parameters=[lidar_param_file],
            output='screen'
        )
    ])
