#include "AnanasPacket.h"

void AnanasPacket::prepare(const int samplesPerBlockExpected, const double sampleRate)
{
    juce::ignoreUnused(sampleRate);
    setSize(sizeof(Header) + 2 * samplesPerBlockExpected * sizeof(int16_t));
    fillWith(0);
}

uint8_t *AnanasPacket::getAudioData()
{
    return &static_cast<uint8_t *>(getData())[sizeof(Header)];
}

void AnanasPacket::writeHeader()
{
    header.timestamp += kNanoSecondsPerPacket;
    timestampRemainder += kNanoSecondsPerPacketRemainder;
    if (timestampRemainder > 1) {
        header.timestamp += 1;
        timestampRemainder -= 1;
    }
    copyFrom(&header, 0, sizeof(Header));
}

void AnanasPacket::setTime(timespec ts)
{
    // Set the time a little ahead; a reproduction offset, and something to
    // compensate for the fact that it took a little while for the follow-up
    // message to arrive.
    header.timestamp = ts.tv_sec * kNanoSecondsPerSecond + ts.tv_nsec + kNanoSecondsPerSecond / 10;
}
