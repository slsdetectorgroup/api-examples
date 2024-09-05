

#include <sls/Detector.h>
#include <sls/detectorData.h>

void dataCallback(sls::detectorData *pData, uint64_t iframe, uint32_t isubframe,
                  void *pArg) {
  std::cout << "\n DataCallback"
            << "\n ------------"
            << "\n Frame number      : " << iframe
            << "\n Sub frame number  : " << (int)isubframe
            << "\n progress index    : " << pData->progressIndex << "%"
            << "\n file name prefix  : " << pData->fileName
            << "\n acquisition index : " << pData->fileIndex
            << "\n nx                : " << pData->nx
            << "\n ny                : " << pData->ny
            << "\n databytes         : " << pData->databytes
            << "\n dynamic range     : " << pData->dynamicRange
            << "\n completeSubImages : " << pData->completeImage << std::endl;
}

int main(int argc, char *argv[]) {

  sls::Detector det;
  /** register data call back and client zmq streaming */
  det.registerDataCallback(&dataCallback, NULL);
  /** enable receiver zmq streaming */
  det.setRxZmqDataStream(true);

  /** - ensuring detector status is idle before starting acquisition. exiting if
   * not idle */
  slsDetectorDefs::runStatus status =
      det.getDetectorStatus().squash(slsDetectorDefs::ERROR);
  if (status != slsDetectorDefs::IDLE) {
    std::cout << "Detector not ready: " << sls::ToString(status) << std::endl;
    return 1;
  }

  det.acquire();
  std::cout << "measurement finished" << std::endl;
  return 0;
}