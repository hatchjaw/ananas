#include "Packet.h"
#include "Utils.h"

void ananas::Packet::prepare(const int numChannels, const int samplesPerBlockExpected, const double sampleRate)
{
    setSize(sizeof(Header) + numChannels * samplesPerBlockExpected * sizeof(int16_t));
    fillWith(0);
    header.numChannels = numChannels;
    header.numFrames = samplesPerBlockExpected;

    // Compute the nanosecond packet timestamp interval. This may not be an
    // integer, so calculate the remainder too, so this can be accumulated
    // (somewhat) accurately.
    nsPerPacket = Constants::NSPS * samplesPerBlockExpected / static_cast<int>(sampleRate);
    nsPerPacketRemainder = static_cast<double>(Constants::NSPS) * samplesPerBlockExpected / static_cast<int>(sampleRate) - static_cast<double>(nsPerPacket);

    halfClientBufferDuration = (static_cast<double>(nsPerPacket) + nsPerPacketRemainder) * 25;

    DBG(samplesPerBlockExpected << "/" << sampleRate << " = " <<
        nsPerPacket << " + " << nsPerPacketRemainder << " ns per block.");
}

uint8_t *ananas::Packet::getAudioData()
{
    return &static_cast<uint8_t *>(getData())[sizeof(Header)];
}

void ananas::Packet::writeHeader()
{
    header.timestamp += nsPerPacket;
    timestampRemainder += nsPerPacketRemainder;
    if (timestampRemainder > 1) {
        header.timestamp += 1;
        timestampRemainder -= 1;
    }
    copyFrom(&header, 0, sizeof(Header));
}

void ananas::Packet::setTime(timespec ts)
{
    // Set the time a little ahead; a reproduction offset, and something to
    // compensate for the fact that it took a little while for the follow-up
    // message to arrive.
    const auto newTime{ts.tv_sec * Constants::NSPS + ts.tv_nsec + Constants::PacketOffsetNs};

    // If the difference between the new time and the current packet timestamp
    // exceeds what can possibly be available at the client, update the header
    // timestamp.
    const auto timestampDiff{static_cast<double>(newTime - header.timestamp)};

    if (timestampDiff > halfClientBufferDuration || timestampDiff < -halfClientBufferDuration) {
        DBG("Timestamp diff is " << timestampDiff << "... Setting packet timestamp to " << newTime);
        header.timestamp = newTime;
    }
}

int64_t ananas::Packet::getTime() const
{
    return header.timestamp;
}
