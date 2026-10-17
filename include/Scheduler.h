#pragma once
#include <stdint.h>
namespace Diyz {
// All time arithmetic uses modulo-2^32 unsigned differences. Intervals must be < 2^31 us.
struct LoopStats {
 uint32_t runs=0, missed=0, maxJitterUs=0, maxExecutionUs=0;
 uint32_t maxIntervalUs=0, minIntervalUs=0, lateStarts=0;
 uint32_t lateEpisodes=0, maxSkippedInEpisode=0;
 uint32_t maxPhaseErrorUs=0;
};
class PeriodicGate {
 uint32_t periodUs_, next_=0, lastStart_=0;
 bool initialized_=false;
 LoopStats stats_{};
public:
 explicit PeriodicGate(uint32_t periodUs):periodUs_(periodUs){}
 bool due(uint32_t now){
  if(!initialized_){initialized_=true;next_=now+periodUs_;lastStart_=now;return false;}
  if((int32_t)(now-next_)<0)return false;
  const uint32_t late=now-next_;
  if(late>stats_.maxJitterUs)stats_.maxJitterUs=late;
  if(late>stats_.maxPhaseErrorUs)stats_.maxPhaseErrorUs=late;
  if(late)++stats_.lateStarts;
  const uint32_t interval=now-lastStart_;
  if(stats_.runs==0 || interval<stats_.minIntervalUs)stats_.minIntervalUs=interval;
  if(interval>stats_.maxIntervalUs)stats_.maxIntervalUs=interval;
  lastStart_=now;
  const uint32_t skipped=late/periodUs_;
  if(skipped){
    ++stats_.lateEpisodes;
    stats_.missed+=skipped;
    if(skipped>stats_.maxSkippedInEpisode)stats_.maxSkippedInEpisode=skipped;
  }
  // Skip overdue ticks; never call a control callback repeatedly to 'catch up'.
  next_+=(skipped+1)*periodUs_;
  ++stats_.runs;
  return true;
 }
 void execution(uint32_t elapsed){if(elapsed>stats_.maxExecutionUs)stats_.maxExecutionUs=elapsed;}
 const LoopStats& stats()const{return stats_;}
};
// Measures actual time between consecutive iterations of loop(), independent of gates.
class LoopGapMonitor {
 uint32_t last_=0; bool initialized_=false;
 uint32_t maxGapUs_=0, gapsAbove5ms_=0, gapsAbove10ms_=0;
public:
 void sample(uint32_t now){
  if(!initialized_){last_=now;initialized_=true;return;}
  const uint32_t gap=now-last_;last_=now;
  if(gap>maxGapUs_)maxGapUs_=gap;
  if(gap>5000)++gapsAbove5ms_;
  if(gap>10000)++gapsAbove10ms_;
 }
 uint32_t maxGapUs()const{return maxGapUs_;}
 uint32_t gapsAbove5ms()const{return gapsAbove5ms_;}
 uint32_t gapsAbove10ms()const{return gapsAbove10ms_;}
};
}
