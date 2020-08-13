/*
# Copyright (c) 2016-2020 Murilo Marques Marinho
#
#    This file is part of rosilo_robot_driver.
#
#    rosilo_robot_driver is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    rosilo_robot_driver is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with rosilo_robot_driver.  If not, see <https://www.gnu.org/licenses/>.
#
# ################################################################
#
#   Author: Murilo M. Marinho, email: murilo@nml.t.u-tokyo.ac.jp
#
# ################################################################*/
#include <rosilo_robot_driver/rosilo_robot_driver_provider.h>

#include <rosilo_conversions/rosilo_conversions.h>

namespace rosilo
{

void RobotDriverProvider::_callback_target_joint_positions(const std_msgs::Float64MultiArrayConstPtr &msg)
{
    target_joint_positions_ = std_vector_double_to_vectorxd(msg->data);

    if(!enabled_)
        enabled_ = true;
}

RobotDriverProvider::RobotDriverProvider(ros::NodeHandle &nodehandle):
    enabled_(false)
{
    publisher_joint_states_ = nodehandle.advertise<sensor_msgs::JointState>(ros::this_node::getName() + "/get/joint_states", 1);
    publisher_joint_limits_min_ = nodehandle.advertise<std_msgs::Float64MultiArray>(ros::this_node::getName() + "/get/joint_positions_min", 1);
    publisher_joint_limits_max_ = nodehandle.advertise<std_msgs::Float64MultiArray>(ros::this_node::getName() + "/get/joint_positions_max", 1);

    subscriber_target_joint_positions_ = nodehandle.subscribe(ros::this_node::getName() + "set/target_joint_positions", 1, &RobotDriverProvider::_callback_target_joint_positions, this);
}

RobotDriverProvider::RobotDriverProvider(ros::NodeHandle &publisher_nodehandle, ros::NodeHandle &subscriber_nodehandle)
{
    publisher_joint_states_ = publisher_nodehandle.advertise<sensor_msgs::JointState>(ros::this_node::getName() + "/get/joint_states", 1);
    publisher_joint_limits_min_ = publisher_nodehandle.advertise<std_msgs::Float64MultiArray>(ros::this_node::getName() + "/get/joint_positions_min", 1);
    publisher_joint_limits_max_ = publisher_nodehandle.advertise<std_msgs::Float64MultiArray>(ros::this_node::getName() + "/get/joint_positions_max", 1);

    subscriber_target_joint_positions_ = subscriber_nodehandle.subscribe(ros::this_node::getName() + "set/target_joint_positions", 1, &RobotDriverProvider::_callback_target_joint_positions, this);
}

VectorXd RobotDriverProvider::get_target_joint_positions() const
{
    if(is_enabled())
        return target_joint_positions_;
    else
        throw std::runtime_error(ros::this_node::getName() + "::RobotDriverProvider::get_target_joint_positions() trying to get an uninitialized vector");
}

void RobotDriverProvider::send_joint_positions(const VectorXd &joint_positions)
{
    sensor_msgs::JointState ros_msg;
    ros_msg.position = vectorxd_to_std_vector_double(joint_positions);
    publisher_joint_states_.publish(ros_msg);
}

void RobotDriverProvider::send_joint_limits(const std::tuple<VectorXd, VectorXd> &joint_limits)
{
    std_msgs::Float64MultiArray ros_msg_min;
    ros_msg_min.data = vectorxd_to_std_vector_double(std::get<0>(joint_limits));
    publisher_joint_limits_min_.publish(ros_msg_min);

    std_msgs::Float64MultiArray ros_msg_max;
    ros_msg_max.data = vectorxd_to_std_vector_double(std::get<1>(joint_limits));
    publisher_joint_limits_max_.publish(ros_msg_max);
}

bool RobotDriverProvider::is_enabled() const
{
    return enabled_;
}

}
