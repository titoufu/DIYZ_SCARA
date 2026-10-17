#pragma once
#include <Arduino.h>

namespace Diyz {
// Fixed-memory TX queue for use by loopTask only. No locking is provided.
// pending() excludes bytes already handed to the UART driver/FIFO.
// A line is atomically enqueued on newline. Oversize/full lines are discarded,
// and counted; the caller must reserve capacity and rate-limit input.
template <size_t RING_SIZE, size_t LINE_SIZE>
class TxQueue : public Print {
 static_assert(LINE_SIZE >= 2, "LINE_SIZE too small");
 static_assert(RING_SIZE >= LINE_SIZE, "RING_SIZE must hold at least one line");
public:
 using Print::write;
 size_t write(uint8_t c) override {
   if (c == '\n') {
     if (lineOverflow_) ++droppedLines_;
     else { line_[lineLen_++] = c; commitLine(); }
     lineLen_ = 0; lineOverflow_ = false;
     return 1;
   }
   if (lineOverflow_) return 1;
   if (lineLen_ < LINE_SIZE - 1) line_[lineLen_++] = c;
   else lineOverflow_ = true;
   return 1;
 }
 size_t write(const uint8_t* buffer, size_t size) override {
   if (!buffer) return 0;
   for (size_t i=0; i<size; ++i) write(buffer[i]);
   return size;
 }
 template <typename Port>
 size_t flushTo(Port& port, size_t maxBytes) {
   if (!count_ || !maxBytes) return 0;
   const int room = port.availableForWrite();
   if (room <= 0) return 0;
   size_t n = static_cast<size_t>(room);
   if (n > maxBytes) n = maxBytes;
   if (n > count_) n = count_;
   size_t sent=0;
   while (sent<n) {
     size_t chunk=n-sent;
     const size_t contiguous=RING_SIZE-tail_;
     if (chunk>contiguous) chunk=contiguous;
     const size_t written=port.write(&ring_[tail_],chunk);
     if(written>chunk) break; // defensive guard for faulty Port implementation
     tail_=(tail_+written)%RING_SIZE;
     count_-=written; sent+=written;
     if(written<chunk) break;
   }
   return sent;
 }
 size_t pending() const { return count_; }
 size_t freeSpace() const { return RING_SIZE-count_; }
 size_t highWater() const { return highWater_; }
 uint32_t droppedLines() const { return droppedLines_; }
 static constexpr size_t capacity() { return RING_SIZE; }
private:
 void commitLine() {
   if (RING_SIZE-count_<lineLen_) { ++droppedLines_; return; }
   for(size_t i=0;i<lineLen_;++i) {
     ring_[head_]=line_[i]; head_=(head_+1)%RING_SIZE;
   }
   count_+=lineLen_;
   if (count_>highWater_) highWater_=count_;
 }
 uint8_t ring_[RING_SIZE]{};
 uint8_t line_[LINE_SIZE]{};
 size_t head_=0,tail_=0,count_=0,highWater_=0,lineLen_=0;
 bool lineOverflow_=false;
 uint32_t droppedLines_=0;
};
} // namespace Diyz
