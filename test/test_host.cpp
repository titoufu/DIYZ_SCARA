#define SERIAL 0x0
#define DISABLED 0x00
#include <cassert>
#include <cstring>
#include "Protocol.h"
#include "Scheduler.h"
#include "LoopTrace.h"
#include "SerialTiming.h"
#include "ConfigRecord.h"
#include "RobotTypes.h"
using namespace Diyz;
int main(){
 Command c;
 assert(parseCommand("PING ID=abc-123",c)==ParseResult::OK);
 assert(strcmp(c.verb,"PING")==0&&strcmp(c.id,"abc-123")==0);
 assert(parseCommand("PING",c)==ParseResult::OK&&!c.hasId);
 assert(parseCommand("PING ID=",c)==ParseResult::INVALID_ID);
 assert(parseCommand("PING OTHER=1",c)==ParseResult::INVALID_SYNTAX);
 assert(parseCommand("",c)==ParseResult::EMPTY);
 assert(parseCommand("PING ID=1 ID=2",c)==ParseResult::INVALID_ID);
 assert(strcmp(toString(DriveState::DRIVE_DISABLED),"DISABLED")==0);
 CalibrationValidity v;assert(!v.ready());
 PeriodicGate g(5000);assert(!g.due(0));assert(!g.due(4999));assert(g.due(5000));assert(g.due(16000));assert(g.stats().missed==1);
 ConfigRecord r=makeRecord(1);assert(validRecord(r));r.sequence=2;assert(!validRecord(r));
 ConfigRecord s=makeRecord(2);assert(validRecord(s));assert(laterSequence(s.sequence,1));
 assert(g.stats().maxIntervalUs==11000);assert(g.stats().missed==1);
 LoopTrace trace; trace.start(1000,1,1);trace.phase(LoopPhase::SERIAL_IO,12);trace.finish(1100,100);trace.start(27100,27,1);assert(trace.event().gapUs==26000);assert(trace.event().uptimeMs==27);assert(trace.event().preceding==LoopPhase::SERIAL_IO);assert(trace.gaps10()==1);
 SerialTiming timing;
 timing.sample(SerialStage::AVAILABLE,63500,123,false,1);
 timing.pollDone(63502,false,123,1);
 assert(timing.slowest().stage==SerialStage::AVAILABLE);
 assert(timing.slowest().durationUs==63500);
 assert(timing.maxEmptyPollUs()==63502);
 assert(timing.maxPollHadInput()==false);
 timing.sample(SerialStage::DISPATCH,25000,456,true,1);
 timing.pollDone(25004,true,456,1);
 assert(timing.maxStageUs(SerialStage::DISPATCH)==25000);
 assert(timing.pollsWithInput()==1);
 assert(strcmp(toString(RobotState::UNCALIBRATED),"UNCALIBRATED")==0);
}
