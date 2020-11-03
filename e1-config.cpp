

#include <sls/Detector.h>
#include <iostream>

int main(int argc, char *argv[]){

    if (argc != 2){
        std::cout << "Wrong number of arguments provided, exiting.\n";
        return 0;
    }

    sls::Detector det;
    det.loadConfig(argv[1]);
    
    auto type = sls::ToString(det.getDetectorType().squash());
    auto nmod = det.size();

    std::cout << "\n\n";
    std::cout << type << " detector with " << nmod << " modules configured\n";

}