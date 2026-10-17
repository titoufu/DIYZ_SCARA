#include <Arduino.h>
#include <string.h>
#include "BoardHAL.h"
#include "RobotManager.h"
#include "ConfigManager.h"
#include "Protocol.h"
#include "Scheduler.h"
#include "LoopTrace.h"
#include "SerialTiming.h"
#include "TxQueue.h"
using namespace Diyz;
static RobotManager robot;
static ConfigManager config;
static PeriodicGate acquisition(5000), planning(10000), supervisor(20000), telemetry(200000);
static char rx[MAX_LINE];
static size_t rxLen = 0;
static bool overflow = false;
static constexpr const char *FW_VERSION = "0.0.11-fase0-stable";
static LoopGapMonitor loopGap;
static LoopTrace loopTrace;
static SerialTiming serialTiming;
static constexpr uint32_t SERIAL_BUDGET = 64;
// Single owner: loopTask. No concurrent writes from other tasks/interrupts.
static TxQueue<8192, 2048> tx;
static constexpr size_t TX_FLUSH_MAX = 128;
// Before reading any command, reserve room for one worst-case response.
// This is internal backpressure only, NOT PC-to-device flow control.
static constexpr size_t TX_RESPONSE_RESERVE = 2048;
static uint32_t txFlushMaxUs = 0;
static uint32_t txBackpressurePolls = 0;
static void prefix(const char *kind, const Command &cmd)
{
    tx.print(kind);
    if (cmd.hasId)
    {
        tx.print(" ID=");
        tx.print(cmd.id);
    }
}
static void processLine(const char *line)
{
    Command cmd;
    const ParseResult result = parseCommand(line, cmd);
    if (result == ParseResult::EMPTY)
        return;
    if (result != ParseResult::OK)
    {
        tx.print("ERR CODE=");
        tx.println(errorName(result));
        return;
    }
    if (strcmp(cmd.verb, "PING") == 0)
    {
        prefix("OK", cmd);
        tx.println(" CMD=PING PONG=1");
    }
    else if (strcmp(cmd.verb, "VERSION") == 0)
    {
        prefix("OK", cmd);
        tx.print(" CMD=VERSION FW=");
        tx.print(FW_VERSION);
        tx.println(" PROTO=1 HW=ESP32-S3-N16R8");
    }
    else if (strcmp(cmd.verb, "STATUS") == 0)
    {
        const auto &s = robot.snapshot();
        prefix("OK", cmd);
        tx.print(" CMD=STATUS ROBOT=");
        tx.print(toString(s.robot));
        tx.print(" DRIVE=");
        tx.print(toString(s.drive));
        tx.print(" CAL=");
        tx.print(s.validity.ready() ? "VALID" : "INVALID");
        tx.print(" ACTUATORS=DISABLED NVS=");
        tx.println(configStorageName(config.storageState()));
    }
    else if (strcmp(cmd.verb, "CONFIG") == 0)
    {
        prefix("OK", cmd);
        tx.print(" CMD=CONFIG STATE=");
        tx.print(configStorageName(config.storageState()));
        tx.print(" SCHEMA=");
        tx.print(config.schema());
        tx.print(" SEQ=");
        tx.print(config.sequence());
        tx.print(" SLOT=");
        tx.println(config.activeSlot());
    }
    else if (strcmp(cmd.verb, "CONFIGSAVE") == 0)
    {
        const bool ok = config.save();
        prefix(ok ? "OK" : "ERR", cmd);
        tx.println(ok ? " CMD=CONFIGSAVE SAVED=1" : " CODE=CONFIG_WRITE_FAILED");
    }
    else if (strcmp(cmd.verb, "HELP") == 0)
    {
        prefix("OK", cmd);
        tx.println(" CMD=HELP COMMANDS=PING,VERSION,STATUS,HELP,DIAG,CONFIG,CONFIGSAVE,TRACE,SERTRACE FORMAT=VERB[ ID=token] CONFIGSAVE=explicit-persist");
    }
    else if (strcmp(cmd.verb, "DIAG") == 0)
    {
        prefix("OK", cmd);
        tx.print(" CMD=DIAG UPTIME_MS=");
        tx.print(millis());
        auto printGate = [](const char *name, const PeriodicGate &g)
        {const auto& s=g.stats();tx.print(' ');tx.print(name);tx.print("_RUN=");tx.print(s.runs);tx.print(' ');tx.print(name);tx.print("_MISS=");tx.print(s.missed);tx.print(' ');tx.print(name);tx.print("_JITTER_US=");tx.print(s.maxJitterUs);tx.print(' ');tx.print(name);tx.print("_EXEC_US=");tx.print(s.maxExecutionUs);tx.print(' ');tx.print(name);tx.print("_INTERVAL_MIN_US=");tx.print(s.minIntervalUs);tx.print(' ');tx.print(name);tx.print("_INTERVAL_MAX_US=");tx.print(s.maxIntervalUs);tx.print(' ');tx.print(name);tx.print("_LATE_RUN=");tx.print(s.lateStarts);
   tx.print(' ');tx.print(name);tx.print("_LATE_EPISODES=");tx.print(s.lateEpisodes);
   tx.print(' ');tx.print(name);tx.print("_MAX_SKIPPED=");tx.print(s.maxSkippedInEpisode); };
        printGate("ACQ", acquisition);
        printGate("PLAN", planning);
        printGate("SUP", supervisor);
        printGate("TEL", telemetry);
        tx.print(" LOOP_MAX_GAP_US=");
        tx.print(loopGap.maxGapUs());
        tx.print(" LOOP_GAP_GT5MS=");
        tx.print(loopGap.gapsAbove5ms());
        tx.print(" LOOP_GAP_GT10MS=");
        tx.print(loopGap.gapsAbove10ms());
        tx.print(" TX_PENDING=");
        tx.print(tx.pending());
        tx.print(" TX_HIGH=");
        tx.print(tx.highWater());
        tx.print(" TX_CAP=");
        tx.print(tx.capacity());
        tx.print(" TX_DROPPED=");
        tx.print(tx.droppedLines());
        tx.print(" TX_FLUSH_MAX_US=");
        tx.print(txFlushMaxUs);
        tx.print(" TX_BACKPRESSURE_POLLS=");
        tx.print(txBackpressurePolls);
        tx.println();
    }
    else if (strcmp(cmd.verb, "TRACE") == 0)
    {
        const auto &e = loopTrace.event();
        prefix("OK", cmd);
        tx.print(" CMD=TRACE MAX_IDLE_GAP_US=");
        tx.print(e.gapUs);
        tx.print(" AT_UPTIME_MS=");
        tx.print(e.uptimeMs);
        tx.print(" PRECEDING_PHASE=");
        tx.print(phaseName(e.preceding));
        tx.print(" CORE=");
        tx.print(e.core);
        tx.print(" MAX_LOOP_WORK_US=");
        tx.print(loopTrace.maxLoopWorkUs());
        tx.print(" MAX_SERIAL_US=");
        tx.print(loopTrace.maxSerialUs());
        tx.print(" MAX_ACQ_US=");
        tx.print(loopTrace.maxPhase(LoopPhase::ACQ));
        tx.print(" MAX_PLAN_US=");
        tx.print(loopTrace.maxPhase(LoopPhase::PLAN));
        tx.print(" MAX_SUP_US=");
        tx.print(loopTrace.maxPhase(LoopPhase::SUP));
        tx.print(" MAX_TEL_US=");
        tx.print(loopTrace.maxPhase(LoopPhase::TEL));
        tx.print(" IDLE_GT5MS=");
        tx.print(loopTrace.gaps5());
        tx.print(" IDLE_GT10MS=");
        tx.println(loopTrace.gaps10());
    }
    else if (strcmp(cmd.verb, "SERTRACE") == 0)
    {
        const auto &e = serialTiming.slowest();
        prefix("OK", cmd);
        tx.print(" CMD=SERTRACE MAX_POLL_US=");
        tx.print(serialTiming.maxPollUs());
        tx.print(" POLL_AT_MS=");
        tx.print(serialTiming.maxPollAtMs());
        tx.print(" POLL_INPUT=");
        tx.print(serialTiming.maxPollHadInput() ? 1 : 0);
        tx.print(" POLL_CORE=");
        tx.print(serialTiming.maxPollCore());
        tx.print(" EMPTY_MAX_US=");
        tx.print(serialTiming.maxEmptyPollUs());
        tx.print(" BUSY_MAX_US=");
        tx.print(serialTiming.maxBusyPollUs());
        tx.print(" AVAILABLE_MAX_US=");
        tx.print(serialTiming.maxStageUs(SerialStage::AVAILABLE));
        tx.print(" READ_MAX_US=");
        tx.print(serialTiming.maxStageUs(SerialStage::READ));
        tx.print(" DISPATCH_MAX_US=");
        tx.print(serialTiming.maxStageUs(SerialStage::DISPATCH));
        tx.print(" SLOW_STAGE=");
        tx.print(serialStageName(e.stage));
        tx.print(" SLOW_US=");
        tx.print(e.durationUs);
        tx.print(" SLOW_AT_MS=");
        tx.print(e.atUptimeMs);
        tx.print(" SLOW_INPUT=");
        tx.print(e.hadInput ? 1 : 0);
        tx.print(" SLOW_CORE=");
        tx.print(e.core);
        tx.print(" EMPTY_POLLS=");
        tx.print(serialTiming.pollsEmpty());
        tx.print(" BUSY_POLLS=");
        tx.println(serialTiming.pollsWithInput());
    }
    else
    {
        prefix("ERR", cmd);
        tx.println(" CODE=UNKNOWN_COMMAND");
    }
}
static void serialPoll()
{
    const uint32_t pollStart = micros();
    const uint8_t core = static_cast<uint8_t>(xPortGetCoreID());
    bool hadInput = false;
    uint32_t budget = SERIAL_BUDGET;
    while (budget--)
    {
        // Pause UART reads when there is no capacity for an entire response.
        // A host flooding commands can still overflow hardware RX; use request/response IDs.
        if (tx.freeSpace() < TX_RESPONSE_RESERVE)
        {
            ++txBackpressurePolls;
            break;
        }
        const uint32_t availableStart = micros();
        const int available = Serial.available();
        serialTiming.sample(SerialStage::AVAILABLE, micros() - availableStart, millis(), hadInput, core);
        if (available <= 0)
            break;
        hadInput = true;
        const uint32_t readStart = micros();
        const int readValue = Serial.read();
        serialTiming.sample(SerialStage::READ, micros() - readStart, millis(), true, core);
        if (readValue < 0)
            continue;
        const char c = static_cast<char>(readValue);
        if (c == '\r')
            continue;
        if (c == '\n')
        {
            const uint32_t dispatchStart = micros();
            if (overflow)
                tx.println("ERR CODE=LINE_TOO_LONG");
            else
            {
                rx[rxLen] = '\0';
                processLine(rx);
            }
            serialTiming.sample(SerialStage::DISPATCH, micros() - dispatchStart, millis(), true, core);
            rxLen = 0;
            overflow = false;
            continue;
        }
        if (static_cast<unsigned char>(c) < 32 || static_cast<unsigned char>(c) > 126)
        {
            overflow = true;
            continue;
        }
        if (!overflow)
        {
            if (rxLen + 1 < sizeof(rx))
                rx[rxLen++] = c;
            else
                overflow = true;
        }
    }
    // Incremental TX pump. Driver capacity is checked before each nonblocking write.
    const uint32_t flushStart = micros();
    tx.flushTo(Serial, TX_FLUSH_MAX);
    const uint32_t flushUs = micros() - flushStart;
    if (flushUs > txFlushMaxUs)
        txFlushMaxUs = flushUs;
    serialTiming.pollDone(micros() - pollStart, hadInput, millis(), core);
}

