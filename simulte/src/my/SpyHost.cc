#include "SpyHost.h"

using namespace ROC;

namespace inet {

Define_Module(SpyHost); 

void SpyHost::initialize(int stage) {
    selfMsg = new cMessage();
    scheduleAt(simTime() + SimTime(1, SIMTIME_S), selfMsg);
    conn_fd = -1;
    listen();
    std::future<void> futureObj = socketExitSignal.get_future();
    std::thread t(&SpyHost::listen_socket, this, std::move(futureObj));
    th = std::move(t);
//    cModuleType *moduleType = cModuleType::get("lte.corenetwork.nodes.MyUe");
//    cModule *module = moduleType->create("dmc1", this);
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
            std::cout << "222 SypHost::handleSelfMessage()" << std::endl;
            auto buf_pointer = messageQueue.back();
            auto actionInfoRecieved = GetActionInfo(buf_pointer);
            auto id = actionInfoRecieved->uav_id();
            auto lat = actionInfoRecieved->latitude();
            auto lng = actionInfoRecieved->longitude();
            std::cout << "@@@Socket::uav id = " << id->str() << std::endl;
            std::cout << "@@@Socket::lat = " << lat->str() << std::endl;
            std::cout << "@@@Socket::lng = " << lng->str() << std::endl;

            inet::ActionInfo* actionInfo = new inet::ActionInfo("action info");
            actionInfo->setLat(lat->str().c_str());
            actionInfo->setLng(lng->str().c_str());

            int idx = std::stoi(id->str());
            if (idx >= 0 && idx < 5) {
                std::cout << "333 SypHost::handleSelfMessage()" << std::endl;
                send(actionInfo, "out", idx);
            }
            else delete actionInfo;

            messageQueue.pop_back();
            delete buf_pointer;
        } 

        scheduleAt(simTime() + SimTime(500, SIMTIME_MS), selfMsg);
    }
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
        str_echo_1(conn_fd);
   }
    std::cout << "Thread End" << std::endl; 
}

void SpyHost::str_echo_1(int sockfd) {
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
        err_sys("str_echo: read erro");
}

} // namespace inet
