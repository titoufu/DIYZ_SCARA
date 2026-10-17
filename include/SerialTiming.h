#pragma once
#include <stdint.h>

namespace Diyz {
// Wall-clock measurements: a FreeRTOS preemption within a stage is included.
enum class SerialStage : uint8_t { NONE, AVAILABLE, READ, DISPATCH };
inline const char* serialStageName(SerialStage s) {
    switch (s) {
        case SerialStage::AVAILABLE: return "AVAILABLE";
        case SerialStage::READ: return "READ";
        case SerialStage::DISPATCH: return "DISPATCH";
        default: return "NONE";
    }
}
struct SerialSlowEvent {
    uint32_t durationUs = 0;
    uint32_t atUptimeMs = 0;
    SerialStage stage = SerialStage::NONE;
    bool hadInput = false;
    uint8_t core = 0;
};
class SerialTiming {
public:
    void sample(SerialStage stage, uint32_t durationUs, uint32_t uptimeMs, bool hadInput, uint8_t core) {
        const unsigned idx = static_cast<unsigned>(stage);
        if (idx < 1 || idx > 3) return;
        if (durationUs > maxStageUs_[idx]) maxStageUs_[idx] = durationUs;
        if (durationUs > slowest_.durationUs) {
            slowest_.durationUs = durationUs;
            slowest_.atUptimeMs = uptimeMs;
            slowest_.stage = stage;
            slowest_.hadInput = hadInput;
            slowest_.core = core;
        }
    }
    void pollDone(uint32_t durationUs, bool hadInput, uint32_t uptimeMs, uint8_t core) {
        if (hadInput) ++pollsWithInput_; else ++pollsEmpty_;
        if (durationUs > maxPollUs_) {
            maxPollUs_ = durationUs;
            maxPollAtMs_ = uptimeMs;
            maxPollHadInput_ = hadInput;
            maxPollCore_ = core;
        }
        if (!hadInput && durationUs > maxEmptyPollUs_) maxEmptyPollUs_ = durationUs;
        if (hadInput && durationUs > maxBusyPollUs_) maxBusyPollUs_ = durationUs;
    }
    const SerialSlowEvent& slowest() const { return slowest_; }
    uint32_t maxStageUs(SerialStage s) const { return maxStageUs_[static_cast<unsigned>(s)]; }
    uint32_t maxPollUs() const { return maxPollUs_; }
    uint32_t maxEmptyPollUs() const { return maxEmptyPollUs_; }
    uint32_t maxBusyPollUs() const { return maxBusyPollUs_; }
    uint32_t maxPollAtMs() const { return maxPollAtMs_; }
    bool maxPollHadInput() const { return maxPollHadInput_; }
    uint8_t maxPollCore() const { return maxPollCore_; }
    uint32_t pollsEmpty() const { return pollsEmpty_; }
    uint32_t pollsWithInput() const { return pollsWithInput_; }
private:
    uint32_t maxStageUs_[4] = {};
    SerialSlowEvent slowest_ = {};
    uint32_t maxPollUs_ = 0, maxEmptyPollUs_ = 0, maxBusyPollUs_ = 0, maxPollAtMs_ = 0;
    uint32_t pollsEmpty_ = 0, pollsWithInput_ = 0;
    bool maxPollHadInput_ = false;
    uint8_t maxPollCore_ = 0;
};
} // namespace Diyz
