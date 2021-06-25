#include "UeGenerator.h"
#include "SpyHost.h"

namespace inet {

void UeGenerator::setSpyHost(SpyHost *spyHost) {
    spy = spyHost;
    ueFactory = cModuleType::get("lte.corenetwork.nodes.MyUe");
    root = getSimulation()->getModuleByPath("<root>");
}

int UeGenerator::generate(string uav_id, int masterId, string lat, string lng) {
    // increase in gate and out gate both by one
    int inGateSize = spy->gateSize("in");
    int outGateSize = spy->gateSize("out");
    assert(inGateSize == outGateSize);
    inGateSize++;
    outGateSize++;
    spy->setGateSize("in", inGateSize);
    spy->setGateSize("out", outGateSize);
    // create new Ue module
    string ue_name = "ue" + uav_id;
    cModule *ue = ueFactory->create(ue_name.c_str(), root);
    // set Ue mac ids to connect to eNB
    ue->par("masterId") = masterId;
    ue->par("macCellId") = masterId;
    // create channel
    cIdealChannel *channel1 = cIdealChannel::create(ue_name.c_str());
    std::cout << spy->gateSize("in") << " " << inGateSize << std::endl;
    cGate *spyIn = spy->gate("in", inGateSize-1);
    ue->gate("myMobilityOut")->connectTo(spyIn, channel1);
    cIdealChannel *channel2 = cIdealChannel::create(ue_name.c_str());
    cGate *ueIn = ue->gate("myMobilityIn");
    spy->gate("out", outGateSize-1)->connectTo(ueIn, channel2);
    // set parameters
    ue->par("numApps") = 1;
    ue->par("mobilityType") = "MyMobility";
    //auto app = ue->getSubmodule("app", 0);
    //app->par("typename") = "VoIPReceiver";
    //ue->par("appTypename") = "VoIPReceiver";
    // finalize and setup
    ue->finalizeParameters();
    ue->buildInside();
    auto lteNic = ue->getSubmodule("lteNic");
    auto ip2lte = lteNic->getSubmodule("ip2lte");
    ip2lte->par("routingTableModule") = "^.^.ipv4.routingTable";
    auto mobility = ue->getSubmodule("mobility");
    mobility->par("initialLatitude") = std::stod(lat);
    mobility->par("initialLongitude") = std::stod(lng);
    mobility->par("initialAltitude") = 0.0;
    ue->scheduleStart(SimTime());
    ue->callInitialize();
    return inGateSize-1;
}

}
