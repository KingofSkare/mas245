// K. M. Knausgård / MAS245 2017
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <iostream>
#include <time.h>

const char *ifname = "can0";
int canSocketDescriptor;

const double periodicTaskRate = 2.0; // Run task at 0.5 Hz.
const int64_t periodicTaskDtNs = static_cast<int64_t>((1.0/periodicTaskRate) * 1.0e9);

bool createCanSocket(int& socketDescriptor);
void RXTX_CAN(int socketDescriptor);

void myPeriodicTask() {
    static uint64_t ii(0U);
    ++ii;
    std::cout << "Tick " << ii << ".." << std::endl;
    RXTX_CAN(canSocketDescriptor);
}

int main() {
    std::cout << "Periodic tick example for MAS245.." << std::endl;

    if (!createCanSocket(canSocketDescriptor)) {
        std::cout << "Could not create CAN socket" << std::endl;
        return 0;
    }

    bool running = true;
    while(running) {
        struct timespec nextTimerDeadline;
        clock_gettime(CLOCK_MONOTONIC, &nextTimerDeadline);

        myPeriodicTask();

        int64_t nextTvNsec = nextTimerDeadline.tv_nsec + periodicTaskDtNs;
        if (nextTvNsec >= 1000000000L) {
            nextTimerDeadline.tv_sec += nextTvNsec / 1000000000L;
            nextTimerDeadline.tv_nsec = nextTvNsec % 1000000000L;
        }

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &nextTimerDeadline, NULL);
    }

    return 0;
}

bool createCanSocket(int& socketDescriptor) {
    struct ifreq ifr;

    if ((socketDescriptor = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Error while opening CAN socket.");
        return false;
    }

    strcpy(ifr.ifr_name, ifname);
    ioctl(socketDescriptor, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(socketDescriptor, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error in socket bind.");
        return false;
    }

    return true;
}

void RXTX_CAN(int socketDescriptor) {
    int nbytes;
    struct can_frame frame;
    nbytes = read(socketDescriptor, &frame, sizeof(struct can_frame));
    if (nbytes < 0) {
        perror("Read");
    }
    printf("0x%03X [%d]", frame.can_id, frame.can_dlc);
    for (int i = 0; i < frame.can_dlc; i++) {
        printf("%d", frame.data[i]);
    }
    printf("\r\n");
    std::cout << "Frame data 0 original: ";
    printf("%d", frame.data[0]);
    std::cout << std::endl;
    frame.can_id = 0x300;
    frame.data[0] = frame.data[0] * 0x02;
    std::cout << "Frame data 0 modified: ";
    printf("%d", frame.data[0]);
    std::cout << std::endl;
    // Write modified CAN message back to Teensy
    write(socketDescriptor, &frame, sizeof(struct can_frame));
}

