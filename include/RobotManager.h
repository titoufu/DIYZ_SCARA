#pragma once
#include "RobotTypes.h"
namespace Diyz {
class RobotManager {
 RobotSnapshot snapshot_{};
public:
 void begin(){snapshot_={};snapshot_.robot=RobotState::SELF_TEST;snapshot_.drive=DriveState::DRIVE_DISABLED;
  snapshot_.robot=RobotState::UNCALIBRATED;}
 const RobotSnapshot& snapshot()const{return snapshot_;}
 // Fase 0 não oferece transições para READY nem comandos de habilitação.
};
}
