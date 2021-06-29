
#ifndef __LTE_SPYHOST_H
#define __LTE_SPYHOST_H

#include <thread>
#include <chrono>
#include <future>
#include <iostream>
#include <vector>
#include <deque>
#include <unordered_map>
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/mobility/my/MyMobility_m.h"
#include "ROC_generated.h"

extern "C" {
    #include "unp.h"
}

namespace inet {
class UeGenerator;

class SpyHost : public cSimpleModule {
private:
    int listen_fd, conn_fd;
    std::promise<void> socketExitSignal;
    std::thread th;
    std::deque<uint8_t*> messageQueue;
    cMessage* selfMsg;
    UeGenerator *ueGen = nullptr;
    void roc_creation_CB(const ROC::ROCInfo*);
    void roc_action_CB(const ROC::ROCInfo*);
    void roc_deletion_CB(const ROC::ROCInfo*);
    std::unordered_map<std::string, int> id_to_gateIdx;
public:
	virtual void initialize(int stage) override;
	virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void listen();
    void listen_socket(std::future<void>);
    void listen_roc(int sockfd);
    ~SpyHost();
};

} // namespace inet

#endif // #ifndef __LTE_SPYHOST_H
