#include "MyMobility.h"

namespace inet {

Define_Module(MyMobility);

MyMobility::MyMobility() :
    moveTimer(nullptr),
    updateInterval(0),
    stationary(false),
    lastVelocity(Coord::ZERO),
    lastUpdate(0),
    nextChange(-1),
    faceForward(false)
{
}

MyMobility::~MyMobility()
{
    cancelAndDelete(moveTimer);
}

void MyMobility::initialize(int stage)
{
    MobilityBase::initialize(stage);
    EV_TRACE << "initializing MyMobility stage " << stage << endl;
    if (stage == INITSTAGE_LOCAL) {
        moveTimer = new cMessage("move");
        //updateInterval = par("updateInterval");
        //faceForward = par("faceForward");
		//double speed = par("speed");
		//rad heading = deg(fmod(par("initialMovementHeading").doubleValue(), 360));
        //rad elevation = deg(fmod(par("initialMovementElevation").doubleValue(), 360));
        //Coord direction = Quaternion(EulerAngles(heading, -elevation, rad(0))).rotate(Coord::X_AXIS);
		//lastVelocity = direction * speed;
    }
//	else if (stage == INITSTAGE_SINGLE_MOBILITY) {
//        initializeOrientation();
//        initializePosition();
//    }
}

void MyMobility::initializePosition()
{
    MobilityBase::initializePosition();
    lastUpdate = simTime();
    scheduleUpdate();
}

void MyMobility::moveAndUpdate()
{
    simtime_t now = simTime();
    if (nextChange == now || lastUpdate != now) {
        move();
        orient();
        lastUpdate = simTime();
        emitMobilityStateChangedSignal();
    }
}

void MyMobility::orient()
{
    if (faceForward) {
        // determine orientation based on direction
        if (lastVelocity != Coord::ZERO) {
            Coord direction = lastVelocity;
            direction.normalize();
            auto alpha = rad(atan2(direction.y, direction.x));
            auto beta = rad(-asin(direction.z));
            auto gamma = rad(0.0);
            lastOrientation = Quaternion(EulerAngles(alpha, beta, gamma));
        }
    }
}

void MyMobility::handleMessage (cMessage *message) {
	if (message->isSelfMessage()) {
		//std::cout << "MyMobility::handleMessage: self message" << std::endl;	
		//handleSelfMessage(message);
	}
	else {
		//ActionInfo* actionInfo = check_and_cast<ActionInfo*>(message);
		ActionInfo* actionInfo = (ActionInfo*) message;
		auto lat = actionInfo->getLat();
		auto lng = actionInfo->getLng();
		//std::cout << "MyMobility::handleMessage:" << "Lat: " << lat <<
		//	"Lng: " << lng << std::endl;
		myMove(actionInfo);
		//std::cout << (message->getName()) << std::endl;
		//ActionInfo* actionInfo = dynamic_cast<ActionInfo*>(message);
		//if (actionInfo) {
		//	std::cout << "Success!" << std::endl;	
		//}
		//else std::cout << "Fail!" << std::endl;
		//ActionInfo* actionInfo = check_and_cast<ActionInfo*>(message);
		delete actionInfo;
	}
}

void MyMobility::myMove(ActionInfo* actionInfo) {
    auto coordinateSystem = getModuleFromPar<IGeographicCoordinateSystem>(par("coordinateSystemModule"), this, false);
	auto lat = deg(std::stod(actionInfo->getLat()));
	auto lng = deg(std::stod(actionInfo->getLng()));
	auto alt = m(10);
	lastPosition = coordinateSystem->computeSceneCoordinate(GeoCoord(lat, lng, alt));
	//auto lng = deg(actionInfo->getLng());
	orient();
    lastUpdate = simTime();
    emitMobilityStateChangedSignal();
}

void MyMobility::move() {
	double elapsedTime = (simTime() - lastUpdate).dbl();
    lastPosition += lastVelocity * elapsedTime;
    //std::cout << "########## MyMobility::move " << "time= " << elapsedTime << "lastVelocity " << lastVelocity << std::endl;
}

void MyMobility::handleSelfMessage(cMessage *message) {
    //moveAndUpdate();
    //scheduleUpdate();
}

void MyMobility::scheduleUpdate()
{
    cancelEvent(moveTimer);
    if (!stationary && updateInterval != 0) {
        // periodic update is needed
        simtime_t nextUpdate = simTime() + updateInterval;
        if (nextChange != -1 && nextChange < nextUpdate)
            // next change happens earlier than next update
            scheduleAt(nextChange, moveTimer);
        else
            // next update happens earlier than next change or there is no change at all
            scheduleAt(nextUpdate, moveTimer);
    }
    else if (nextChange != -1)
        // no periodic update is needed
        scheduleAt(nextChange, moveTimer);
}

Coord MyMobility::getCurrentPosition()
{
    moveAndUpdate();
    return lastPosition;
}

Coord MyMobility::getCurrentVelocity()
{
    moveAndUpdate();
    return lastVelocity;
}

Quaternion MyMobility::getCurrentAngularPosition()
{
    moveAndUpdate();
    return lastOrientation;
}

Quaternion MyMobility::getCurrentAngularVelocity()
{
    moveAndUpdate();
    return lastAngularVelocity;
}

} // namespace inet
