ROS2 Foxy:

colcon build

Usage:

ros2 launch turtlebot3_gazebo turtlebot3_AWS_small.launch.py 
(Spawns turtlebot3 in gazebo world)

Cartographer
ros2 launch turtlebot3_cartographer cartographer_laser.launch.py 

Gmapping
ros2 launch slam_gmapping gmapping_lidar_odom.launch.py 

for rf2o (Lidar Odom)
ros2 launch rf2o_laser_odometry rf2o_laser_odometry.launch.py 

Map location:
src/turtlebot3_simulations/turtlebot3_gazebo/maps


Please go to https://drive.google.com/drive/folders/1tcmeceL6Mu73eJsCMuEmK1O37OPUaQ2j?usp=sharing (since files were over 100MB, couldn't put it on Github)
