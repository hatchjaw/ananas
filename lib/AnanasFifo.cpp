#include "AnanasFifo.h"

AnanasFifo::AnanasFifo()
{
    startTimerHz(2);
}

static int nWrites{0}, nReads{0};

void AnanasFifo::write(const juce::AudioBuffer<float> *src)
{
    const auto writeHandle{fifo.write(src->getNumSamples())};

    for (auto ch{0}; ch < src->getNumChannels(); ++ch) {
        const auto readPointer{src->getReadPointer(ch)};
        buffer.copyFrom(ch, writeHandle.startIndex1, readPointer, writeHandle.blockSize1);
        buffer.copyFrom(ch, writeHandle.startIndex2, readPointer + writeHandle.blockSize1, writeHandle.blockSize2);
    }

    ++nWrites;

    readyForRead.signal();
}

void AnanasFifo::read(uint8_t *dest, const int numSamples)
{
    juce::ignoreUnused(readyForRead.wait());

    const juce::ScopedLock lock{mutex};

    const auto readHandle{fifo.read(numSamples)};
    const auto blockTwoOffset{readHandle.blockSize1 * 2 * sizeof(int16_t)}; // 2 is numChannels...

    if (readHandle.blockSize1 + readHandle.blockSize2 != 128) {
        DBG("Read num samples: " << readHandle.blockSize1 + readHandle.blockSize2);
    }

    for (auto ch{0}; ch < buffer.getNumChannels(); ++ch) {
        converter.convertSamples(dest, ch, buffer.getReadPointer(ch, readHandle.startIndex1), 0, readHandle.blockSize1);
        converter.convertSamples(&dest[blockTwoOffset], ch, buffer.getReadPointer(ch, readHandle.startIndex2), 0, readHandle.blockSize2);
    }

    ++nReads;
}

void AnanasFifo::timerCallback()
{
#if JUCE_DEBUG
    std::cout << '\r' << "FIFO size:" << std::setw(6) << fifo.getTotalSize() <<
            " | frames ready:" << std::setw(6) << fifo.getNumReady() <<
            " | free space:" << std::setw(6) << fifo.getFreeSpace() <<
            std::flush;
#endif
}
