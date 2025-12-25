# JazzyCopter Simulation 

[![ROS 2 Jazzy](https://img.shields.io/badge/ROS_2-Jazzy-blue?logo=ros&logoColor=white)](https://docs.ros.org/en/jazzy/)
![C](https://img.shields.io/badge/C-00599C?logo=c&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?logo=python&logoColor=white)
[![Gazebo Harmonic](https://img.shields.io/badge/Gazebo-Harmonic-orange?logo=gazebo&logoColor=white)](https://gazebosim.org/home)
[![Ubuntu 24.04](https://img.shields.io/badge/OS-Ubuntu_24.04-E95420?logo=ubuntu&logoColor=white)](https://releases.ubuntu.com/noble/)
[![License](https://img.shields.io/badge/License-Apache_2.0-green.svg)](LICENSE)

**JazzyCopter** is a modular quadcopter simulation framework designed for **ROS 2 Jazzy Jalisco** running on **Ubuntu 24.04**. It leverages **Gazebo Harmonic** for physics rendering and utilizes a hybrid **C++** (performance) and **Python** (scripting/prototyping) architecture.

## Preview

<Images will be added later ~>

## Features

* **Physics Engine:** High-fidelity simulation using Gazebo Harmonic (gz-sim).
* **ROS 2 Integration:** Full usage of the `ros_gz_bridge` for sensor data and control command communication.
* **Hybrid Control:**
    * Low-level motor mixing and stabilization in **C++**.
    * High-level path planning and state estimation in **Python**.
* **Sensors:** Simulated IMU, GPS, and Lidar support.

## Prerequisites

Ensure your system meets the following requirements:

* **OS:** Ubuntu 24.04 LTS (Noble Numbat)
* **ROS 2:** Jazzy Jalisco ([Installation Guide](https://docs.ros.org/en/jazzy/Installation.html))
* **Simulator:** Gazebo Harmonic

### Install Dependencies
```bash
sudo apt update
sudo apt install ros-jazzy-ros-gz ros-jazzy-xacro python3-colcon-common-extensions
