#include <fcntl.h>

#define CLOCK_INVALID (-1)
#define CLOCKFD 3
#define FD_TO_CLOCKID(fd)	((clockid_t) ((((unsigned int) ~fd) << 3) | CLOCKFD))
#define CLOCKID_TO_FD(clk)	((unsigned int) ~((clk) >> 3))

#define SIOCETHTOOL	0x8946		/* Ethtool interface		*/

#include "AnanasPacket.h"

#include <linux/ethtool.h>
#include <net/if.h>
#include <sys/ioctl.h>

struct sk_ts_info
{
    int valid;
    int phc_index;
    unsigned int so_timestamping;
    unsigned int tx_types;
    unsigned int rx_filters;
};

int sk_get_ts_info(const char *name, struct sk_ts_info *sk_info)
{
    struct ethtool_ts_info info;
    struct ifreq ifr;
    int fd, err;

    memset(&ifr, 0, sizeof(ifr));
    memset(&info, 0, sizeof(info));
    info.cmd = ETHTOOL_GET_TS_INFO;
    strncpy(ifr.ifr_name, name, IFNAMSIZ - 1);
    ifr.ifr_data = (char *) &info;
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    err = ioctl(fd, SIOCETHTOOL, &ifr);

    close(fd);

    /* copy the necessary data to sk_info */
    memset(sk_info, 0, sizeof(struct sk_ts_info));
    sk_info->valid = 1;
    sk_info->phc_index = info.phc_index;
    sk_info->so_timestamping = info.so_timestamping;
    sk_info->tx_types = info.tx_types;
    sk_info->rx_filters = info.rx_filters;

    return 0;
}

clockid_t phc_open(const char *phc)
{
    clockid_t clkid;
    struct timespec ts;
    struct timex tx;
    int fd;

    memset(&tx, 0, sizeof(tx));

    fd = open(phc, O_RDWR);
    if (fd < 0)
        return CLOCK_INVALID;

    clkid = FD_TO_CLOCKID(fd);
    /* check if clkid is valid */
    if (clock_gettime(clkid, &ts)) {
        close(fd);
        return CLOCK_INVALID;
    }
    if (clock_adjtime(clkid, &tx)) {
        close(fd);
        return CLOCK_INVALID;
    }

    return clkid;
}

static int clkid;

void AnanasPacket::prepare(const int samplesPerBlockExpected, const double sampleRate)
{
    juce::ignoreUnused(sampleRate);
    setSize(sizeof(Header) + 2 * samplesPerBlockExpected * sizeof(int16_t));
    fillWith(0);

    auto device = "enp85s0u2u3u1";

    struct sk_ts_info ts_info;
    char phc_device[19];

    sk_get_ts_info(device, &ts_info);

    snprintf(phc_device, sizeof(phc_device), "/dev/ptp%d", ts_info.phc_index);

    clkid = phc_open(phc_device);
}

uint8_t *AnanasPacket::getAudioData()
{
    return &static_cast<uint8_t *>(getData())[sizeof(Header)];
}

static int64_t prevTime{0};
static int count{0};
static int64_t totalTime{0};

void AnanasPacket::writeHeader()
{
    // timespec now{};
    // clock_gettime(clkid, &now);
    // // DBG("Clock " << clkid << " time is " << now.tv_sec << "." << now.tv_nsec << " or " << ctime(&now.tv_sec));
    //
    // // Include a reproduction offset (1/20 s) in the header timestamp.
    // const int64_t ts{now.tv_sec * nanoSecondsPerSecond + now.tv_nsec + nanoSecondsPerSecond / 20}; // .05 s
    // const int64_t diff{ts - prevTime};
    // if (prevTime != 0) {
    //     totalTime += diff;
    // }
    //
    // if (++count % 1000 == 1) {
    //     DBG("\nClock " << clkid << " time is " << now.tv_sec << "." << now.tv_nsec << " or " << ctime(&now.tv_sec));
    //     auto avgInterval{static_cast<double>(totalTime) / count};
    //     DBG("\nprev: " << prevTime << " current: " << ts << " diff: " << diff);
    //     DBG("Packets sent: " << count << " Average transmission interval: " << avgInterval);
    // }
    // prevTime = ts;
    //
    // header.timestamp = ts;//now.tv_sec * nanoSecondsPerSecond + now.tv_nsec + nanoSecondsPerSecond / 10;
    // copyFrom(&header, 0, sizeof(Header));

    if (firstPacket) {
        firstPacket = false;
        timespec now{};
        clock_gettime(clkid, &now);
        DBG("Clock " << clkid << " time is " << now.tv_sec << "." << now.tv_nsec << " or " << ctime(&now.tv_sec));

        // Include a reproduction offset (1/10 s) in the header timestamp.
        const int64_t ts{now.tv_sec * nanoSecondsPerSecond + now.tv_nsec + nanoSecondsPerSecond / 10}; // .1 s
        const int64_t diff{ts - prevTime};
        if (prevTime != 0) {
            totalTime += diff;
        }

        if (++count % 1000 == 1) {
            DBG("\nClock " << clkid << " time is " << now.tv_sec << "." << now.tv_nsec << " or " << ctime(&now.tv_sec));
            auto avgInterval{static_cast<double>(totalTime) / count};
            DBG("\nprev: " << prevTime << " current: " << ts << " diff: " << diff);
            DBG("Packets sent: " << count << " Average transmission interval: " << avgInterval);
        }
        prevTime = ts;

        header.timestamp = ts;
        copyFrom(&header, 0, sizeof(Header));
    } else {
        // Hm, just bump the timestamp by the expected amount each packet?..
        prevTime += 2666666; // 1e9 * 128/48000
        header.timestamp = prevTime;
        copyFrom(&header, 0, sizeof(Header));
    }
}
