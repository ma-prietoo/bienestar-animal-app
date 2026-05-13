#pragma once

#include <Arduino.h>
#include "data_types.h"

class BufferStore {
public:
  static constexpr size_t CAPACITY = 32;

  void begin() {
    countValue = 0;
    writeIndex = 0;
  }

  void push(const WellnessSample &sample, const WellnessAnalysis &analysis) {
    records[writeIndex] = BufferedRecord { sample, analysis };
    writeIndex = (writeIndex + 1) % CAPACITY;

    if (countValue < CAPACITY) {
      countValue++;
    }
  }

  size_t count() const {
    return countValue;
  }

  void clear() {
    countValue = 0;
    writeIndex = 0;
  }

private:
  BufferedRecord records[CAPACITY];
  size_t countValue = 0;
  size_t writeIndex = 0;
};
