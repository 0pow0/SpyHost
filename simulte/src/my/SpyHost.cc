#include "SpyHost.h"
#include "UeGenerator.h"

using namespace ROC;

namespace inet {

Define_Module(SpyHost); 

SpyHost::~SpyHost() {
    delete ueGen;
}

void SpyHost::initialize(int stage) {
    selfMsg = new cMessage();
    scheduleAt(simTime() + SimTime(1, SIMTIME_S), selfMsg);
    conn_fd = -1;
    listen();
    std::future<void> futureObj = socketExitSignal.get_future();
    std::thread t(&SpyHost::listen_socket, this, std::move(futureObj));
    th = std::move(t);
    ueGen = new UeGenerator();
    ueGen->setSpyHost(this);
}	

void SpyHost::finish() {
    Close(conn_fd);
    Close(listen_fd);
    socketExitSignal.set_value();
    th.join();
    std::cout << "Socket listening thread exited!" << std::endl;
}

void SpyHost::handleMessage(cMessage *msg) {
    //ActionInfo* actionInfo = new ActionInfo("action info");
    //actionInfo->setLat("123");
    //actionInfo->setLng("123");
    //std::cout << "Lat: " << actionInfo->getLat() << std::endl;
    if (msg->isSelfMessage()) {
       //for (int i = 0; i < 5; i++) {
       //    send(i == 4 ? actionInfo : actionInfo->dup(), "out", i);
       //}
        //std::cout << "111 SypHost::handleSelfMessage()" << std::endl;
        if (messageQueue.size() != 0) {
            std::cout << "[SpyHost] SypHost::handleSelfMessage()" << std::endl;
            auto buf_pointer = messageQueue.front();
            // unpack into general roc_info
            auto roc_info = GetROCInfo(buf_pointer);
            // determine roc_type
            auto roc_type = roc_info->info_type();
            switch(roc_type) {
                case ROCType_Create: roc_creation_CB(roc_info);
                                     break;
                case ROCType_Action: roc_action_CB(roc_info);
                                     break;
                default: break;
            }
            messageQueue.pop_front();
            delete buf_pointer;
        } 
        scheduleAt(simTime() + SimTime(500, SIMTIME_MS), selfMsg);
    }
}

void SpyHost::roc_creation_CB(const ROCInfo* roc_info) {
    auto creation_info = static_cast<const CreationInfo*>(roc_info->info());
    auto uav_id = creation_info->uav_id()->str();
    auto lat = creation_info->latitude()->str();
    auto lng = creation_info->longitude()->str();
    auto master_id = creation_info->master_id();
    if (id_to_gateIdx.count(uav_id)) return;
    std::cout << "[SpyHost] roc_creation_CB(): " 
        << "id=" << uav_id 
        << "lat=" << lat 
        << "lng=" << lng 
        << "master_id=" << master_id << std::endl;
    int gate_idx = ueGen->generate(uav_id, master_id, lat, lng);
    id_to_gateIdx.emplace(uav_id, gate_idx);
}

void SpyHost::roc_action_CB(const ROCInfo* roc_info) {
    auto actionInfoRecieved = static_cast<const ROC::ActionInfo*>(roc_info->info());
    auto uav_id = actionInfoRecieved->uav_id()->str();
    auto lat = actionInfoRecieved->latitude()->str();
    auto lng = actionInfoRecieved->longitude()->str();
    if (!id_to_gateIdx.count(uav_id)) return;
    std::cout << "[SpyHost] roc_action_CB(): " 
        << "id=" << uav_id 
        << "lat=" << lat 
        << "lng=" << lng << std::endl;
    inet::ActionInfo* actionInfo = new inet::ActionInfo("action info");
    actionInfo->setLat(lat.c_str());
    actionInfo->setLng(lng.c_str());

    int idx = id_to_gateIdx[uav_id];
    int gateSize = this->gateSize("in");
    if (idx >= 0 && idx < gateSize) {
        send(actionInfo, "out", idx);
    }
    else delete actionInfo;
}

void SpyHost::listen() {
    listen_fd = Socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(62700);
    Inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    
    Bind(listen_fd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    Listen(listen_fd, LISTENQ);
    while (conn_fd == -1) {
        conn_fd = Accept(listen_fd, NULL, NULL); 
    }
    int flags = fcntl(conn_fd, F_GETFL, 0);
    fcntl(conn_fd, F_SETFL, flags | O_NONBLOCK);
    Close(listen_fd);
}

void SpyHost::listen_socket(std::future<void> futureObj) {
    ssize_t n;                                                   
    char buf[MAXLINE];
    while (futureObj.wait_for(std::chrono::milliseconds(1)) ==
std::future_status::timeout) {
        listen_roc(conn_fd);
   }
    std::cout << "Thread End" << std::endl; 
}

void SpyHost::listen_roc(int sockfd) {
    ssize_t n;
    char buf[MAXLINE];
again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0) {
        char* tmp = new char[MAXLINE]; 
        memcpy(tmp, buf, sizeof(buf));
        //uint8_t* buf_pointer = (uint8_t*) tmp;
        //auto actionInfoRecieved = GetActionInfo(buf_pointer);
        //auto id = actionInfoRecieved->uav_id();
        //auto lat = actionInfoRecieved->latitude();
        //auto lng = actionInfoRecieved->longitude();
        messageQueue.push_back((uint8_t*) tmp);
        //std::cout << "size =" << messageQueue.size() << std::endl;
        //std::cout << "@@@Socket::uav id = " << id->str() << std::endl;
        //std::cout << "@@@Socket::lat = " << lat->str() << std::endl;
        //std::cout << "@@@Socket::lng = " << lng->str() << std::endl;
    }
    
    if (n < 0 && errno == EINTR)
        goto again;
    if (n < 0 && errno == EAGAIN)
        goto again;
    else if (n < 0) 
        err_sys("listen_roc: read erro");
}

} // namespace inet
