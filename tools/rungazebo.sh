#!/bin/bash
# Runs gazebo with the correct plugin path
GAZEBO_PLUGIN_PATH=$GAZEBO_PLUGIN_PATH:`pwd`/../build GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:`pwd`/../res/models gazebo -u
