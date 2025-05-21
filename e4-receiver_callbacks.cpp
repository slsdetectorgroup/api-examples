// Instead of the slsReceiver
#include <sls/Receiver.h>
#include "sls/ToString.h"
#include "sls/logger.h"

#include <csignal> //SIGINT
#include <iostream>
#include <semaphore.h>
#include <sys/syscall.h>
#include <unistd.h>

sem_t semaphore;

void sigInterruptHandler(int p) { sem_post(&semaphore); }

/**
 * Start Acquisition Call back (slsMultiReceiver writes data if file write
 * enabled) if registerCallBackRawDataReady or
 * registerCallBackRawDataModifyReady registered, users get data
 */
int StartAcq(const slsDetectorDefs::startCallbackHeader callbackHeader,
             void *objectPointer) {
    LOG(sls::logINFOBLUE) << "#### Start Acquisition:"
                          << "\n\t["
                          << "\n\tUDP Port : "
                          << sls::ToString(callbackHeader.udpPort)
                          << "\n\tDynamic Range : "
                          << callbackHeader.dynamicRange
                          << "\n\tDetector Shape : "
                          << sls::ToString(callbackHeader.detectorShape)
                          << "\n\tImage Size : " << callbackHeader.imageSize
                          << "\n\tFile Path : " << callbackHeader.filePath
                          << "\n\tFile Name : " << callbackHeader.fileName
                          << "\n\tFile Index : " << callbackHeader.fileIndex
                          << "\n\tQuad Enable : " << callbackHeader.quad
                          << "\n\tAdditional Json Header : "
                          << sls::ToString(callbackHeader.addJsonHeader)
                          << "\n\t]";
    return 0;
}

/** Acquisition Finished Call back */
void AcquisitionFinished(
    const slsDetectorDefs::endCallbackHeader callbackHeader,
    void *objectPointer) {
    LOG(sls::logINFOBLUE) << "#### AcquisitionFinished:"
                          << "\n\t["
                          << "\n\tUDP Port : "
                          << sls::ToString(callbackHeader.udpPort)
                          << "\n\tComplete Frames : "
                          << sls::ToString(callbackHeader.completeFrames)
                          << "\n\tLast Frame Index : "
                          << sls::ToString(callbackHeader.lastFrameIndex)
                          << "\n\t]";
}

/** Define Colors to print data call back in different colors for different
 * recievers */
#define PRINT_IN_COLOR(c, f, ...)                                              \
    printf("\033[%dm" f RESET, 30 + c + 1, ##__VA_ARGS__)

    
/**
 * Get Receiver Data Call back
 * Prints in different colors(for each receiver process) the different headers
 * for each image call back.
 */
void GetData(slsDetectorDefs::sls_receiver_header &header,
             slsDetectorDefs::dataCallbackHeader callbackHeader,
             char *dataPointer, size_t &imageSize, void *objectPointer) {

    slsDetectorDefs::sls_detector_header detectorHeader = header.detHeader;

    PRINT_IN_COLOR(
        (callbackHeader.udpPort % 10),
        "#### GetData: "
        "\n\tCallback Header: "
        "\n\t["
        "\n\tUDP Port: %u"
        "\n\tShape: [%u, %u]"
        "\n\tAcq Index : %lu"
        "\n\tFrame Index :%lu"
        "\n\tProgress : %.2f%%"
        "\n\tCompelte Image :%s"
        "\n\tFlip Rows :%s"
        "\n\tAdditional Json Header : %s"
        "\n\t]"
        "\n\ttReceiver Header: "
        "\n\t["
        "\n\tFrame Number : %lu"
        "\n\tExposure Length :%u"
        "\n\tPackets Caught :%u"
        "\n\tDetector Specific 1: %lu"
        "\n\tTimestamp : %lu"
        "\n\tModule Id :%u"
        "\n\tRow : %u"
        "\n\tColumn :%u"
        "\n\tDetector Specific 2 : %u"
        "\n\tDetector Specific 3 : %u"
        "\n\tDetector Specific 4 : %u"
        "\n\tDetector Type : %s"
        "\n\tVersion: %u"
        "\n\t]"
        "\n\tFirst Byte Data: 0x%x"
        "\n\tImage Size: %zu\n\n",
        callbackHeader.udpPort, callbackHeader.shape.x, callbackHeader.shape.y,
        callbackHeader.acqIndex, callbackHeader.frameIndex,
        callbackHeader.progress,
        sls::ToString(callbackHeader.completeImage).c_str(),
        sls::ToString(callbackHeader.flipRows).c_str(),
        sls::ToString(callbackHeader.addJsonHeader).c_str(),
        detectorHeader.frameNumber, detectorHeader.expLength,
        detectorHeader.packetNumber, detectorHeader.detSpec1,
        detectorHeader.timestamp, detectorHeader.modId, detectorHeader.row,
        detectorHeader.column, detectorHeader.detSpec2, detectorHeader.detSpec3,
        detectorHeader.detSpec4, sls::ToString(detectorHeader.detType).c_str(),
        detectorHeader.version,
        // header->packetsMask.to_string().c_str(),
        ((uint8_t)(*((uint8_t *)(dataPointer)))), imageSize);
}


int main(int argc, char *argv[]) {

  sem_init(&semaphore, 1, 0);


  // Catch signal SIGINT to close files and call destructors properly
  struct sigaction sa;
  sa.sa_flags = 0;                     // no flags
  sa.sa_handler = sigInterruptHandler; // handler function
  sigemptyset(&sa.sa_mask); // dont block additional signals during invocation
                            // of handler
  if (sigaction(SIGINT, &sa, nullptr) == -1) {
    std::cout << "Could not set handler function for SIGINT";
  }

  // if socket crash, ignores SISPIPE, prevents global signal handler
  // subsequent read/write to socket gives error - must handle locally
  struct sigaction asa;
  asa.sa_flags = 0;          // no flags
  asa.sa_handler = SIG_IGN;  // handler function
  sigemptyset(&asa.sa_mask); // dont block additional signals during
                             // invocation of handler
  if (sigaction(SIGPIPE, &asa, nullptr) == -1) {
    std::cout << "Could not set handler function for SIGPIPE";
  }

  try {
    sls::Receiver r(argc, argv);

    // register call backs
    /** - Call back for start acquisition */
    std::cout << "Registering 	StartAcq()";
    r.registerCallBackStartAcquisition(StartAcq, nullptr);

    /** - Call back for acquisition finished */
    std::cout << "Registering 	AcquisitionFinished()";
    r.registerCallBackAcquisitionFinished(AcquisitionFinished, nullptr);

    /* 	- Call back for raw data */
    std::cout << "Registering GetData()";
    r.registerCallBackRawDataReady(GetData, nullptr);

    std::cout << "[ Press \'Ctrl+c\' to exit ]";
    sem_wait(&semaphore);
    sem_destroy(&semaphore);
  } catch (...) {
    // pass
  }
  
  return 0;
}