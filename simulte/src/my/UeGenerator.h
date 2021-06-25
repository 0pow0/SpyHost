#ifndef __LTE_UEGENERATOR_H
#define __LTE_UEGENERATOR_H

#include <omnetpp.h>

using namespace omnetpp;

namespace inet { 

class SpyHost;

class UeGenerator {
    using string = std::string;
private:
    cModuleType *ueFactory;
    SpyHost *spy;
    cModule *root;
public:
    int generate(std::string, int, std::string, std::string); 
    void setSpyHost(SpyHost *);
};

} // namespace inet

#endif
