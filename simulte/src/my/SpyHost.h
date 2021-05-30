
#ifndef __LTE_SPYHOST_H
#define __LTE_SPYHOST_H

#include <thread>
#include <chrono>
#include <future>
#include <iostream>
#include <vector>
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/mobility/my/MyMobility_m.h"
#include "ActionInfo_generated.h"

extern "C" {
    #include "unp.h"
}

namespace inet {

class SpyHost : public cSimpleModule {
private:
    int listen_fd, conn_fd;
    std::promise<void> socketExitSignal;
    std::thread th;
    std::vector<uint8_t*> messageQueue;
    cMessage* selfMsg;
public:
	virtual void initialize(int stage) override;
	virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void listen();
    void listen_socket(std::future<void>);
    void str_echo_1(int sockfd);

};

} // namespace inet

#endif // #ifndef __LTE_SPYHOST_H
