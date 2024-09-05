
#include <sls/Receiver.h>

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
int StartAcq(const std::string &filePath, const std::string &fileName,
             uint64_t fileIndex, size_t imageSize, void *objectPointer) {
    std::cout << "#### StartAcq:  filePath:" << filePath
                          << "  fileName:" << fileName
                          << " fileIndex:" << fileIndex
                          << "  imageSize:" << imageSize << " ####" << std::endl;
    return 0;
}

/** Acquisition Finished Call back */
void AcquisitionFinished(uint64_t framesCaught, void *objectPointer) {
    std::cout << "#### AcquisitionFinished: framesCaught:"
                          << framesCaught << " ####" << std::endl;
}

/**
 * Get Receiver Data Call back
 * Prints in different colors(for each receiver process) the different headers
 * for each image call back.
 */
void GetData(slsDetectorDefs::sls_receiver_header &header, char *dataPointer,
             size_t imageSize, void *objectPointer) {
    slsDetectorDefs::sls_detector_header detectorHeader = header.detHeader;

    printf(
        "#### %d %d GetData: ####\n"
        "frameNumber: %lu\t\texpLength: %u\t\tpacketNumber: %u\t\tdetSpec1: %lu"
        "\t\ttimestamp: %lu\t\tmodId: %u\t\t"
        "row: %u\t\tcolumn: %u\t\tdetSpec2: %u\t\tdetSpec3: %u"
        "\t\tdetSpec4: %u\t\tdetType: %u\t\tversion: %u"
        //"\t\tpacketsMask:%s"
        "\t\tfirstbytedata: 0x%x\t\tdatsize: %zu\n\n",
        detectorHeader.column, detectorHeader.row,
        (long unsigned int)detectorHeader.frameNumber, detectorHeader.expLength,
        detectorHeader.packetNumber, (long unsigned int)detectorHeader.detSpec1,
        (long unsigned int)detectorHeader.timestamp, detectorHeader.modId,
        detectorHeader.row, detectorHeader.column, detectorHeader.detSpec2,
        detectorHeader.detSpec3, detectorHeader.detSpec4,
        detectorHeader.detType, detectorHeader.version,
        // header->packetsMask.to_string().c_str(),
        ((uint8_t)(*((uint8_t *)(dataPointer)))), imageSize);
}

/**
 * Get Receiver Data Call back (modified)
 * Prints in different colors(for each receiver process) the different headers
 * for each image call back.
 * @param modifiedImageSize new data size in bytes after the callback.
 * This will be the size written/streamed. (only smaller value is allowed).
 */
void GetData(slsDetectorDefs::sls_receiver_header &header, char *dataPointer,
             size_t &modifiedImageSize, void *objectPointer) {
    slsDetectorDefs::sls_detector_header detectorHeader = header.detHeader;

    printf(
        "#### %d %d GetData: ####\n"
        "frameNumber: %lu\t\texpLength: %u\t\tpacketNumber: %u\t\tdetSpec1: %lu"
        "\t\ttimestamp: %lu\t\tmodId: %u\t\t"
        "row: %u\t\tcolumn: %u\t\tdetSpec2: %u\t\tdetSpec3: %u"
        "\t\tdetSpec4: %u\t\tdetType: %u\t\tversion: %u"
        //"\t\tpacketsMask:%s"
        "\t\tfirstbytedata: 0x%x\t\tdatsize: %zu\n\n",
        detectorHeader.column, detectorHeader.row,
        (long unsigned int)detectorHeader.frameNumber, detectorHeader.expLength,
        detectorHeader.packetNumber, (long unsigned int)detectorHeader.detSpec1,
        (long unsigned int)detectorHeader.timestamp, detectorHeader.modId,
        detectorHeader.row, detectorHeader.column, detectorHeader.detSpec2,
        detectorHeader.detSpec3, detectorHeader.detSpec4,
        detectorHeader.detType, detectorHeader.version,
        // header->packetsMask.to_string().c_str(),
        *reinterpret_cast<uint8_t *>(dataPointer), modifiedImageSize);

    // if data is modified, eg ROI and size is reduced
    modifiedImageSize = 26000;
}

int main(int argc, char *argv[]) {

  sem_init(&semaphore, 1, 0);

  std::cout << "Created [ Tid: " << syscall(SYS_gettid) << " ]";

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
  std::cout << "Exiting [ Tid: " << syscall(SYS_gettid) << " ]";
  std::cout << "Exiting Receiver";
  return 0;
}