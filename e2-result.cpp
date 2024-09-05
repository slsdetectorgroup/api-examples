#include <sls/Detector.h>

#include <algorithm>
#include <iostream>

int main()
{

    /* 
    Detector returns a Result<T> which is a thin wrapper around
    std::vector. The Result holds the requested info from each detector.
     */
    sls::Result<int> res1{1, 1, 1};
    sls::Result<int> res2{1, 2, 3};


    // Printing each value of the Result is supported with <<
    std::cout << "res1: " << res1 << '\n';
    std::cout << "res2: " << res2 << "\n\n";

    /*
    To reduce the result to one value you can call the squash
    method. This returns the value if all elements has the same
    value. Otherwise a default constructed T
    */
    std::cout << "res1.squash(): " << res1.squash() << '\n';
    std::cout << "res2.squash(): " << res2.squash() << "\n\n";
    
    // It's also possible to specify the value when elements differ
    std::cout << "res1.squash(-1): " << res1.squash(-1) << '\n';
    std::cout << "res2.squash(-1): " << res2.squash(-1) << "\n\n";


    /*
    For error handling it is possible to use the tsquash method. 
    This throws an sls::RuntimeError if the values are different.
    The error message is printed to the consol by default. 
    */
   
    try{
        res2.tsquash("Values are different");
    }catch(sls::RuntimeError &e){
        std::cout << "Do something to handle error\n";
    }

    //Test is values are equal but not throw an exception
    std::cout << '\n';
    std::cout << "res1.equal(): " << std::boolalpha << res1.equal() <<'\n';
    std::cout << "res2.equal(): " << std::boolalpha << res2.equal() << "\n\n";


    //Since Result exposes the begin and end iterators of the vector it can 
    //be passed to standars library algorithms. 
    std::sort(res2.begin(), res2.end(), std::greater<>());
    std::cout << "res2: " << res2 << '\n';
    
} 