void setup()
{
    BoardHAL::beginSafe();
    Serial.begin(115200);
    robot.begin();
    config.begin();
    // Sem espera Serial: firmware continua funcional mesmo sem monitor conectado.
    tx.print("EVENT TYPE=BOOT FW=");
    tx.print(FW_VERSION);
    tx.println(" PROTO=1 ACTUATORS=DISABLED");
}
void loop()
{
    const uint32_t now = micros();
    loopTrace.start(now, millis(), static_cast<uint8_t>(xPortGetCoreID()));
    loopGap.sample(now);
    auto measure = [](LoopPhase p, uint32_t begin)
    { loopTrace.phase(p, micros() - begin); };
    if (acquisition.due(now))
    {
        uint32_t start = micros(); /* no physical sensing in phase 0 */
        acquisition.execution(micros() - start);
        measure(LoopPhase::ACQ, start);
    }
    if (planning.due(now))
    {
        uint32_t start = micros();
        planning.execution(micros() - start);
        measure(LoopPhase::PLAN, start);
    }
    if (supervisor.due(now))
    {
        uint32_t start = micros();
        supervisor.execution(micros() - start);
        measure(LoopPhase::SUP, start);
    }
    if (telemetry.due(now))
    {
        uint32_t start = micros();
        telemetry.execution(micros() - start);
        measure(LoopPhase::TEL, start);
    }
    const uint32_t serStart = micros();
    serialPoll();
    measure(LoopPhase::SERIAL_IO, serStart);
    const uint32_t end = micros();
    loopTrace.finish(end, end - now);
}
