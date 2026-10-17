#pragma once
#include <stdint.h>
namespace Diyz {
enum class RobotState : uint8_t { BOOT, SELF_TEST, UNCALIBRATED, READY, RUNNING, PAUSED, FAULT };
enum class DriveState : uint8_t { DRIVE_DISABLED, HOLDING, MOVING, STOPPING, INHIBITED };
inline const char* toString(RobotState s) {
  switch(s) { case RobotState::BOOT:return "BOOT"; case RobotState::SELF_TEST:return "SELF_TEST";
    case RobotState::UNCALIBRATED:return "UNCALIBRATED"; case RobotState::READY:return "READY";
    case RobotState::RUNNING:return "RUNNING"; case RobotState::PAUSED:return "PAUSED";
    case RobotState::FAULT:return "FAULT"; } return "INVALID";
}
inline const char* toString(DriveState s) {
  switch(s) { case DriveState::DRIVE_DISABLED:return "DISABLED"; case DriveState::HOLDING:return "HOLDING";
    case DriveState::MOVING:return "MOVING"; case DriveState::STOPPING:return "STOPPING";
    case DriveState::INHIBITED:return "INHIBITED"; } return "INVALID";
}
struct CalibrationValidity {
 bool zero_valid=false, limits_valid=false, motor_direction_valid=false, sensor_valid=false;
 bool workspace_valid=false, control_tuned=false, commissioning_approved=false;
 bool ready() const {return zero_valid&&limits_valid&&motor_direction_valid&&sensor_valid&&workspace_valid&&control_tuned&&commissioning_approved;}
};
struct RobotSnapshot { RobotState robot=RobotState::BOOT; DriveState drive=DriveState::DRIVE_DISABLED; CalibrationValidity validity{}; bool actuatorsEnabled=false; };
}
