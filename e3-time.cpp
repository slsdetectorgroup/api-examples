#include "Detector.h"
#include <chrono>
#include <iostream>


int main(){

    sls::Detector det;

    //Set exposure time to 500us
    std::chrono::microseconds t0{500};
    det.setExptime(t0);
    std::cout << "exptime: " << det.getExptime() << '\n';


    //Set exposure time in floating point (232.57ms)
    std::chrono::duration<double, std::milli> t1{232.57};
    det.setExptime(std::chrono::duration_cast<std::chrono::nanoseconds>(t1));
    std::cout << "exptime: " << det.getExptime() << '\n';

    //Getting exposure time as an int in ns
    auto r = det.getExptime();
    auto t2 = r.squash();
    int t_ns = t2.count();

    std::cout << "Exptime in ns: " << t_ns << '\n';
    std::cout << "Exptime in s: " << std::chrono::duration<double>(t2).count() << '\n';


}