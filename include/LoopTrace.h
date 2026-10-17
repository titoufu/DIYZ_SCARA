#pragma once
#include <stdint.h>

namespace Diyz {

enum class LoopPhase : uint8_t {
    IDLE, ACQ, PLAN, SUP, TEL, SERIAL_IO, OTHER
};

inline const char* phaseName(LoopPhase phase) {
    switch (phase) {
        case LoopPhase::IDLE:   return "IDLE";
        case LoopPhase::ACQ:    return "ACQ";
        case LoopPhase::PLAN:   return "PLAN";
        case LoopPhase::SUP:    return "SUP";
        case LoopPhase::TEL:    return "TEL";
        case LoopPhase::SERIAL_IO: return "SERIAL";
        default:                return "OTHER";
    }
}

struct TraceEvent {
    uint32_t uptimeMs = 0;
    uint32_t gapUs = 0;
    uint32_t lastLoopWorkUs = 0;
    LoopPhase preceding = LoopPhase::IDLE;
    uint8_t core = 0;
};

class LoopTrace {
public:
    void phase(LoopPhase phaseId, uint32_t elapsedUs) {
        const unsigned idx = static_cast<unsigned>(phaseId);
        if (idx < 6 && elapsedUs > maxPhaseUs_[idx]) {
            maxPhaseUs_[idx] = elapsedUs;
        }
        lastPhase_ = phaseId;
        if (phaseId == LoopPhase::SERIAL_IO && elapsedUs > longestSerialUs_) {
            longestSerialUs_ = elapsedUs;
        }
    }

    void finish(uint32_t endUs, uint32_t workUs) {
        lastEndUs_ = endUs;
        ended_ = true;
        if (workUs > longestLoopWorkUs_) {
            longestLoopWorkUs_ = workUs;
        }
    }

    void start(uint32_t nowUs, uint32_t uptimeMs, uint8_t core) {
        if (!ended_) return;
        const uint32_t gap = nowUs - lastEndUs_;
        if (gap > 5000) ++eventsOver5ms_;
        if (gap > 10000) ++eventsOver10ms_;
        if (gap > maxGap_.gapUs) {
            maxGap_.uptimeMs = uptimeMs;
            maxGap_.gapUs = gap;
            maxGap_.lastLoopWorkUs = longestLoopWorkUs_;
            maxGap_.preceding = lastPhase_;
            maxGap_.core = core;
        }
    }

    const TraceEvent& event() const { return maxGap_; }
    uint32_t maxSerialUs() const { return longestSerialUs_; }
    uint32_t maxLoopWorkUs() const { return longestLoopWorkUs_; }
    uint32_t maxPhase(LoopPhase phaseId) const {
        const unsigned idx = static_cast<unsigned>(phaseId);
        return idx < 6 ? maxPhaseUs_[idx] : 0;
    }
    uint32_t gaps5() const { return eventsOver5ms_; }
    uint32_t gaps10() const { return eventsOver10ms_; }

private:
    uint32_t lastEndUs_ = 0;
    bool ended_ = false;
    LoopPhase lastPhase_ = LoopPhase::IDLE;
    uint32_t maxPhaseUs_[6] = {};
    uint32_t longestSerialUs_ = 0;
    uint32_t longestLoopWorkUs_ = 0;
    uint32_t eventsOver5ms_ = 0;
    uint32_t eventsOver10ms_ = 0;
    TraceEvent maxGap_ = {};
};

} // namespace Diyz
